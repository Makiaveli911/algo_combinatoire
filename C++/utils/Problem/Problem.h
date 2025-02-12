//
// Created by chaid on 11/02/2025.
//

#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <variant>
#include "../Space/BaseVar/BaseVar.h"
#include "../Space/FloatVar/FloatVar.h"
#include "../Target/Target.h"
#include <functional>

class Problem {
public:
    using objective_function_t = std::function<double(const std::vector<double>&)>;
    // Constructeurs
    // Un seul bound (instance de BaseVar)
    Problem(const std::shared_ptr<BaseVar>& bounds,
            const std::string &minmax = "min",
            objective_function_t objective_function = nullptr);

    virtual ~Problem() = default;

    // Getter pour _bounds
    std::vector<std::shared_ptr<BaseVar>> get_bounds() const;

    // Setters
    void set_bounds(const std::shared_ptr<BaseVar>& bounds);
    void set_bounds(const std::vector<std::shared_ptr<BaseVar>>& bounds);
    void set_seed(int seed);

    // Méthodes statiques pour le traitement de solution avec bounds
    static std::vector<double> encode_solution_with_bounds(const std::vector<std::vector<double>>& x,
                                                             const std::vector<std::shared_ptr<BaseVar>>& bounds);
    static std::map<std::string, std::vector<double>> decode_solution_with_bounds(const std::vector<double>& x,
                                                                                    const std::vector<std::shared_ptr<BaseVar>>& bounds);
    static std::vector<double> correct_solution_with_bounds(const std::vector<double>& x,
                                                              const std::vector<std::shared_ptr<BaseVar>>& bounds);
    // Retourne soit une solution aplatie (encoded) soit la solution segmentée
    static std::variant<std::vector<std::vector<double>>, std::vector<double>>
        generate_solution_with_bounds(const std::vector<std::shared_ptr<BaseVar>>& bounds,
                                      bool encoded = true);

    // Méthodes d'instance qui utilisent les bounds de l'objet
    std::vector<double> encode_solution(const std::vector<std::vector<double>>& x) const;
    std::map<std::string, std::vector<double>> decode_solution(const std::vector<double>& x) const;
    std::vector<double> correct_solution(const std::vector<double>& x) const;
    std::variant<std::vector<std::vector<double>>, std::vector<double>>
        generate_solution(bool encoded = true);

    // Retourne l'objet Target associé à une solution donnée
    Target get_target(const std::vector<double>& solution);

    // Variables membres (noms identiques à la version Python)
    std::vector<std::shared_ptr<BaseVar>> _bounds;
    std::vector<double> lb;
    std::vector<double> ub;
    std::string minmax;
    int seed;
    std::string name;
    std::string log_to;
    std::string log_file;
    int n_objs;
    std::vector<double> obj_weights;
    int n_dims;
    bool save_population;
    std::string msg;

private:
    objective_function_t obj_func;
    // Configure les fonctions en générant une solution test et en vérifiant obj_func
    void __set_functions();
};

#endif // PROBLEM_H

