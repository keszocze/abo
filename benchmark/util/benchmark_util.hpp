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


    enum class ISCAS85File {
        C17 = 0, C432 = 1, C499 = 2, C880 = 3,
        C1355 = 4, C1908 = 5, C2670 = 6, C3540 = 7,
        C5315 = 8, C6288 = 9, C7552 = 10
    };

    std::string iscas_85_filename_by_id(ISCAS85File file);

    std::vector<BDD> load_iscas_85_file(Cudd &mgr, ISCAS85File file);

}

#endif // BENCHMARK_UTIL_H
