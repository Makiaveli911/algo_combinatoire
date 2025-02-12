//
// Created by chaid on 11/02/2025.
//

#ifndef FLOATVAR_H
#define FLOATVAR_H

#include "../BaseVar/BaseVar.h"
#include <vector>
#include <stdexcept>

class FloatVar : public BaseVar {
public:
    // Constructeur avec bornes scalaires et nom par défaut "float"
    FloatVar(double lb = -10.0, double ub = 10.0, const std::string &name = "float");

    // Constructeur avec bornes vectorielles
    FloatVar(const std::vector<double> &lb, const std::vector<double> &ub, const std::string &name = "float");

    // Implémentation des méthodes abstraites
    virtual std::vector<double> encode(const std::vector<double> &x) const override;
    virtual std::vector<double> decode(const std::vector<double> &x) const override;
    virtual std::vector<double> correct(const std::vector<double> &x) const override;
    virtual std::vector<double> generate() override;

private:
    // Méthode privée pour définir les bornes (surchargée)
    void _set_bounds(double lb, double ub);
    void _set_bounds(const std::vector<double> &lb, const std::vector<double> &ub);
};

#endif // FLOATVAR_H

