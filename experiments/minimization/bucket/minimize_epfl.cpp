#include <iostream>

#include "../util/benchmark_util.hpp"
#include "minimization_helper.hpp"

using namespace abo::minimization;
using abo::benchmark::EPFLFile;
using std::cout;
using std::endl;

int main()
{
    cout << "Function name, original size, smallest size found, corresponding error rate, "
            "runtime"
         << endl;

    for (EPFLFile file :
         {EPFLFile::Log2, EPFLFile::Adder, EPFLFile::Bar, EPFLFile::Div, EPFLFile::Hyp,  EPFLFile::Max, EPFLFile::Mul, EPFLFile::Sin, EPFLFile::Sqrt, EPFLFile::Square})
    {
        MinimizationInputInfo info;
        info.input = epfl_filepath_by_id(file);
        info.metrics = {
            {10, ErrorMetric::ERROR_RATE, 0.05},
        };
        info.sift = true;
        info.operators = {Operator::POSITIVE_COFACTOR, Operator::NEGATIVE_COFACTOR};


        auto res = bucket_minimize_helper(info);


        cout << epfl_filename_by_id(file) << ", " << res.original_size << ", " << res.smallest_function.bdd_size << ", "
             << res.smallest_function.metric_values[0] << ", "
             << abo::minimization::format_time(res.minimization_time) << endl;
    }
    return 0;
}
