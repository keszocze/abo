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
    std::vector<BDD> expected_rounded_down = abo::example_bdds::example1b(mgr);
    std::vector<BDD> expected_rounded_up = abo::example_bdds::example1c(mgr);
    std::vector<BDD> expected_rounded = abo::example_bdds::example1d(mgr);


    std::vector<BDD> self_rounded_up, self_rounded_down, self_rounded;

    for (const BDD &b : fun) {
        self_rounded_down.push_back(abo::operators::round_down(mgr, b, 2, 4));
        self_rounded_up.push_back(abo::operators::round_up(mgr, b, 2, 4));
        self_rounded.push_back(abo::operators::round(mgr, b, 2));
    }

    CHECK(expected_rounded_down == self_rounded_down);

    CHECK(expected_rounded_up == self_rounded_up);

    CHECK(expected_rounded == self_rounded);
}
