//
// Created by chaid on 11/02/2025.
//

#include "originalAbc.h"
#include <random>
#include <algorithm>
#include <stdexcept>

// Constructeur
OriginalABC::OriginalABC(int epoch, int pop_size, int n_limits,
                         const std::map<std::string, std::string>& kwargs)
    : Optimizer(kwargs), epoch(epoch), pop_size(pop_size), n_limits(n_limits)
{
    // La configuration de certains paramètres supplémentaires peut être réalisée dans Optimizer
}

// Méthode d'initialisation des variables
void OriginalABC::initialize_variables() {
    // Initialise le vecteur trials avec des zéros (taille = pop_size)
    trials.assign(pop_size, 0);
}

// Méthode évolutive de l'algorithme
void OriginalABC::evolve(int epoch) {
    // Lambda pour choisir un indice aléatoire en excluant certains indices
    auto get_random_index_excluding = [this](const std::vector<int>& exclusions) -> int {
        int index;
        std::uniform_int_distribution<int> dist(0, this->pop_size - 1);
        while (true) {
            index = dist(this->generator);
            if (std::find(exclusions.begin(), exclusions.end(), index) == exclusions.end()) {
                break;
            }
        }
        return index;
    };

    // --- Phase des "employed bees" ---
    for (int idx = 0; idx < pop_size; idx++) {
        // Choisir un employé aléatoire différent de idx
        int rdx = get_random_index_excluding({ idx });
        int n_dims = this->problem->n_dims;  // Nombre de dimensions
        std::vector<double> phi(n_dims);
        std::uniform_real_distribution<double> dist_phi(-1.0, 1.0);
        for (int j = 0; j < n_dims; j++) {
            phi[j] = dist_phi(this->generator);
        }
        // Calculer la nouvelle position :
        // pos_new = pop[idx].solution + phi * (pop[rdx].solution - pop[idx].solution)
        std::vector<double> pos_new(n_dims);
        for (int j = 0; j < n_dims; j++) {
            double diff = this->pop[rdx].solution[j] - this->pop[idx].solution[j];
            pos_new[j] = this->pop[idx].solution[j] + phi[j] * diff;
        }
        // Correction de la solution
        pos_new = this->correct_solution(pos_new);
        // Génération d'un nouvel agent avec la solution pos_new
        Agent agent = this->generate_agent(pos_new);
        // Si la nouvelle cible est meilleure (selon minmax) on remplace sinon on incrémente le compteur d'essais
        if (this->compare_target(agent.target, this->pop[idx].target, this->problem->minmax)) {
            this->pop[idx] = agent;
            trials[idx] = 0;
        } else {
            trials[idx] += 1;
        }
    }

    // --- Phase des "onlooker bees" ---
    // Calcul des fitness de chaque agent employé
    std::vector<double> employed_fits(pop_size);
    for (int i = 0; i < pop_size; i++) {
        employed_fits[i] = this->pop[i].target.fitness();
    }
    for (int idx = 0; idx < pop_size; idx++) {
        // Sélection par roulette
        int selected_bee = this->get_index_roulette_wheel_selection(employed_fits);
        // Choisir un employé aléatoire différent de idx et selected_bee
        int rdx = get_random_index_excluding({ idx, selected_bee });
        int n_dims = this->problem->n_dims;
        std::vector<double> phi(n_dims);
        std::uniform_real_distribution<double> dist_phi(-1.0, 1.0);
        for (int j = 0; j < n_dims; j++) {
            phi[j] = dist_phi(this->generator);
        }
        std::vector<double> pos_new(n_dims);
        for (int j = 0; j < n_dims; j++) {
            double diff = this->pop[rdx].solution[j] - this->pop[selected_bee].solution[j];
            pos_new[j] = this->pop[selected_bee].solution[j] + phi[j] * diff;
        }
        pos_new = this->correct_solution(pos_new);
        Agent agent = this->generate_agent(pos_new);
        if (this->compare_target(agent.target, this->pop[selected_bee].target, this->problem->minmax)) {
            this->pop[selected_bee] = agent;
            trials[selected_bee] = 0;
        } else {
            trials[selected_bee] += 1;
        }
    }

    // --- Phase des "scout bees" ---
    // Identifier les indices où le nombre d'essais dépasse n_limits
    std::vector<int> abandoned;
    for (int i = 0; i < pop_size; i++) {
        if (trials[i] >= n_limits) {
            abandoned.push_back(i);
        }
    }
    // Pour chaque source abandonnée, générer un nouvel agent et réinitialiser son compteur d'essais
    for (int idx : abandoned) {
        this->pop[idx] = this->generate_agent();
        trials[idx] = 0;
    }
}

