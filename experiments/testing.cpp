#include <approximation_operators.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <cudd_helpers.hpp>
#include <dump_dot.hpp>
#include <from_papers.hpp>
#include <iostream>

using namespace abo;
using namespace util;
using namespace operators;
int main() {

    Cudd mgr(2);

    std::vector<BDD> f = abo::example_bdds::running_example(mgr);

    std::vector<BDD> rounded_up;

    for (const BDD &fi: f) {
        rounded_up.push_back(round_up(mgr, fi, 1, 1));
    }


    dump_dot_readable_to_file(mgr, rounded_up, "rounded.dot");
    dump_dot_readable(mgr, rounded_up, std::cout);


    dump_dot_readable_to_file(mgr, f, "running.dot");
    dump_dot_readable(mgr, f, std::cout);

}