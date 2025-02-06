#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Inclusion des autres dépendances (à adapter selon votre projet)
#include "Agent.h"
#include "History.h"
#include "Logger.h"
#include "Problem.h"
#include "Target.h"
#include "Termination.h"
#include "Validator.h"

/**
 * @brief Classe de base de tous les algorithmes.
 *
 * Notes :
 * - La fonction solve() est la méthode principale qui entraîne le modèle.
 * - Le parallélisme (multithreading ou multiprocessing) est utilisé dans les méthodes :
 *   generate_population() et update_target_for_population().
 * - Format général :
 *      - population = [agent_1, agent_2, ..., agent_N]
 *      - agent = [solution, target]
 *      - target = [fitness value, objective_list]
 *      - objective_list = [obj_1, obj_2, ..., obj_M]
 */
class Optimizer {
public:
    // Constantes (les mêmes noms que dans le code Python)
    static constexpr double EPSILON = 10E-10;
    static const std::vector<std::string> SUPPORTED_MODES;
    static const std::vector<std::string> AVAILABLE_MODES;
    static const std::vector<std::string> PARALLEL_MODES;
    // Pour SUPPORTED_ARRAYS, nous indiquons simplement les noms des types utilisés en Python
    static const std::vector<std::string> SUPPORTED_ARRAYS;

    // Attributs
    int epoch;                // Nombre d'époques
    int pop_size;             // Taille de la population
    std::string mode;         // Mode d'exécution
    int n_workers;            // Nombre de workers (threads/processus)
    std::string name;         // Nom de l'optimiseur

    std::vector<Agent> pop;   // Population (liste d'agents)
    Agent g_best;             // Meilleur agent global
    Agent* g_worst;           // Agent global le moins bon (initialisé à nullptr)

    Problem* problem;         // Problème associé
    Logger* logger;           // Logger associé
    History* history;         // Historique

    Validator validator;      // Instance de Validator

    bool sort_flag;           // Indique si le tri est activé
    int nfe_counter;          // Compteur d'évaluations (initialisé à -1)
    std::map<std::string, double> parameters;       // Dictionnaire des paramètres
    std::vector<std::string> params_name_ordered;     // Ordre des noms de paramètres
    bool is_parallelizable;   // Indique si l'algorithme peut être parallélisé

    /**
     * @brief Constructeur (équivalent de __init__ en Python)
     *
     * @param kwargs Un std::map contenant les arguments nommés.
     */
    Optimizer(const std::map<std::string, std::string>& kwargs);

    virtual ~Optimizer();

protected:
    /**
     * @brief Méthode privée pour définir les arguments clés.
     *
     * Simule la méthode __set_keyword_arguments de Python.
     *
     * @param kwargs Le std::map contenant les paramètres.
     */
    void __set_keyword_arguments(const std::map<std::string, std::string>& kwargs);
};

#endif // OPTIMIZER_H
