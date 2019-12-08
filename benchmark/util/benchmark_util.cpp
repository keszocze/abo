#include "benchmark_util.hpp"

#include "error_rate.hpp"
#include "average_case_error.hpp"
#include "worst_case_error.hpp"
#include "average_bit_flip_error.hpp"
#include "worst_case_bit_flip_error.hpp"
#include "worst_case_relative_error.hpp"
#include "average_case_relative_error.hpp"

namespace abo::benchmark {

std::string error_metric_name(ErrorMetric metric) {
    std::string name;
    switch (metric) {
    case ErrorMetric::WORST_CASE: name = "worst case"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_APPROX: name = "worst case relative"; break;
    case ErrorMetric::AVERAGE_CASE: name = "average case"; break;
    case ErrorMetric::MEAN_SQUARED: name = "mean squared"; break;
    case ErrorMetric::ERROR_RATE: name = "error rate"; break;
    case ErrorMetric::AVERAGE_BIT_FLIP: name = "average bit flip"; break;
    case ErrorMetric::WORST_CASE_BIT_FLIP: name = "worst case bit flip"; break;
    case ErrorMetric::APPROXIMATE_WORST_CASE_5: name = "approximate worst case - 5 bit"; break;
    case ErrorMetric::AVERAGE_RELATIVE_ADD: name = "acr add"; break;
    case ErrorMetric::AVERAGE_RELATIVE_APPROX: name = "acr approx"; break;
    }

    return name;
}

double compute_error_metric(const Cudd &mgr, const std::vector<BDD> &original, const std::vector<BDD> &approx, ErrorMetric metric) {
    switch (metric) {
    case ErrorMetric::WORST_CASE:
        return static_cast<double>(abo::error_metrics::worst_case_error(mgr, original, approx));
    case ErrorMetric::WORST_CASE_RELATIVE_APPROX:
    {
        auto bounds = abo::error_metrics::worst_case_relative_error_bounds(mgr, original, approx);
        return static_cast<double>((bounds.first + bounds.second) / 2.0);
    }
    case ErrorMetric::AVERAGE_CASE:
        return static_cast<double>(abo::error_metrics::average_case_error(mgr, original, approx));
    case ErrorMetric::MEAN_SQUARED:
        return static_cast<double>(abo::error_metrics::mean_squared_error(mgr, original, approx));
    case ErrorMetric::ERROR_RATE:
        return static_cast<double>(abo::error_metrics::error_rate(mgr, original, approx));
    case ErrorMetric::AVERAGE_BIT_FLIP:
        return static_cast<double>(abo::error_metrics::average_bit_flip_error(original, approx));
    case ErrorMetric::WORST_CASE_BIT_FLIP:
        return static_cast<double>(abo::error_metrics::worst_case_bit_flip_error(mgr, original, approx));
    case ErrorMetric::APPROXIMATE_WORST_CASE_5:
        return static_cast<double>(abo::error_metrics::approximate_worst_case_error(mgr, original, approx, 5));
    case ErrorMetric::AVERAGE_RELATIVE_ADD:
        return static_cast<double>(abo::error_metrics::average_relative_error_add(mgr, approx, original));
    case ErrorMetric::AVERAGE_RELATIVE_APPROX:
        auto bounds = abo::error_metrics::average_relative_error_bounds(mgr, approx, original);
        return static_cast<double>((bounds.first + bounds.second) / 2.0);
    }

    throw new std::logic_error("error metric not handled for computation");
}

}
