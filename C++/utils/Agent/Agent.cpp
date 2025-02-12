//
// Created by chaid on 11/02/2025.
//

#include "Agent.h"

// Initialisation de l'identifiant statique
int Agent::ID = 0;

Agent::Agent(const std::vector<double>& solution,
             const Target& target,
             const std::map<std::string, std::string>& kwargs)
    : solution(solution), target(target), kwargs(kwargs)
{
    set_kwargs(kwargs);
    id = increase();
}

int Agent::increase() {
    ID += 1;
    return ID;
}

void Agent::set_kwargs(const std::map<std::string, std::string>& kwargs) {
    // En C++, il n'est pas possible d'ajouter dynamiquement des attributs à l'objet.
    // Nous stockons simplement le dictionnaire kwargs dans l'attribut correspondant.
    this->kwargs = kwargs;
}

Agent Agent::copy() const {
    // Crée une nouvelle instance d'Agent en copiant solution, target et kwargs.
    // Puisque target est stocké par valeur, le constructeur de copie de Target sera utilisé.
    return Agent(this->solution, this->target, this->kwargs);
}

