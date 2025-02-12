//
// Created by chaid on 11/02/2025.
//

#include "BaseVar.h"
#include <random>

// Initialisation de SUPPORTED_ARRAY (pour information, il ne sera pas utilisé en C++)
const std::vector<std::string> BaseVar::SUPPORTED_ARRAY = {"tuple", "list", "np.ndarray"};

BaseVar::BaseVar(const std::string &name)
    : name(name), n_vars(0), lb(), ub(), _seed(0)
{
    // Initialisation du générateur avec une graine aléatoire issue du std::random_device
    std::random_device rd;
    generator = std::mt19937(rd());
}

int BaseVar::seed() const {
    return _seed;
}

void BaseVar::set_seed(int value) {
    _seed = value;
    generator = std::mt19937(static_cast<unsigned int>(_seed));
}

