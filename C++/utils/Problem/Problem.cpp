//
// Created by chaid on 11/02/2025.
//

#include "Problem.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>


// ---------------------- Constructeurs ----------------------

// Constructeur avec un seul bound
Problem::Problem(const std::shared_ptr<BaseVar>& bound,
                 const std::string &minmax,
                 objective_function_t objective_function)
    : minmax(minmax), seed(-1), name("P"), log_to("console"), log_file("history.txt"),
      n_objs(1), n_dims(0), save_population(false)
{
    set_bounds(bound);
    __set_functions();
}

// ---------------------- Getters et Setters ----------------------

std::vector<std::shared_ptr<BaseVar>> Problem::get_bounds() const {
    return _bounds;
}

void Problem::set_bounds(const std::shared_ptr<BaseVar>& bound) {
    if (!bound) {
        throw std::invalid_argument("Invalid bounds. It should be an instance of BaseVar.");
    }
    bound->set_seed(seed);
    _bounds.clear();
    _bounds.push_back(bound);
    lb.clear();
    ub.clear();
    for (auto& b : _bounds) {
        lb.insert(lb.end(), b->lb.begin(), b->lb.end());
        ub.insert(ub.end(), b->ub.begin(), b->ub.end());
    }
}

void Problem::set_seed(int seed) {
    this->seed = seed;
    for (auto& b : _bounds) {
        b->set_seed(seed);
    }
}

void Problem::__set_functions() {
    // Génération d'une solution test (encodée)
    auto tested_solution_variant = generate_solution(true);
    this->n_dims = tested_solution_variant.size;

   }

// ---------------------- Méthodes statiques ----------------------

std::vector<double> Problem::encode_solution_with_bounds(const std::vector<std::vector<double>>& x,
                                                           const std::vector<std::shared_ptr<BaseVar>>& bounds) {
    std::vector<double> x_new;
    for (size_t i = 0; i < bounds.size(); ++i) {
        // On suppose que chaque appel à encode renvoie un std::vector<double>
        std::vector<double> encoded = bounds[i]->encode(x[i]);
        x_new.insert(x_new.end(), encoded.begin(), encoded.end());
    }
    return x_new;
}

std::map<std::string, std::vector<double>> Problem::decode_solution_with_bounds(const std::vector<double>& x,
                                                                                  const std::vector<std::shared_ptr<BaseVar>>& bounds) {
    std::map<std::string, std::vector<double>> x_new;
    size_t n_vars = 0;
    for (size_t i = 0; i < bounds.size(); ++i) {
        const auto& var = bounds[i];
        if(n_vars + var->n_vars > x.size()){
            throw std::invalid_argument("Input vector size does not match the total number of variables.");
        }
        std::vector<double> sub(x.begin() + n_vars, x.begin() + n_vars + var->n_vars);
        std::vector<double> temp = var->decode(sub);
        // Dans le cas d'une seule variable, on stocke quand même dans un vecteur
        x_new[var->name] = temp;
        n_vars += var->n_vars;
    }
    return x_new;
}

std::vector<double> Problem::correct_solution_with_bounds(const std::vector<double>& x,
                                                            const std::vector<std::shared_ptr<BaseVar>>& bounds) {
    std::vector<double> x_new;
    size_t n_vars = 0;
    for (size_t i = 0; i < bounds.size(); ++i) {
        const auto& var = bounds[i];
        if(n_vars + var->n_vars > x.size()){
            throw std::invalid_argument("Input vector size does not match the total number of variables.");
        }
        std::vector<double> sub(x.begin() + n_vars, x.begin() + n_vars + var->n_vars);
        std::vector<double> corrected = var->correct(sub);
        x_new.insert(x_new.end(), corrected.begin(), corrected.end());
        n_vars += var->n_vars;
    }
    return x_new;
}

std::variant<std::vector<std::vector<double>>, std::vector<double>>
Problem::generate_solution_with_bounds(const std::vector<std::shared_ptr<BaseVar>>& bounds,
                                         bool encoded) {
    std::vector<std::vector<double>> x;
    for (auto& var : bounds) {
        x.push_back(var->generate());
    }
    if (encoded) {
        std::vector<double> encoded_solution = encode_solution_with_bounds(x, bounds);
        return encoded_solution;
    }
    return x;
}

// ---------------------- Méthodes d'instance ----------------------

std::vector<double> Problem::encode_solution(const std::vector<std::vector<double>>& x) const {
    return encode_solution_with_bounds(x, _bounds);
}

std::map<std::string, std::vector<double>> Problem::decode_solution(const std::vector<double>& x) const {
    return decode_solution_with_bounds(x, _bounds);
}

std::vector<double> Problem::correct_solution(const std::vector<double>& x) const {
    return correct_solution_with_bounds(x, _bounds);
}

std::variant<std::vector<std::vector<double>>, std::vector<double>>

Problem::generate_solution(bool encoded) {
    return generate_solution_with_bounds(_bounds, encoded);
}

Target Problem::get_target(const std::vector<double>& solution) {
    std::vector<double> objs = obj_func(solution);
    return Target(objs, obj_weights);
}

