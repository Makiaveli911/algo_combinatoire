//
// Created by chaid on 11/02/2025.
//

#ifndef ORIGINALABC_H
#define ORIGINALABC_H

#include "optimizer.h"
#include <vector>
#include <map>
#include <string>

/**
 * The original version of: Artificial Bee Colony (ABC)
 *
 * Hyper-parameters:
 *   - n_limits: Limit of trials before abandoning a food source, default = 25
 *
 * Références:
 * [1] B. Basturk, D. Karaboga, An artificial bee colony (ABC) algorithm for numeric function optimization,
 *     in: IEEE Swarm Intelligence Symposium 2006.
 */
class OriginalABC : public Optimizer {
public:
    // Constructeur
    // epoch: nombre maximum d'itérations (default = 10000)
    // pop_size: taille de la population (default = 100)
    // n_limits: nombre limite d'essais avant abandon (default = 25)
    // kwargs: paramètres supplémentaires
    OriginalABC(int epoch = 10000, int pop_size = 100, int n_limits = 25,
                const std::map<std::string, std::string>& kwargs = std::map<std::string, std::string>());

    // Méthode d'initialisation des variables (par exemple, initialisation du vecteur trials)
    void initialize_variables();

    // La méthode evolve représente l'évolution d'une itération (phase employed, onlooker, scout)
    void evolve(int epoch);

    // Variables membres (noms inchangés par rapport au code Python)
    // Agent* g_best();
    int epoch;
    int pop_size;
    int n_limits;
    std::vector<int> trials;
};

#endif // ORIGINALABC_H

