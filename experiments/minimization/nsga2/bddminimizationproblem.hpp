#ifndef BDDMINIMIZATIONPROBLEM_H
#define BDDMINIMIZATIONPROBLEM_H

#include <string>
#include <vector>

#include <pagmo/population.hpp>
#include <pagmo/problem.hpp>
#include <pagmo/types.hpp>

#include <cudd/cplusplus/cuddObj.hh>

/**
 * @brief The BDDMinimizationProblem class
 * It creates and handles individuals that represent approximated functions for the use in a genetic
 * algorithm This class encodes the kind of approximations as follows: For each BDD in the function
 * and each variable that is present in the support of that function, the genetic algorithm is given
 * the choice to use an approximation operator operating on exactly that BDD and variable level. For
 * each of these possibilities, one integer value is stored in the individuum, with 0 representing
 * no approximation (the default state) and higher values representing the different approximations
 */
class BDDMinimizationProblem : public pagmo::problem
{
public:
    // pagmo needs these constructors
    BDDMinimizationProblem() = default;
    BDDMinimizationProblem(const BDDMinimizationProblem& o) = default;
    BDDMinimizationProblem(BDDMinimizationProblem&& o) = default;
    BDDMinimizationProblem(const Cudd& mgr, const std::vector<BDD>& original, float max_error_rate,
                           bool er_and_ace, float max_ace);

    pagmo::vector_double fitness(const pagmo::vector_double& parameters) const;

    pagmo::vector_double::size_type get_nobj() const
    {
        return er_and_ace ? 3 : 2;
    }

    std::pair<pagmo::vector_double, pagmo::vector_double> get_bounds() const;

    /**
     * @brief make_population Creates a population of individuums for genetic optimization
     * @param population_size The size of the population to create
     * @param initialization_count The number of approximations used in each individuum. They are
     * randomly distributed
     * @return The created population
     */
    pagmo::population make_population(std::size_t population_size,
                                      std::size_t initialization_count) const;

    // report the number of integer dimensions
    pagmo::vector_double::size_type get_nix() const
    {
        // all dimensions are integer
        return get_bounds().first.size();
    }

    std::string get_name() const
    {
        return "BDD Minimization";
    }

private:
    std::vector<BDD> function_from_individuum(const pagmo::vector_double& parameters) const;

    static std::vector<std::pair<std::size_t, std::size_t>>
    compute_support_positions(const std::vector<BDD>& function);

private:
    // these must be pointers due to the necessary default constructability
    const Cudd* mgr = nullptr;
    const std::vector<BDD>* original = nullptr;

    const float max_error_rate = 0.05f;
    const float max_average_case_error = 1;

    const std::size_t original_node_count = 0;
    const std::vector<std::pair<std::size_t, std::size_t>> support_positions;

    const bool er_and_ace = false;
};

PAGMO_S11N_PROBLEM_EXPORT_KEY(BDDMinimizationProblem)

#endif // BDDMINIMIZATIONPROBLEM_H
