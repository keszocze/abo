//
// Created by keszocze on 10.10.18.
//

#include <error_rate.hpp>


#include <catch2/catch.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <cudd_helpers.hpp>
#include <simple.hpp>

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

    REQUIRE(1 == abo::error_metrics::error_rate(mgr,3, and_bdd,and_approx_bdd));

    and_approx_bdd = and_approx_bdd.Constrain(x);

    REQUIRE(3 == abo::error_metrics::error_rate(mgr,3, and_bdd,and_approx_bdd));

    and_approx_bdd = and_approx_bdd.Constrain(z);

    REQUIRE(7 == abo::error_metrics::error_rate(mgr,3, and_bdd,and_approx_bdd));


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

    REQUIRE(0 == abo::error_metrics::error_rate(mgr,n, or_bdd,or_pos_approx_bdd));
    REQUIRE(0 == abo::error_metrics::error_rate(mgr,n, or_bdd,or_neg_approx_bdd));

    for (size_t i = 0; i < n; i++) {
        or_pos_approx_bdd = or_pos_approx_bdd.Constrain(mgr.bddVar(i));
        or_neg_approx_bdd = or_neg_approx_bdd.Constrain(!mgr.bddVar(i));

        unsigned int expected_error = ((1 << (i+1)) - 1);


        auto computed_error = abo::error_metrics::error_rate(mgr,n, or_bdd,or_neg_approx_bdd);

//        std::cout << "i=" << i << "\texpected error=" << expected_error <<"\tcomputed error=" << computed_error << "\n";
        REQUIRE(expected_error == computed_error);
        REQUIRE(1 ==  abo::error_metrics::error_rate(mgr,n, or_bdd,or_pos_approx_bdd));
    }


}



TEST_CASE("Generic AND constraint error rate test"){


    constexpr size_t n = 20;

    Cudd mgr(n);

    BDD and_bdd = abo::example_bdds::and_bdd(mgr,n);

    BDD and_pos_approx_bdd = and_bdd;
    BDD and_neg_approx_bdd = and_bdd;

    REQUIRE(0 == abo::error_metrics::error_rate(mgr,n, and_bdd,and_pos_approx_bdd));
    REQUIRE(0 == abo::error_metrics::error_rate(mgr,n, and_bdd,and_neg_approx_bdd));

    for (size_t i = 0; i < n; i++) {
        and_pos_approx_bdd = and_pos_approx_bdd.Constrain(mgr.bddVar(i));
        and_neg_approx_bdd = and_pos_approx_bdd.Constrain(!mgr.bddVar(i));
        unsigned int expected_error = ((1 << (i+1)) - 1);

        // TODO sinnvolles overloads, so dsas man keine dämlichen ein-bdd vektoren mehr bauen muss

        auto computed_error = abo::error_metrics::error_rate(mgr,n, and_bdd,and_pos_approx_bdd);

//        std::cout << "i=" << i << "\texpected error=" << expected_error <<"\tcomputed error=" << computed_error << "\n";
        REQUIRE(expected_error == computed_error);
        REQUIRE(expected_error == abo::error_metrics::error_rate(mgr,n,and_bdd,and_neg_approx_bdd));
    }


}