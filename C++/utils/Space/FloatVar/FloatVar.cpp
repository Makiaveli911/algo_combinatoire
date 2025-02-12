//
// Created by chaid on 11/02/2025.
//

#include "FloatVar.h"
#include <algorithm> // pour std::clamp (C++17)
#include <random>

FloatVar::FloatVar(double lb, double ub, const std::string &name)
    : BaseVar(name)
{
    _set_bounds(lb, ub);
}

FloatVar::FloatVar(const std::vector<double> &lb_vec, const std::vector<double> &ub_vec, const std::string &name)
    : BaseVar(name)
{
    _set_bounds(lb_vec, ub_vec);
}

void FloatVar::_set_bounds(double lb_val, double ub_val) {
    // Si lb et ub sont des nombres, on les stocke comme vecteur d'un seul élément
    this->lb = { lb_val };
    this->ub = { ub_val };
    n_vars = 1;
}

void FloatVar::_set_bounds(const std::vector<double> &lb_vec, const std::vector<double> &ub_vec) {
    if (lb_vec.size() != ub_vec.size()) {
        throw std::invalid_argument("Invalid lb or ub. Length of lb should equal to length of ub.");
    }
    this->lb = lb_vec;
    this->ub = ub_vec;
    n_vars = static_cast<int>(lb_vec.size());
}

std::vector<double> FloatVar::encode(const std::vector<double> &x) const {
    // Retourne x converti en vecteur de double (déjà le cas)
    return x;
}

std::vector<double> FloatVar::decode(const std::vector<double> &x) const {
    // Applique correct sur x puis retourne le résultat
    return correct(x);
}

std::vector<double> FloatVar::correct(const std::vector<double> &x) const {
    if (x.size() != lb.size()) {
        throw std::invalid_argument("Input vector size does not match bounds size.");
    }
    std::vector<double> result(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        // Utilise std::clamp pour contraindre x[i] entre lb[i] et ub[i]
        result[i] = std::clamp(x[i], lb[i], ub[i]);
    }
    return result;
}

std::vector<double> FloatVar::generate() {
    std::vector<double> result(n_vars);
    for (int i = 0; i < n_vars; ++i) {
        std::uniform_real_distribution<double> dist(lb[i], ub[i]);
        result[i] = dist(generator);
    }
    return result;
}

