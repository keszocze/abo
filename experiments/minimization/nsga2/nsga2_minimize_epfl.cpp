#include <iostream>
#include <string>
#include <vector>

#include "aig_parser.hpp"
#include "minimization_helper.hpp"
#include "../util/benchmark_util.hpp"
#include "nsga2_minimization_helper.hpp"

int main()
{

    using abo::benchmark::EPFLFile;

    const double MAX_ERROR_RATE = 0.05;

    std::cout << "File, Orig. Size, Approx. Size, ER, Time" << std::endl;

    for (EPFLFile file :
        {EPFLFile::Log2, EPFLFile::Adder, EPFLFile::Bar, EPFLFile::Div, EPFLFile::Hyp,  EPFLFile::Max, EPFLFile::Mul, EPFLFile::Sin, EPFLFile::Sqrt, EPFLFile::Square})
    {
        Cudd mgr(40);
        // allow dynamic variable reordering during reading to start with reasonable bdd sizes
        mgr.AutodynEnable();
        abo::parser::aig_parser parser(mgr);
        const std::vector<BDD> function = load_epfl_benchmark_file(mgr,file);
        // disable reordering as it can cause problems during the optimization process
        mgr.AutodynDisable();


        abo::nsga2minimization::MinimizationResult result =
            abo::nsga2minimization::run_genetic_minimization(mgr, function, MAX_ERROR_RATE, 0,
                                                             false);
        std::cout << epfl_filename_by_id(file) << ", " << mgr.nodeCount(function) << ", " << result.node_count << ", "
                  << result.error_rate << ", "
                  << abo::minimization::format_time(result.computation_time) << std::endl;
    }

    return 0;
}
