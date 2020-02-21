#include "nsga2_minimization_helper.hpp"
#include "bddminimizationproblem.hpp"

#include <chrono>
#include <pagmo/algorithms/nsga2.hpp>

namespace abo::nsga2minimization {

    MinimizationResult run_genetic_minimization(const Cudd &mgr,
                                                const std::vector<BDD> &function,
                                                double max_error_rate,
                                                double max_average_case_error,
                                                bool use_both_metrics) {

        const std::size_t POPULATION_SIZE = 100; // nsga2 requires this to be a multiple of 4
        // the number of generations to run in one step, if no further improvement was found, the algorithm terminates
        const std::size_t NUM_GENERATIONS = 20;

        // parameters for nsga2
        const float CROSSOVER_PROBABILITY = 0.5;
        const float CROSSOVER_ETA = 20;
        const float MUTATION_COUNT = 0.3;
        const float MUTATION_ETA = 20;

        const float INITIALIZATION_COUNT = 3;

        // this class evaluates the fitness of different indivuums
        BDDMinimizationProblem problem(mgr, function, max_error_rate, use_both_metrics, max_average_case_error);

        //  convert the mutation count to a mutation probability
        float mp = MUTATION_COUNT / problem.get_nix();
        pagmo::algorithm algorithm(pagmo::nsga2(NUM_GENERATIONS, CROSSOVER_PROBABILITY, CROSSOVER_ETA, mp, MUTATION_ETA));

        srand(0);
        algorithm.set_seed(0);

        pagmo::population pop = problem.make_population(POPULATION_SIZE, INITIALIZATION_COUNT);

        // store information about the best indiviuum found
        MinimizationResult best_result;
        best_result.node_count = std::numeric_limits<std::size_t>::max();
        best_result.error_rate = 0;
        best_result.average_case_error = 0;

        auto before = std::chrono::high_resolution_clock::now();

        while (true) {
            std::size_t previous_smallest = best_result.node_count;
            pop = algorithm.evolve(pop);

            // find the best individuum
            for (auto ind : pop.get_x()) {
                auto fit = problem.fitness(ind);
                // if a function with a smaller node count within the error metric constraints is found, save it
                if (fit[1] < max_error_rate && (!use_both_metrics || fit[2] < max_average_case_error) && fit[0] < best_result.node_count) {
                    best_result.node_count = fit[0];
                    best_result.error_rate = fit[1];
                    if (use_both_metrics) {
                        best_result.average_case_error = fit[2];
                    }
                }
            }

            // stop when nothing better was found for some time
            if (best_result.node_count == previous_smallest) {
                break;
            }
        }

        auto after = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> minimization_time = std::chrono::duration_cast<std::chrono::duration<double>>(after - before);

        best_result.computation_time = minimization_time.count() * 1000.0f;
        return best_result;
    }

}
