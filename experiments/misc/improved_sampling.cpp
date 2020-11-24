#include <cudd/cplusplus/cuddObj.hh>

#include "../util/benchmark_util.hpp"
#include "approximation_operators.hpp"
#include "error_rate.hpp"

using namespace abo::benchmark;
using namespace abo::operators;

int main()
{

    Cudd mgr(0);

    const std::vector<BDD> original{load_iscas_85_file(mgr, ISCAS85File::C880)};

    std::cout << "Read file: " << original.size() << " outputs\n";

    std::vector<BDD> rounded;
    rounded.reserve(original.size());
    for (const BDD& orig : original)
    {
        rounded.push_back(subset_light_child(mgr, orig, 53, 10000000));
    }

    std::cout << "Approximated the function\n";

    const auto error_rate = abo::error_metrics::error_rate(mgr, original, rounded);

    std::cout << "error rate: " << error_rate << "\n";

    for (size_t n = 100; n <= 20000; n += 100)
    {
            const auto naive_sampling = abo::error_metrics::error_rate_sampling(mgr, original, rounded, n);
            const auto improved_sampling = abo::error_metrics::error_rate_efficient_sampling(mgr, original, rounded, n);

            std::cout << n << ";" << naive_sampling << ";" << improved_sampling << "\n";
    }

    return 0;
}