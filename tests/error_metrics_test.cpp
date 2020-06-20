//
// Created by keszocze on 10.10.18.
//

#include <error_rate.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <catch2/catch.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <cudd_helpers.hpp>
#include <simple.hpp>
#include <from_papers.hpp>
#include <worst_case_error.hpp>
#include <average_case_error.hpp>
#include <worst_case_relative_error.hpp>

#include <iostream>


TEST_CASE("Trivial Error Rate Test") {
    Cudd mgr(2);

    mgr.pushVariableName("x");
    mgr.pushVariableName("y");
    mgr.pushVariableName("z");

    std::vector<std::string> varnames = {"x","y","z"};
    std::vector<std::string> funnames = {"and", "and_hat"};

    BDD x = mgr.bddVar(0);
    BDD y = mgr.bddVar(1);
    BDD z = mgr.bddVar(2);


    BDD and_bdd = x*y*z;

    BDD and_approx_bdd = and_bdd.Constrain(y);

    std::vector<BDD> funs{and_bdd, and_approx_bdd};

//    abo::util::dump_dot(mgr,funs[0],varnames,funnames[0]);
//
//    abo::util::dump_dot(mgr,funs, varnames, funnames);

    REQUIRE(1.0 / 8.0 == abo::error_metrics::error_rate(mgr,and_bdd,and_approx_bdd));

    and_approx_bdd = and_approx_bdd.Constrain(x);

    REQUIRE(3.0 / 8.0 == abo::error_metrics::error_rate(mgr,and_bdd,and_approx_bdd));

    and_approx_bdd = and_approx_bdd.Constrain(z);

    REQUIRE(7.0 / 8.0 == abo::error_metrics::error_rate(mgr,and_bdd,and_approx_bdd));


}


/**
 * @brief Hardcoded example from the 2016 ASP-DAC paper by Soeken et al.
 */
TEST_CASE("Example 3 from ASP-DAC 2016 paper") {
    Cudd mgr(2);

    std::vector<BDD> fun = abo::example_bdds::example3(mgr);

//    abo::util::dump_dot(mgr,fun,{"x_1", "x_2"},{"d_0", "d_1", "d_2", "d_3", "d_4"});



    boost::multiprecision::uint256_t max_error = abo::error_metrics::get_max_value(mgr, fun);

    REQUIRE(13 == max_error);

}

TEST_CASE("Trivial worst_case_error") {
    Cudd mgr(2);

    std::vector<BDD> zero_fun{mgr.bddZero(), mgr.bddZero()};
    std::vector<BDD> one_fun{mgr.bddOne(), mgr.bddOne()};


//    std::cout << abo::error_metrics::worst_case_error(mgr, zero_fun, one_fun);

}


TEST_CASE("Generic OR constraint error rate test"){


    constexpr size_t n = 30;

    Cudd mgr(n);

    BDD or_bdd = abo::example_bdds::or_bdd(mgr,n);

    // TODO Methoden für die generische Erzeugung von AND, OR, XOR etc. BDDs erzeugen

    for (size_t i = 0; i < n; i++) {
        or_bdd = or_bdd + mgr.bddVar(i);
    }


    BDD or_pos_approx_bdd = or_bdd;
    BDD or_neg_approx_bdd = or_bdd;

    REQUIRE(0 == abo::error_metrics::error_rate(mgr, or_bdd,or_pos_approx_bdd));
    REQUIRE(0 == abo::error_metrics::error_rate(mgr, or_bdd,or_neg_approx_bdd));

    for (size_t i = 0; i < n; i++) {
        or_pos_approx_bdd = or_pos_approx_bdd.Constrain(mgr.bddVar(i));
        or_neg_approx_bdd = or_neg_approx_bdd.Constrain(!mgr.bddVar(i));

        double expected_error = double((1L << (i+1)) - 1) / double(1L << n);


        auto computed_error = abo::error_metrics::error_rate(mgr, or_bdd,or_neg_approx_bdd);

//        std::cout << "i=" << i << "\texpected error=" << expected_error <<"\tcomputed error=" << computed_error << "\n";
        REQUIRE(expected_error == computed_error);
        REQUIRE(1.0 / double(1L << n) ==  abo::error_metrics::error_rate(mgr, or_bdd,or_pos_approx_bdd));
    }


}



TEST_CASE("Generic AND constraint error rate test"){


    constexpr size_t n = 20;

    Cudd mgr(n);

    BDD and_bdd = abo::example_bdds::and_bdd(mgr,n);

    BDD and_pos_approx_bdd = and_bdd;
    BDD and_neg_approx_bdd = and_bdd;

    REQUIRE(0 == abo::error_metrics::error_rate(mgr, and_bdd,and_pos_approx_bdd));
    REQUIRE(0 == abo::error_metrics::error_rate(mgr, and_bdd,and_neg_approx_bdd));

    for (size_t i = 0; i < n; i++) {
        and_pos_approx_bdd = and_pos_approx_bdd.Constrain(mgr.bddVar(i));
        and_neg_approx_bdd = and_pos_approx_bdd.Constrain(!mgr.bddVar(i));
        double expected_error = double((1L << (i+1)) - 1) / double(1L << n);

        // TODO sinnvolles overloads, so dsas man keine dämlichen ein-bdd vektoren mehr bauen muss

        auto computed_error = abo::error_metrics::error_rate(mgr, and_bdd,and_pos_approx_bdd);

//        std::cout << "i=" << i << "\texpected error=" << expected_error <<"\tcomputed error=" << computed_error << "\n";
        REQUIRE(expected_error == computed_error);
        REQUIRE(expected_error == abo::error_metrics::error_rate(mgr,and_bdd,and_neg_approx_bdd));
    }


}


TEST_CASE("Trivial average case error") {

    const size_t n = 3;
    Cudd mgr(n);

    std::vector<BDD> zero({mgr.bddZero(), mgr.bddZero()});
    std::vector<BDD> one({mgr.bddOne(), mgr.bddOne()});

    REQUIRE(abo::error_metrics::average_case_error(mgr, one, zero) == 3);
}

static void check_wcr_values(Cudd &mgr, const std::vector<BDD> &f,
                             const std::vector<BDD> &g, long desired_wcr) {
    auto f_ = f;
    auto g_ = g;

    // equalize function sizes (is necessary for the error metrics)
    while (f_.size() < g_.size()) f_.push_back(mgr.bddZero());
    while (g_.size() < f_.size()) g_.push_back(mgr.bddZero());

    double bin_search = abo::error_metrics::wcre_search(mgr, f, g);
    REQUIRE(desired_wcr == bin_search);

    auto ran_search = abo::error_metrics::wcre_randomized_search(mgr, f, g);
    REQUIRE(desired_wcr * ran_search.second == ran_search.first);

    auto symbolic = abo::error_metrics::wcre_symbolic_division(mgr, f, g);
    REQUIRE(desired_wcr == symbolic);

    auto add = abo::error_metrics::wcre_symbolic_division(mgr, f, g);
    REQUIRE(desired_wcr == add);
}

TEST_CASE("Simple worst case relative error metrics") {
    Cudd mgr(256);
    auto f1 = abo::util::number_to_bdds(mgr, 1);

    // check some static values
    for (long f2val : {0, 1, 5, 21, 1232345}) {
        auto f2 = abo::util::number_to_bdds(mgr, f2val + 1);
        check_wcr_values(mgr, f1, f2, f2val);
    }

    // check division by zero not occuring
    check_wcr_values(mgr, abo::util::number_to_bdds(mgr, 0), f1, 1);

    // division is performed
    check_wcr_values(mgr, abo::util::number_to_bdds(mgr, 3),
                     abo::util::number_to_bdds(mgr, 18), 5);
}
