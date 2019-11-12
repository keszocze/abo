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





/**
 * @brief Hardcoded example from the 2016 ASP-DAC paper by Soeken et al.
 */
TEST_CASE("Example 3 from ASP-DAC 2016 paper") {
    Cudd mgr(2);

    std::vector<BDD> fun = abo::example_bdds::example3(mgr);

//    abo::util::dump_dot(mgr,fun,{"x_1", "x_2"},{"d_0", "d_1", "d_2", "d_3", "d_4"});




}
