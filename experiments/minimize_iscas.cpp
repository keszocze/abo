#include <iostream>

#include "minimization_helper.hpp"

using namespace abo::minimization;
using std::cout;
using std::endl;

int main()
{
    cout << "Function name, original size, smallest size found, corresponding error rate, "
            "minimzation time"
         << endl;

    for (std::string file : {"c432", "c499", "c880", "c1355", "c1908", "c2670", "c3540"})
    {
        MinimizationInputInfo info;
        info.input = file;
        info.metrics = {
            {10, ErrorMetric::ERROR_RATE, 0.05},
        };
        info.sift = true;
        info.operators = {Operator::POSITIVE_COFACTOR, Operator::NEGATIVE_COFACTOR};

        auto res = bucket_minimize_helper(info);

        cout << file << ", " << res.original_size << ", " << res.smallest_function.bdd_size << ", "
             << res.smallest_function.metric_values[0] << ", "
             << abo::minimization::format_time(res.minimization_time) << endl;
    }
    return 0;
}
