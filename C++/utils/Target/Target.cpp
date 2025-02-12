//
// Created by chaid on 10/02/2025.
//
#include "Target.h"
#include <stdexcept>

// Fonction auxiliaire pour calculer le produit scalaire de deux vecteurs
static double dot_product(const std::vector<double>& a, const std::vector<double>& b) {
    if(a.size() != b.size()){
        throw std::invalid_argument("Les vecteurs doivent avoir la même taille pour le produit scalaire.");
    }
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// Constructeur principal
Target::Target(const std::vector<double>& objectives, const std::vector<double>& weights) : _objectives(), _weights(), _fitness(0.0) {
    if(objectives.empty()){
        throw std::invalid_argument("Invalid objectives. It should be a list, tuple, np.ndarray, int or float.");
    }
    set_objectives(objectives);
    set_weights(weights);
    calculate_fitness(this->_weights);
}

// Surcharge du constructeur pour un unique objectif
Target::Target(double objective, double weight) : _objectives(), _weights(), _fitness(0.0) {
    // Convertir le nombre en vecteur
    set_objectives(objective);
    // S'il n'y a pas de poids fourni (weight == 0.0), on définit le poids par défaut à 1.0 pour chaque objectif
    if(weight == 0.0) {
        set_weights(std::vector<double>(_objectives.size(), 1.0));
    } else {
        set_weights(weight);
    }
    calculate_fitness(this->_weights);
}

// Méthode copy qui retourne une copie de l'objet
Target Target::copy() const {
    return Target(_objectives, _weights);
}

// Getter pour objectives
std::vector<double> Target::objectives() const {
    return _objectives;
}

// Setter pour objectives avec un vecteur
void Target::set_objectives(const std::vector<double>& objs) {
    if(objs.empty()){
        throw std::invalid_argument("Invalid objectives. It should be a list, tuple, np.ndarray, int or float.");
    }
    // On suppose ici que le vecteur est déjà "aplati"
    _objectives = objs;
}

// Surcharge du setter pour objectives avec un unique nombre
void Target::set_objectives(double obj) {
    _objectives = { obj };
}

// Getter pour weights
std::vector<double> Target::weights() const {
    return _weights;
}

// Setter pour weights avec un vecteur
void Target::set_weights(const std::vector<double>& weights) {
    if(weights.empty()){
        // Si aucun poids n'est fourni, on affecte par défaut 1.0 à chaque objectif
        if(_objectives.empty()){
            throw std::runtime_error("objectives must be set before setting weights.");
        }
        _weights = std::vector<double>(_objectives.size(), 1.0);
    } else {
        // Si la taille du vecteur weights ne correspond pas à celle des objectives, on utilise par défaut des poids de 1.0
        if(weights.size() != _objectives.size()){
            _weights = std::vector<double>(_objectives.size(), 1.0);
        } else {
            _weights = weights;
        }
    }
}

// Surcharge du setter pour weights avec un unique nombre
void Target::set_weights(double weight) {
    if(_objectives.empty()){
        throw std::runtime_error("objectives must be set before setting weights.");
    }
    // Répéter le poids pour chaque objectif
    _weights = std::vector<double>(_objectives.size(), weight);
}

// Getter pour fitness
double Target::fitness() const {
    return _fitness;
}

// Méthode pour calculer le fitness
void Target::calculate_fitness(const std::vector<double>& weights) {
    std::vector<double> local_weights = weights;
    if(local_weights.size() != _objectives.size()){
        local_weights = std::vector<double>(_objectives.size(), 1.0);
    }
    _fitness = dot_product(local_weights, _objectives);
}
