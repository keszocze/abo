#include <iostream>
#include <string>
#include <vector>

#include "aig_parser.hpp"
#include "minimization_helper.hpp"
#include "nsga2_minimization_helper.hpp"

int main()
{

    const double MAX_ERROR_RATE = 0.05;

    std::cout << "File, Orig. Size, Approx. Size, ER, Time" << std::endl;

    std::vector<std::string> files = {"c432", "c499", "c880", "c1355", "c1908", "c2670", "c3540"};
    for (std::string file : files)
    {
        Cudd mgr(40);
        // allow dynamic variable reordering during reading to start with reasonable bdd sizes
        mgr.AutodynEnable();
        abo::parser::aig_parser parser(mgr);
        lorina::read_aiger("./iscas85/" + file + ".aig", parser);
        // disable reordering as it can cause problems during the optimization process
        mgr.AutodynDisable();

        const std::vector<BDD> function = parser.get_outputs();

        abo::nsga2minimization::MinimizationResult result =
            abo::nsga2minimization::run_genetic_minimization(mgr, function, MAX_ERROR_RATE, 0,
                                                             false);
        std::cout << file << ", " << mgr.nodeCount(function) << ", " << result.node_count << ", "
                  << result.error_rate << ", "
                  << abo::minimization::format_time(result.computation_time) << std::endl;
    }

    return 0;
}
