//
// Created by keszocze on 10.10.18.
//

#include <boost/multiprecision/cpp_int.hpp>
#include <catch2/catch.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <cudd_helpers.hpp>
#include <simple.hpp>
#include <from_papers.hpp>


#include <iostream>
#include <approximation_operators.hpp>





/**
 * @brief Hardcoded example from the 2016 ASP-DAC paper by Soeken et al.
 */
TEST_CASE("Example 3 from ASP-DAC 2016 paper") {
    Cudd mgr(2);

    std::vector<BDD> fun = abo::example_bdds::example1a(mgr);
    std::vector<BDD> rounded_up_fun = abo::example_bdds::example1c(mgr);

    std::vector<BDD> self_rounded;

    for (const BDD &f: fun) {
        self_rounded.push_back(abo::operators::subset_heavy_child(mgr,f,3,4));
    }


    abo::util::dump_dot(mgr,self_rounded,{"x_1", "x_2", "x_3", "x_4"},{"f0", "f1", "f2"});




}
