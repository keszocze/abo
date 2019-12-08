#ifndef BENCHMARK_UTIL_H
#define BENCHMARK_UTIL_H

#include <string>
#include <cudd/cplusplus/cuddObj.hh>
#include <vector>

namespace abo::benchmark {

    enum class ErrorMetric {
        WORST_CASE,
        WORST_CASE_RELATIVE_APPROX,
        APPROXIMATE_WORST_CASE_5,
        AVERAGE_CASE,
        AVERAGE_RELATIVE_APPROX,
        AVERAGE_RELATIVE_ADD,
        MEAN_SQUARED,
        ERROR_RATE,
        AVERAGE_BIT_FLIP,
        WORST_CASE_BIT_FLIP,
    };

    std::string error_metric_name(ErrorMetric metric);

    double compute_error_metric(const Cudd &mgr, const std::vector<BDD> &original, const std::vector<BDD> &approx, ErrorMetric metric);

}

#endif // BENCHMARK_UTIL_H
