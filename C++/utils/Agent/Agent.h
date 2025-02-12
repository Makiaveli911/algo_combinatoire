//
// Created by chaid on 11/02/2025.
//

#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "../Target/Target.h"

class Agent {
public:
    // Identifiant unique auto-incrémenté pour chaque agent
    static int ID;

    // Constructeur
    // - solution : représente la position de l'agent (std::vector<double>)
    // - target : stocke la fitness de la solution (instance de Target)
    // - kwargs : paramètres supplémentaires
    Agent(const std::vector<double>& solution = std::vector<double>(),
          const Target& target = Target(0.0),
          const std::map<std::string, std::string>& kwargs = std::map<std::string, std::string>());

    // Méthode de classe pour incrémenter l'identifiant
    static int increase();

    // Affecte les valeurs contenues dans kwargs aux attributs de l'agent
    void set_kwargs(const std::map<std::string, std::string>& kwargs);

    // Retourne une copie de l'agent
    Agent copy() const;

    // Attributs (noms inchangés par rapport à la version Python)
    std::vector<double> solution;
    Target target;
    int id;
    std::map<std::string, std::string> kwargs;
};

#endif // AGENT_H

