#include "bddminimizationproblem.hpp"
#include "average_case_error.hpp"
#include "cudd_helpers.hpp"
#include "error_rate.hpp"

BDDMinimizationProblem::BDDMinimizationProblem(const Cudd& mgr, const std::vector<BDD>& original,
                                               float max_error_rate, bool er_and_ace, float max_ace)
    : mgr(&mgr)
    , original(&original)
    , max_error_rate(max_error_rate)
    , max_average_case_error(max_ace)
    , original_node_count(mgr.nodeCount(original))
    , support_positions(compute_support_positions(original))
    , er_and_ace(er_and_ace)
{
}

std::vector<std::pair<std::size_t, std::size_t>>
BDDMinimizationProblem::compute_support_positions(const std::vector<BDD>& function)
{
    std::vector<std::pair<std::size_t, std::size_t>> result;
    for (std::size_t i = 0; i < function.size(); i++)
    {
        auto support = function.at(i).SupportIndices();
        for (auto s : support)
        {
            result.push_back({i, s});
        }
    }
    return result;
}

std::pair<pagmo::vector_double, pagmo::vector_double> BDDMinimizationProblem::get_bounds() const
{
    return {pagmo::vector_double(support_positions.size(), 0),
            pagmo::vector_double(support_positions.size(), 2)};
}

pagmo::population BDDMinimizationProblem::make_population(std::size_t population_size,
                                                          std::size_t initialization_count) const
{
    pagmo::population pop(*this, 0);
    for (std::size_t i = 0; i < population_size; i++)
    {
        std::vector<double> data(get_bounds().first.size(), 0);
        for (std::size_t b = 0; b < initialization_count; b++)
        {
            data[rand() % data.size()] = 1 + rand() % 2;
        }
        pop.push_back(data);
    }
    return pop;
}

std::vector<BDD>
BDDMinimizationProblem::function_from_individuum(const pagmo::vector_double& parameters) const
{
    std::vector<BDD> individuum = *original;

    for (std::size_t i = 0; i < support_positions.size(); i++)
    {
        int bit = support_positions[i].first;
        int level = support_positions[i].second;
        if (parameters[i] > 1.5f)
        {
            individuum[bit] = individuum[bit].Cofactor(mgr->bddVar(level));
        }
        else if (parameters[i] > 0.5f)
        {
            individuum[bit] = individuum[bit].Cofactor(!mgr->bddVar(level));
        }
    }
    return individuum;
}

pagmo::vector_double BDDMinimizationProblem::fitness(const pagmo::vector_double& parameters) const
{
    const std::vector<BDD>& function = *original;

    std::vector<BDD> individuum_function = function_from_individuum(parameters);

    // compute error metrics
    double node_count = mgr->nodeCount(individuum_function);

    // if the created function is greater than the original, we do not need to compute the error
    // metrics as this indivuum is guaranteed to not lie in any reasonable pareto front as it is
    // dominated by the original un-approximated function
    if (node_count >= original_node_count)
    {
        // maximum values for the error metrics are used here to be safe
        if (!er_and_ace)
        {
            return {node_count * 5, 100};
        }
        else
        {
            return {node_count * 5, 100, max_average_case_error * 2};
        }
    }

    double error_rate = abo::error_metrics::error_rate(*mgr, function, individuum_function);

    double average_case_error = 0;
    if (er_and_ace)
    {
        average_case_error =
            double(abo::error_metrics::average_case_error(*mgr, function, individuum_function));
    }

    // punish error values above the maximum error metrics (the exact multipliers do not matter
    // here)
    if (error_rate > max_error_rate)
    {
        node_count *= error_rate * 10000 + 50;
    }
    if (average_case_error > max_average_case_error)
    {
        node_count *= 100 + average_case_error;
    }

    if (!er_and_ace)
    {
        return {node_count, error_rate};
    }
    else
    {
        return {node_count, error_rate, average_case_error};
    }
}

PAGMO_S11N_PROBLEM_IMPLEMENT(BDDMinimizationProblem)
