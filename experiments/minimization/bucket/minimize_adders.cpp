#include <iostream>
#include <tuple>

#include "minimization_helper.hpp"

using namespace abo::minimization;
using std::cout;
using std::endl;

int main()
{
    cout << "size f;size fhat;error rate;average case error;runtime\n";

    std::vector<std::tuple<int, double>> parameters = {
        {8, 7}
        //        , {16, 35}
        //                                                       , {32, 9000}
    };
    for (auto [bits, ace_limit] : parameters)
    {
        MinimizationInputInfo info;
        info.input = "adder" + std::to_string(bits);
        info.metrics = {
            {20, ErrorMetric::ERROR_RATE, 0.05},
            {20, ErrorMetric::AVERAGE_CASE, ace_limit},
        };

        const std::vector<size_t> bounds{20,20};

        info.sift = false;
        info.operators = {Operator::POSITIVE_COFACTOR, Operator::NEGATIVE_COFACTOR};

        auto result = bucket_minimize_helper(info);
        cout << result.original_size << ";" << result.smallest_function.bdd_size << ";"
             << result.smallest_function.metric_values[0] << ";"
             << result.smallest_function.metric_values[1] << ";"
             << abo::minimization::format_time(result.minimization_time) << "\n";

        cout << endl << "Buckets:" << endl;

        size_t i = 0;
        for (auto bucket : result.all_buckets)
        {
            const auto idxs = create_multi_dim_index(i,bounds);
            for (size_t j: idxs) {
                cout << j << ";";
            }


            if (bucket.is_empty) {
                cout <<"NaN;NaN;NaN\n";
            }
            else
            {
                cout << bucket.bdd_size << ";" << bucket.metric_values[0] << ";"
                     << bucket.metric_values[1] << "\n";
            }

            ++i;
        }
        cout << endl;
    }
}
