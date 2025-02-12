//
// Created by chaid on 11/02/2025.
//

#include "optimizer.h"
#include <chrono>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <future>
#include <cmath>
#include <functional>

// Initialisation de SUPPORTED_ARRAYS (pour information)
const std::vector<std::string> Optimizer::SUPPORTED_ARRAYS = {"list", "tuple", "np.ndarray"};

// -------------------- Constructeur --------------------
Optimizer::Optimizer(const std::map<std::string, std::string>& kwargs)
    : epoch(0), pop_size(0), mode("single"), n_workers(1), name(""),
      nfe_counter(-1), generator(std::random_device{}())
{
    __set_keyword_arguments(kwargs);
    // Initialisation des historiques
    list_global_best.clear();
    list_current_best.clear();
    list_global_best_fit.clear();
    list_current_best_fit.clear();
    list_global_worst.clear();
    list_current_worst.clear();
    list_epoch_time.clear();

    if (name.empty()) {
        // On peut utiliser le nom de la classe ici
        name = "Optimizer";
    }
}

// -------------------- Méthodes internes --------------------
void Optimizer::__set_keyword_arguments(const std::map<std::string, std::string>& kwargs) {
    // Pour chaque clé, on affecte la valeur aux attributs connus
    for (const auto& pair : kwargs) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        if (key == "epoch") {
            epoch = std::stoi(value);
        } else if (key == "pop_size") {
            pop_size = std::stoi(value);
        } else if (key == "mode") {
            mode = value;
        } else if (key == "n_workers") {
            n_workers = std::stoi(value);
        } else if (key == "name") {
            name = value;
        }
        // D'autres clés peuvent être ajoutées si nécessaire
    }
}

void Optimizer::store_initial_best_worst(const Agent& best, const Agent& worst) {
    list_global_best.push_back(best.copy());
    list_current_best.push_back(best.copy());
    list_global_best_fit.push_back(best.target.fitness());
    list_current_best_fit.push_back(best.target.fitness());
    list_global_worst.push_back(worst.copy());
    list_current_worst.push_back(worst.copy());
}

std::string Optimizer::get_name() const {
    return name;
}

void Optimizer::initialize_variables() {
    // Par défaut, ne fait rien (à surcharger)
}

void Optimizer::before_initialization(const std::vector<std::vector<double>>& starting_solutions) {
    if (starting_solutions.empty()) {
        return;
    }
    if (starting_solutions.size() == static_cast<size_t>(pop_size)) {
        // On suppose que chaque solution est un vecteur dont la taille correspond à problem->n_dims
        pop.clear();
        for (const auto& sol : starting_solutions) {
            if (sol.size() == static_cast<size_t>(problem->n_dims)) {
                pop.push_back(generate_agent(sol));
            } else {
                throw std::invalid_argument("Invalid starting_solutions. It should be a list of positions or 2D matrix of positions only.");
            }
        }
    } else {
        throw std::invalid_argument("Invalid starting_solutions. It should be a list/2D matrix of positions with same length as pop_size.");
    }
}

void Optimizer::initialization() {
    if (pop.empty()) {
        pop = generate_population(pop_size);
    }
}

void Optimizer::after_initialization() {
    // Récupération des meilleurs et pires agents de la population initiale
    auto tuple_agents = get_special_agents(pop, 1, 1, problem->minmax);
    std::vector<Agent> best_agents = std::get<1>(tuple_agents);
    std::vector<Agent> worst_agents = std::get<2>(tuple_agents);
    if (!best_agents.empty() && !worst_agents.empty()) {
        g_best = best_agents[0];
        g_worst = worst_agents[0];
        store_initial_best_worst(g_best, g_worst);
    }
}

void Optimizer::before_main_loop() {
    // Par défaut, ne fait rien
}

void Optimizer::evolve(int /*epoch*/) {
    // À surcharger dans les algorithmes dérivés
}
void Optimizer::check_problem(const std::variant<std::shared_ptr<Problem>, std::map<std::string, std::string>>& problem_param, int seed) {
    if (std::holds_alternative<std::shared_ptr<Problem>>(problem_param)) {
        // Si 'problem' est déjà une instance de Problem
        auto prob = std::get<std::shared_ptr<Problem>>(problem_param);
        prob->set_seed(seed);
        this->problem = prob;
    }
    // Recréer le générateur aléatoire avec la graine donnée
    generator.seed(seed);
    // Réinitialiser la population et les meilleurs/pires agents
    pop.clear();
    g_best = Agent();
    g_worst = Agent();
}

Agent Optimizer::solve(const std::variant<std::shared_ptr<Problem>, std::map<std::string, std::string>>& problem_var,
                         const std::vector<std::vector<double>>& starting_solutions,
                         int seed) {
    check_problem(problem_var, seed);
    initialize_variables();
    before_initialization(starting_solutions);
    initialization();
    after_initialization();

    before_main_loop();
    for (int ep = 1; ep <= epoch; ep++) {
        auto start = std::chrono::high_resolution_clock::now();
        evolve(ep);
        // Mise à jour de la meilleure solution globale
        auto update_result = update_global_best_agent(pop, true);
        pop = update_result.first;
        g_best = update_result.second;
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        list_epoch_time.push_back(elapsed);
    }
    track_optimize_process();
    return g_best;
}

void Optimizer::track_optimize_process() {
    if (!list_global_best.empty()) {
        // Supprime la première solution de chaque historique
        list_global_best.erase(list_global_best.begin());
        list_current_best.erase(list_current_best.begin());
        list_global_worst.erase(list_global_worst.begin());
        list_current_worst.erase(list_current_worst.begin());
    }
}

Agent Optimizer::generate_empty_agent(const std::vector<double>& solution) {
    std::vector<double> sol = solution;
    if (sol.empty()) {
        // On suppose que Problem::generate_solution(true) renvoie un std::vector<double>
        auto sol_variant = problem->generate_solution(true);
        if (std::holds_alternative<std::vector<double>>(sol_variant)) {
            sol = std::get<std::vector<double>>(sol_variant);
        } else {
            // Si la solution est retournée sous forme segmentée, on la concatène
            auto vec2 = std::get<std::vector<std::vector<double>>>(sol_variant);
            for (const auto& v : vec2) {
                sol.insert(sol.end(), v.begin(), v.end());
            }
        }
    }
    return Agent(sol);
}

Agent Optimizer::generate_agent(const std::vector<double>& solution) {
    Agent agent = generate_empty_agent(solution);
    agent.target = get_target(agent.solution);
    return agent;
}

std::vector<Agent> Optimizer::generate_population(int pop_size_param) {
    int size = pop_size_param > 0 ? pop_size_param : pop_size;
    std::vector<Agent> population;
    if (mode == "thread" || mode == "process") {
        std::vector<std::future<Agent>> futures;
        for (int i = 0; i < size; i++) {
            futures.push_back(std::async(std::launch::async, [this]() {
                return generate_agent();
            }));
        }
        for (auto& fut : futures) {
            population.push_back(fut.get());
        }
    } else {
        for (int i = 0; i < size; i++) {
            population.push_back(generate_agent());
        }
    }
    return population;
}

std::vector<double> Optimizer::amend_solution(const std::vector<double>& solution) {
    return solution;
}

std::vector<double> Optimizer::correct_solution(const std::vector<double>& solution) {
    std::vector<double> amended = amend_solution(solution);
    return problem->correct_solution(amended);
}

std::vector<Agent> Optimizer::update_target_for_population(std::vector<Agent>& pop) {
    std::vector<std::vector<double>> pos_list;
    for (auto& agent : pop) {
        pos_list.push_back(agent.solution);
    }
    if (mode == "thread" || mode == "process") {
        std::vector<std::future<Target>> futures;
        for (const auto& pos : pos_list) {
            futures.push_back(std::async(std::launch::async, [this, pos]() {
                return get_target(pos, false);
            }));
        }
        for (size_t i = 0; i < futures.size(); i++) {
            pop[i].target = futures[i].get();
        }
    } else if (mode == "swarm") {
        for (size_t i = 0; i < pos_list.size(); i++) {
            pop[i].target = get_target(pos_list[i], false);
        }
    }
    nfe_counter += static_cast<int>(pop.size());
    return pop;
}

Target Optimizer::get_target(const std::vector<double>& solution, bool counted) {
    if (counted) {
        nfe_counter++;
    }
    return problem->get_target(solution);
}

bool Optimizer::compare_target(const Target& target_x, const Target& target_y, const std::string& minmax) {
    if (minmax == "min") {
        return (target_x.fitness() < target_y.fitness());
    } else {
        return (target_x.fitness() >= target_y.fitness());
    }
}

std::vector<Agent> Optimizer::duplicate_pop(const std::vector<Agent>& pop) {
    std::vector<Agent> dup;
    for (const auto& agent : pop) {
        dup.push_back(agent.copy());
    }
    return dup;
}

std::variant<std::vector<Agent>, std::pair<std::vector<Agent>, std::vector<int>>>
Optimizer::get_sorted_population(const std::vector<Agent>& pop, const std::string& minmax, bool return_index) {
    std::vector<Agent> sorted_pop = duplicate_pop(pop);
    // Création d'un vecteur d'indices
    std::vector<int> indices(sorted_pop.size());
    std::iota(indices.begin(), indices.end(), 0);
    // Tri en fonction du fitness
    std::sort(indices.begin(), indices.end(), [&sorted_pop](int i1, int i2) {
        return sorted_pop[i1].target.fitness() < sorted_pop[i2].target.fitness();
    });
    if (minmax == "max") {
        std::reverse(indices.begin(), indices.end());
    }
    std::vector<Agent> result;
    for (int idx : indices) {
        result.push_back(sorted_pop[idx]);
    }
    if (return_index) {
        return std::make_pair(result, indices);
    }
    return result;
}

std::tuple<std::vector<Agent>, std::vector<Agent>, std::vector<Agent>>
Optimizer::get_special_agents(const std::vector<Agent>& pop, int n_best, int n_worst, const std::string& minmax) {
    auto sorted_var = get_sorted_population(pop, minmax, false);
    std::vector<Agent> sorted_pop;
    if (std::holds_alternative<std::vector<Agent>>(sorted_var)) {
        sorted_pop = std::get<std::vector<Agent>>(sorted_var);
    }
    std::vector<Agent> best_agents, worst_agents;
    if (n_best > 0 && sorted_pop.size() >= static_cast<size_t>(n_best))
        best_agents.assign(sorted_pop.begin(), sorted_pop.begin() + n_best);
    if (n_worst > 0 && sorted_pop.size() >= static_cast<size_t>(n_worst))
        worst_agents.assign(sorted_pop.rbegin(), sorted_pop.rbegin() + n_worst);
    return std::make_tuple(sorted_pop, best_agents, worst_agents);
}

Agent Optimizer::get_better_agent(const Agent& agent_x, const Agent& agent_y, const std::string& minmax, bool reverse) {
    int idx = (minmax == "min") ? 0 : 1;
    if (reverse)
        idx = 1 - idx;
    if (idx == 0) {
        return (agent_x.target.fitness() < agent_y.target.fitness()) ? agent_x.copy() : agent_y.copy();
    } else {
        return (agent_x.target.fitness() < agent_y.target.fitness()) ? agent_y.copy() : agent_x.copy();
    }
}

std::vector<Agent> Optimizer::greedy_selection_population(const std::vector<Agent>& pop_old, const std::vector<Agent>& pop_new, const std::string& minmax) {
    if (pop_old.size() != pop_new.size()) {
        throw std::invalid_argument("Greedy selection of two population with different length.");
    }
    std::vector<Agent> new_pop;
    for (size_t i = 0; i < pop_old.size(); i++) {
        if (minmax == "min") {
            new_pop.push_back((pop_new[i].target.fitness() < pop_old[i].target.fitness()) ? pop_new[i] : pop_old[i]);
        } else {
            new_pop.push_back((pop_new[i].target.fitness() > pop_old[i].target.fitness()) ? pop_new[i] : pop_old[i]);
        }
    }
    return new_pop;
}

std::pair<std::vector<Agent>, Agent> Optimizer::update_global_best_agent(std::vector<Agent>& pop, bool save) {
    auto sorted_var = get_sorted_population(pop, problem->minmax, false);
    std::vector<Agent> sorted_pop;
    if (std::holds_alternative<std::vector<Agent>>(sorted_var)) {
        sorted_pop = std::get<std::vector<Agent>>(sorted_var);
    }
    Agent c_best = sorted_pop.front();
    Agent c_worst = sorted_pop.back();
    if (save) {
        list_current_best.push_back(c_best);
        Agent better = get_better_agent(c_best, list_global_best.empty() ? c_best : list_global_best.back(), problem->minmax);
        list_global_best.push_back(better);
        list_current_worst.push_back(c_worst);
        Agent worse = get_better_agent(c_worst, list_global_worst.empty() ? c_worst : list_global_worst.back(), problem->minmax, true);
        list_global_worst.push_back(worse);
        return {sorted_pop, better};
    } else {
        Agent local_better = get_better_agent(c_best, list_current_best.back(), problem->minmax);
        list_current_best.back() = local_better;
        Agent global_better = get_better_agent(c_best, list_global_best.back(), problem->minmax);
        list_global_best.back() = global_better;
        Agent local_worst = get_better_agent(c_worst, list_current_worst.back(), problem->minmax, true);
        list_current_worst.back() = local_worst;
        Agent global_worst = get_better_agent(c_worst, list_global_worst.back(), problem->minmax, true);
        list_global_worst.back() = global_worst;
        return {sorted_pop, global_better};
    }
}

int Optimizer::get_index_roulette_wheel_selection(const std::vector<double>& list_fitness) {
    if (list_fitness.empty()) return 0;
    // Calcul de l'étendue (ptp)
    double min_val = *std::min_element(list_fitness.begin(), list_fitness.end());
    double max_val = *std::max_element(list_fitness.begin(), list_fitness.end());
    double ptp = max_val - min_val;
    if (std::abs(ptp) < 1e-12) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(list_fitness.size()) - 1);
        return dist(generator);
    }
    // Si des valeurs négatives existent, décaler
    std::vector<double> adjusted = list_fitness;
    if (*std::min_element(adjusted.begin(), adjusted.end()) < 0) {
        for (auto& val : adjusted) {
            val -= min_val;
        }
    }
    std::vector<double> final_fitness = adjusted;
    if (problem->minmax == "min") {
        double max_adj = *std::max_element(adjusted.begin(), adjusted.end());
        for (auto& val : final_fitness) {
            val = max_adj - val;
        }
    }
    double sum = std::accumulate(final_fitness.begin(), final_fitness.end(), 0.0);
    if (std::abs(sum) < 1e-12) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(final_fitness.size()) - 1);
        return dist(generator);
    }
    // Distribution discrète pondérée
    std::discrete_distribution<int> dist(final_fitness.begin(), final_fitness.end());
    return dist(generator);
}

