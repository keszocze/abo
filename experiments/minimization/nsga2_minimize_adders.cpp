#include <iostream>
#include <string>
#include <vector>

#include "approximate_adders.hpp"
#include "minimization_helper.hpp"
#include "nsga2_minimization_helper.hpp"

int main()
{

    const double MAX_ERROR_RATE = 0.05;

    std::cout << "Bits, Orig. Size, Approx. Size, ER, ACR, Time" << std::endl;

    for (auto [bits, max_ace] : std::vector<std::pair<int, int>>{{8, 7}, {16, 35}, {32, 9000}})
    {
        Cudd mgr(2 * bits + 1);
        auto function = abo::example_bdds::regular_adder(mgr, bits);

        abo::nsga2minimization::MinimizationResult result =
            abo::nsga2minimization::run_genetic_minimization(mgr, function, MAX_ERROR_RATE, max_ace,
                                                             true);
        std::cout << bits << ", " << mgr.nodeCount(function) << ", " << result.node_count << ", "
                  << result.error_rate << ", " << result.average_case_error << ", "
                  << abo::minimization::format_time(result.computation_time) << std::endl;
    }

    return 0;
}
