#include "benchmark_util.hpp"

#include "average_bit_flip_error.hpp"
#include "average_case_error.hpp"
#include "average_case_relative_error.hpp"
#include "error_rate.hpp"
#include "worst_case_bit_flip_error.hpp"
#include "worst_case_error.hpp"
#include "worst_case_relative_error.hpp"

#include "aig_parser.hpp"
#include "approximate_adders.hpp"

namespace abo::benchmark {

std::string error_metric_name(ErrorMetric metric)
{
    std::string name;
    switch (metric)
    {
    case ErrorMetric::WORST_CASE: name = "worst case"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_APPROX: name = "worst case relative"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_BINARY_SEARCH: name = "wcr binary search"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_RANDOMIZED: name = "wcr randomized"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_ADD: name = "wcr ADD"; break;
    case ErrorMetric::WORST_CASE_RELATIVE_SYMBOLIC: name = "wcr symbolic"; break;
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

double compute_error_metric(const Cudd& mgr, const std::vector<BDD>& original,
                            const std::vector<BDD>& approx, ErrorMetric metric)
{
    switch (metric)
    {
    case ErrorMetric::WORST_CASE:
        return static_cast<double>(abo::error_metrics::worst_case_error(mgr, original, approx));
    case ErrorMetric::WORST_CASE_RELATIVE_APPROX: {
        auto bounds = abo::error_metrics::wcre_bounds(mgr, original, approx);
        return static_cast<double>((bounds.first + bounds.second) / 2.0);
    }
    case ErrorMetric::WORST_CASE_RELATIVE_BINARY_SEARCH:
        return static_cast<double>(abo::error_metrics::wcre_search(mgr, original, approx));
    case ErrorMetric::WORST_CASE_RELATIVE_RANDOMIZED:
    {
        auto fraction = abo::error_metrics::wcre_randomized_search(mgr, original, approx);
        return static_cast<double>(fraction.first) / fraction.second;
    }
    case ErrorMetric::WORST_CASE_RELATIVE_ADD:
        return static_cast<double>(abo::error_metrics::wcre_add(mgr, original, approx));
    case ErrorMetric::WORST_CASE_RELATIVE_SYMBOLIC:
        return static_cast<double>(abo::error_metrics::wcre_symbolic_division(mgr, original, approx));
    case ErrorMetric::AVERAGE_CASE:
        return static_cast<double>(abo::error_metrics::average_case_error(mgr, original, approx));
    case ErrorMetric::MEAN_SQUARED:
        return static_cast<double>(abo::error_metrics::mean_squared_error(mgr, original, approx));
    case ErrorMetric::ERROR_RATE:
        return static_cast<double>(abo::error_metrics::error_rate(mgr, original, approx));
    case ErrorMetric::AVERAGE_BIT_FLIP:
        return static_cast<double>(abo::error_metrics::average_bit_flip_error(original, approx));
    case ErrorMetric::WORST_CASE_BIT_FLIP:
        return static_cast<double>(
            abo::error_metrics::worst_case_bit_flip_error(mgr, original, approx));
    case ErrorMetric::APPROXIMATE_WORST_CASE_5:
        return static_cast<double>(
            abo::error_metrics::approximate_worst_case_error(mgr, original, approx, 5));
    case ErrorMetric::AVERAGE_RELATIVE_ADD:
        return static_cast<double>(abo::error_metrics::acre_add(mgr, approx, original));
    case ErrorMetric::AVERAGE_RELATIVE_APPROX:
        auto bounds = abo::error_metrics::acre_bounds(mgr, approx, original);
        return static_cast<double>((bounds.first + bounds.second) / 2.0);
    }

    throw new std::logic_error("error metric not handled for computation");
}

std::string iscas_85_filename_by_id(ISCAS85File file)
{
    const std::vector<std::string> files = {"c17.aig",   "c432.aig",  "c499.aig",  "c880.aig",
                                            "c1355.aig", "c1908.aig", "c2670.aig", "c3540.aig",
                                            "c5315.aig", "c6288.aig", "c7552.aig"};

    std::size_t file_id = static_cast<std::size_t>(file);
    if (file_id >= files.size())
    {
        throw new std::logic_error("Unknown ISCAS 85 file number!");
    }

    return files[file_id];
}

std::string iscas_85_filepath_by_id(ISCAS85File file)
{
    return "./iscas85/" + iscas_85_filename_by_id(file);
}

std::vector<BDD> load_iscas_85_file(Cudd& mgr, ISCAS85File file)
{

    std::string file_path = iscas_85_filepath_by_id(file);

    abo::parser::aig_parser parser(mgr);
    lorina::read_aiger(file_path, parser);

    return parser.get_outputs();
}

std::string epfl_filename_by_id(EPFLFile file)
{
    const std::vector<std::string> files = {"adder.aig", "bar.aig",   "div.aig",        "hyp.aig",
                                            "log2.aig",  "max.aig",   "multiplier.aig", "sin.aig",
                                            "sqrt.aig",  "square.aig"};

    std::size_t file_id = static_cast<std::size_t>(file);
    if (file_id >= files.size())
    {
        throw new std::logic_error("Unknown EPFL file number!");
    }

    return files[file_id];
}

std::string epfl_filepath_by_id(EPFLFile file)
{
    return "./epfl/" + epfl_filename_by_id(file);
}

std::vector<BDD> load_epfl_benchmark_file(Cudd& mgr, EPFLFile file)
{

    std::string filename = epfl_filepath_by_id(file);

    abo::parser::aig_parser parser(mgr);
    lorina::read_aiger(filename, parser);

    return parser.get_outputs();
}

std::string approximate_adder_name(ApproximateAdder adder, std::size_t bits, std::size_t par1,
                                   std::size_t par2)
{
    switch (adder)
    {
    case ApproximateAdder::ACA1:
        return "ACA_1_N" + std::to_string(bits) + "_Q" + std::to_string(par1);
    case ApproximateAdder::ACA2:
        return "ACA_2_N" + std::to_string(bits) + "_Q" + std::to_string(par1);
    case ApproximateAdder::GDA:
        return "GDA_St_N" + std::to_string(bits) + "_M" + std::to_string(par1) + "_P" +
               std::to_string(par2);
    case ApproximateAdder::GEAR:
        return "GeAr_N" + std::to_string(bits) + "_R" + std::to_string(par1) + "_P" +
               std::to_string(par2);
    }

    throw new std::logic_error("encountered unknown approximate adder");
}

std::vector<BDD> get_approximate_adder(Cudd& mgr, ApproximateAdder adder, std::size_t bits,
                                       std::size_t par1, std::size_t par2)
{
    switch (adder)
    {
    case ApproximateAdder::ACA1: return abo::example_bdds::almost_correct_adder_1(mgr, bits, par1);
    case ApproximateAdder::ACA2: return abo::example_bdds::almost_correct_adder_2(mgr, bits, par1);
    case ApproximateAdder::GDA:
        return abo::example_bdds::gracefully_degrading_adder(mgr, bits, par1, par2);
    case ApproximateAdder::GEAR:
        return abo::example_bdds::generic_accuracy_reconfigurable_adder(mgr, bits, par1, par2);
    }

    throw new std::logic_error("encountered unknown approximate adder");
}

} // namespace abo::benchmark
