#ifndef TARGET_H
#define TARGET_H

#include <vector>
#include <stdexcept>

class Target {
public:
    // Constructeur principal : prend en paramètre un vecteur d'objectives et éventuellement un vecteur de weights.
    Target(const std::vector<double>& objectives,
           const std::vector<double>& weights = std::vector<double>());

    // Surcharge du constructeur : permet de passer un unique nombre pour objectives.
    // Le paramètre weight est optionnel. S'il vaut 0.0, on considérera qu'aucun poids n'est fourni (et on utilisera 1.0 pour chaque objectif).
    Target(double objective, double weight = 0.0);

    // Méthode copy qui retourne une copie de l'objet
    Target copy() const;

    // Getter pour objectives
    std::vector<double> objectives() const;

    // Setter pour objectives (surcharge)
    void set_objectives(const std::vector<double>& objs);
    void set_objectives(double obj);

    // Getter pour weights
    std::vector<double> weights() const;

    // Setter pour weights (surcharge)
    void set_weights(const std::vector<double>& weights);
    void set_weights(double weight);

    // Getter pour fitness
    double fitness() const;

    // Méthode pour calculer le fitness
    void calculate_fitness(const std::vector<double>& weights);

private:
    // Variables membres conservant exactement le même nom que dans votre code Python
    std::vector<double> _objectives;
    std::vector<double> _weights;
    double _fitness;
};

#endif // TARGET_H