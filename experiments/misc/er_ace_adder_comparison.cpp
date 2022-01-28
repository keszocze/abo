#include <cudd/cplusplus/cuddObj.hh>

#include "../../src/example_bdds/approximate_adders.hpp"
#include "benchmark_util.hpp"
#include <average_case_error.hpp>
#include <iostream>

using namespace abo::benchmark;

static void generate_adders(ApproximateAdder adder, size_t n_bits, size_t param1, size_t param2)
{

    std::string adder_configuration = approximate_adder_name(adder, n_bits, param1, param2);

    //    state.SetLabel(adder_configuration + " - " + error_metric_name);
    Cudd mgr(n_bits + 1);
    auto correct = abo::example_bdds::regular_adder(mgr, n_bits);
    std::vector<BDD> approximate_adder = get_approximate_adder(mgr, adder, n_bits, param1, param2);

    double er = abo::benchmark::compute_error_metric(mgr, correct, approximate_adder, ErrorMetric::ERROR_RATE);
    double ace = abo::benchmark::compute_error_metric(mgr, correct, approximate_adder, ErrorMetric::AVERAGE_CASE);
    double wc = abo::benchmark::compute_error_metric(mgr, correct, approximate_adder, ErrorMetric::WORST_CASE);

    std::cout << adder_configuration << ";" << er << ";" << ace << ";" << wc << "\n";
}

int main()
{
//    generate_adders(ApproximateAdder::GEAR, 8, 1, 4);
//    generate_adders(ApproximateAdder::GEAR, 8, 2, 2);
//    generate_adders(ApproximateAdder::GEAR, 8, 4, 2);

//    generate_adders(ApproximateAdder::GEAR, 16, 4, 8);
//    generate_adders(ApproximateAdder::GEAR, 16, 8, 4);
    generate_adders(ApproximateAdder::GEAR, 16, 6, 4);
    generate_adders(ApproximateAdder::GEAR, 16, 4, 4);
    generate_adders(ApproximateAdder::GEAR, 16, 4, 8);
    generate_adders(ApproximateAdder::GEAR, 16, 2, 4);

//    generate_adders(ApproximateAdder::GEAR, 32, 15, 5);
//    generate_adders(ApproximateAdder::GEAR, 32, 20, 5);
//    generate_adders(ApproximateAdder::GEAR, 32, 15, 10);
//    generate_adders(ApproximateAdder::GEAR, 32, 20, 15);

//    generate_adders(ApproximateAdder::GEAR, 32, 3, 10);



    const size_t n = 3;
    Cudd mgr(n);

    std::vector<BDD> zero({mgr.bddVar(0), mgr.bddVar(1), mgr.bddVar(2)});
    std::vector<BDD> one({mgr.bddVar(0) | (mgr.bddVar(2) & mgr.bddVar(1)),  mgr.bddVar(1), mgr.bddVar(2)});

    std::cout << abo::error_metrics::average_case_error(mgr, one, zero) << "\n";

    return 0;
}