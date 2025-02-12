#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include "utils/Space/FloatVar/FloatVar.h"      // Votre classe FloatVar dans utils/Space
#include "OriginalABC.h"                         // Votre classe OriginalABC dans ABC
#include "utils/Problem/Problem.h"               // Votre classe Problem traduite
#include "utils/Agent/Agent.h"                   // Votre classe Agent traduite
#include "utils/Target/Target.h"                 // Votre classe Target traduite

// --- La fonction objective_function telle que dans le script Python ---
double objective_function(const std::vector<double>& solution) {
    double sum = 0.0;
    for (double x : solution) {
        sum += x * x;
    }
    return sum;
}

// --- Structure pour reproduire le problem_dict Python ---
struct ProblemDict {
    std::shared_ptr<FloatVar> bounds;
    std::string minmax;
    std::function<double(const std::vector<double>&)> obj_func;
};

// --- Classe MyProblem ---
// Cette classe hérite de Problem et redéfinit la méthode obj_func
class MyProblem : public Problem {
public:
    std::function<double(const std::vector<double>&)> obj_func_ptr;

    MyProblem(const std::shared_ptr<BaseVar>& bounds, const std::string& minmax,
              std::function<double(const std::vector<double>&)> obj_func)
        : Problem(bounds, minmax), obj_func_ptr(obj_func) {}

    // La méthode obj_func retourne un std::vector<double> (même si en Python c'est un nombre)
    virtual std::vector<double> obj_func(const std::vector<double>& x) override {
        double result = obj_func_ptr(x);
        return std::vector<double>{result};
    }
};
int main() {
    // --- Création du problem_dict exactement comme dans le script Python ---
    ProblemDict problem_dict;
    // Reproduire "FloatVar(lb=(-10.,)*30, ub=(10.,)*30, name='delta')"
    std::vector<double> lb(30, -10.0);
    std::vector<double> ub(30, 10.0);
    problem_dict.bounds = std::make_shared<FloatVar>(lb, ub, "delta");
    problem_dict.minmax = "min";
    problem_dict.obj_func = objective_function;

    // --- Instanciation du modèle OriginalABC avec epoch=1000, pop_size=50, n_limits=50 ---
    OriginalABC model(1000, 50, 50);

    // --- Conversion du problem_dict en un objet Problem via MyProblem ---
    std::shared_ptr<Problem> problem = std::make_shared<MyProblem>(problem_dict.bounds, problem_dict.minmax, problem_dict.obj_func);

    // --- Appel de solve() en passant le problème (seed fixé à 1234, starting_solutions vide) ---
    Agent g_best = model.solve(problem, std::vector<std::vector<double>>{}, 1234);

    // --- Affichage de la solution et de la fitness (g_best correspond à la variable Python g_best) ---
    std::cout << "Solution: ";
    for (double v : g_best.solution) {
        std::cout << v << " ";
    }
    std::cout << ", Fitness: " << g_best.target.fitness() << std::endl;

    // --- Affichage de la meilleure solution globale stockée dans l'algorithme ---
    // On utilise le getter public get_g_best() pour accéder à g_best.
    const Agent& best = model.get_g_best();
    std::cout << "Global Best Solution: ";
    for (double v : best.solution) {
        std::cout << v << " ";
    }
    std::cout << ", Fitness: " << best.target.fitness() << std::endl;

    return 0;
}
