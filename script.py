import numpy as np
import os
import time
import multiprocessing
from ABC import OriginalABC
from utils.space import FloatVar

def rosenbrock_function(solution):
  return np.sum((1 - solution[:-1])**2.0 + 100.0 * (solution[1:] - solution[:-1]**2.0)**2.0)

def rastrigin_function(solution):
  A = 10
  return A * len(solution) + np.sum(solution**2 - A * np.cos(2 * np.pi * solution))

def ackley_function(solution):
  a = 20
  b = 0.2
  c = 2 * np.pi
  d = len(solution)
  sum1 = np.sum(solution**2)
  sum2 = np.sum(np.cos(c * solution))
  term1 = -a * np.exp(-b * np.sqrt(sum1 / d))
  term2 = -np.exp(sum2 / d)
  return term1 + term2 + a + np.exp(1)

# Récupérer le nombre de cœurs du CPU
n_cores = multiprocessing.cpu_count()

functions = [rosenbrock_function, rastrigin_function, ackley_function]
dimensions = [30, 50, 100]
num_executions = 10
pop_size = 30
epochs = 5000

# Clear results directory
for filename in os.listdir('results'):
  if filename.endswith('.txt'):
    file_path = os.path.join('results', filename)
    if os.path.isfile(file_path):
      os.unlink(file_path)

start_time = time.time()

for func in functions:
  for dim in dimensions:
    results = []
    with open(f'results/results_{func.__name__}_{dim}.txt', 'w') as f:
      for _ in range(num_executions):
        problem_dict = {
          "bounds": FloatVar(lb=(-10.,) * dim, ub=(10.,) * dim, name="delta"),
          "obj_func": func,
          "minmax": "min",
        }
        model = OriginalABC(epoch=epochs, pop_size=pop_size, loudness=0.8, pulse_rate=0.95, pf_min=0.1, pf_max=10.0, n_workers=n_cores)
        g_best = model.solve(problem_dict)

        results.append(g_best.target.fitness)
        f.write(f"Fonction: {func.__name__}, Dimension: {dim}, Exécution: {_+1}, Fitness: {g_best.target.fitness}\n")
      
      avg_fitness = np.mean(results)
      std_fitness = np.std(results)
      f.write(f"Fonction: {func.__name__}, Dimension: {dim}, Fitness Moyenne sur {num_executions} exécutions: {avg_fitness}\n")
      f.write(f"Fonction: {func.__name__}, Dimension: {dim}, Écart Type sur {num_executions} exécutions: {std_fitness}\n")

print(f"Temps total d'exécution: {time.time() - start_time} secondes")