//
// Created by chaid on 11/02/2025.
//
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>
#include <map>
#include <variant>
#include <tuple>
#include <future>
#include <stdexcept>
#include <random>
#include "utils/Agent/Agent.h"
#include "utils/Problem/Problem.h"
#include "utils/Target/Target.h"

class Optimizer {
public:
    // Pour information, cette liste n'est pas utilisée directement en C++
    static const std::vector<std::string> SUPPORTED_ARRAYS;

    // Constructeur prenant un dictionnaire de paramètres optionnels
    Optimizer(const std::map<std::string, std::string>& kwargs = std::map<std::string, std::string>());
    virtual ~Optimizer() = default;

    // Méthodes de gestion de l'historique des meilleures/pire solutions
    void store_initial_best_worst(const Agent& best, const Agent& worst);
    std::string get_name() const;

    // Méthodes d'initialisation (peuvent être surchargées)
    virtual void initialize_variables();
    virtual void before_initialization(const std::vector<std::vector<double>>& starting_solutions = {});
    virtual void initialization();
    virtual void after_initialization();
    virtual void before_main_loop();
    virtual void evolve(int epoch);

    // Vérification et affectation de l'objet Problem (peut être fourni soit comme instance, soit sous forme de dictionnaire)
    void check_problem(const std::variant<std::shared_ptr<Problem>, std::map<std::string, std::string>>& problem_var, int seed);

    // Méthode principale de résolution qui renvoie la meilleure solution (Agent)
    Agent solve(const std::variant<std::shared_ptr<Problem>, std::map<std::string, std::string>>& problem_var,
                const std::vector<std::vector<double>>& starting_solutions = {},
                int seed = 0);

    // Sauvegarde des historiques (après optimisation)
    void track_optimize_process();

    // Génération d'un agent vide ou complet
    Agent generate_empty_agent(const std::vector<double>& solution = std::vector<double>());
    Agent generate_agent(const std::vector<double>& solution = std::vector<double>());

    // Génération de la population (possibilité de parallélisation)
    virtual std::vector<Agent> generate_population(int pop_size = 0);

    // Correction d'une solution (la méthode amend_solution peut être surchargée)
    virtual std::vector<double> amend_solution(const std::vector<double>& solution);
    std::vector<double> correct_solution(const std::vector<double>& solution);

    // Mise à jour de la cible (Target) pour l'ensemble de la population (possibilité de parallélisation)
    virtual std::vector<Agent> update_target_for_population(std::vector<Agent>& pop);

    // Calcul de la target pour une solution donnée (le compteur d'évaluations est incrémenté si demandé)
    Target get_target(const std::vector<double>& solution, bool counted = true);

    // Comparaison de deux cibles en fonction du critère "min" ou "max"
    static bool compare_target(const Target& target_x,
                           const Target& target_y,
                           const std::string& minmax = "min");

    // Duplique la population en copiant chaque agent
    static std::vector<Agent> duplicate_pop(const std::vector<Agent>& pop);

    // Retourne la population triée (et éventuellement les indices de tri)
    static std::variant<std::vector<Agent>, std::pair<std::vector<Agent>, std::vector<int>>>
        get_sorted_population(const std::vector<Agent>& pop, const std::string& minmax = "min", bool return_index = false);

    // Renvoie un tuple contenant la population triée, les n_best et n_worst solutions (copies)
    static std::tuple<std::vector<Agent>, std::vector<Agent>, std::vector<Agent>>
        get_special_agents(const std::vector<Agent>& pop, int n_best = 3, int n_worst = 3, const std::string& minmax = "min");

    // Retourne l'agent le mieux adapté parmi deux (en fonction de minmax et d'un éventuel ordre inversé)
    static Agent get_better_agent(const Agent& agent_x, const Agent& agent_y, const std::string& minmax = "min", bool reverse = false);

    // Sélection gloutonne entre deux populations (élément par élément)
    static std::vector<Agent> greedy_selection_population(const std::vector<Agent>& pop_old, const std::vector<Agent>& pop_new, const std::string& minmax = "min");

    // Mise à jour de la meilleure solution globale et de l'historique associé
    std::pair<std::vector<Agent>, Agent> update_global_best_agent(std::vector<Agent>& pop, bool save = true);

    // Sélection par roulette (retourne l'indice sélectionné)
    int get_index_roulette_wheel_selection(const std::vector<double>& list_fitness);

    Agent& get_g_best() {
        return g_best;
    }


protected:
    // Attributs principaux (noms identiques à la version Python)
    int epoch;
    int pop_size;
    std::string mode;
    int n_workers;
    std::string name;
    std::vector<Agent> pop;
    Agent g_best;
    Agent g_worst;
    std::shared_ptr<Problem> problem;
    // logger non implémenté ici

    // Historique des solutions
    std::vector<Agent> list_global_best;
    std::vector<Agent> list_current_best;
    std::vector<double> list_global_best_fit;
    std::vector<double> list_current_best_fit;
    std::vector<Agent> list_global_worst;
    std::vector<Agent> list_current_worst;
    std::vector<double> list_epoch_time;

    int nfe_counter;

    // Générateur aléatoire
    std::mt19937 generator;

private:
    // Affecte dynamiquement les valeurs de kwargs aux attributs de l'objet
    void __set_keyword_arguments(const std::map<std::string, std::string>& kwargs);
};

#endif // OPTIMIZER_H

