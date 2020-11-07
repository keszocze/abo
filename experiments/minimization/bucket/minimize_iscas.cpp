#include <iostream>

#include "../util/benchmark_util.hpp"
#include "minimization_helper.hpp"

using namespace abo::minimization;
using abo::benchmark::ISCAS85File;
using std::cout;
using std::endl;

int main()
{
    cout << "Function name, original size, smallest size found, corresponding error rate, "
            "runtime"
         << endl;

    for (ISCAS85File file :
         {ISCAS85File::C432, ISCAS85File::C499, ISCAS85File::C880, ISCAS85File::C1355,
          ISCAS85File::C1908, ISCAS85File::C2670, ISCAS85File::C3540})
    {
        MinimizationInputInfo info;
        info.input = iscas_85_filepath_by_id(file);
        info.metrics = {
            {10, ErrorMetric::ERROR_RATE, 0.05},
        };
        info.sift = true;
        info.operators = {Operator::POSITIVE_COFACTOR, Operator::NEGATIVE_COFACTOR};


        auto res = bucket_minimize_helper(info);


        cout << iscas_85_filename_by_id(file) << ", " << res.original_size << ", " << res.smallest_function.bdd_size << ", "
             << res.smallest_function.metric_values[0] << ", "
             << abo::minimization::format_time(res.minimization_time) << endl;
    }
    return 0;
}
