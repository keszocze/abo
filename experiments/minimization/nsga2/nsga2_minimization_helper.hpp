#ifndef NSGA2_MINIMIZATION_HELPER_HPP
#define NSGA2_MINIMIZATION_HELPER_HPP

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::nsga2minimization {

struct MinimizationResult
{
    std::size_t node_count;
    double error_rate;
    double average_case_error;    // only populated if it used during the minimization
    std::size_t computation_time; // in milliseconds
};

MinimizationResult run_genetic_minimization(const Cudd& mgr, const std::vector<BDD>& function,
                                            double max_error_rate, double max_average_case_error,
                                            bool use_both_metrics);

} // namespace abo::nsga2minimization

#endif // NSGA2_MINIMIZATION_HELPER_HPP
