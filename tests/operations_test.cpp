#include <catch2/catch.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <cudd_helpers.hpp>
#include <from_papers.hpp>
#include <iostream>


TEST_CASE("Test child operators") {

    // hand-crafted test cases for understanding whether the low/how operators actually work as expected

    Cudd mgr(2);

    std::vector<BDD> f = abo::example_bdds::example3(mgr);

    BDD lf0 = abo::util::low(mgr,f.at(0));
    BDD hf0 = abo::util::high(mgr,f.at(0));

    CHECK(mgr.bddZero() == hf0);

    BDD tmp = !mgr.bddVar(1);

    CHECK(tmp == lf0);

    auto pred = Catch::Predicate<std::invalid_argument>(
            [] (std::invalid_argument const & e) -> bool {
                std::string msg{e.what()};
                return msg.find("Cannot retrieve child of a terminal node");;
            },
            "Cannot retrieve child of a terminal node");

    // Note that these tests are rather brittle as someone might change the actual exception text
    CHECK_THROWS_MATCHES(abo::util::low(mgr,f.at(4)), std::invalid_argument, pred);
    CHECK_THROWS_MATCHES(abo::util::high(mgr,f.at(4)), std::invalid_argument, pred);


//    abo::util::dump_dot(mgr,f);
//    abo::util::dump_dot(mgr,lf0);
//    abo::util::dump_dot(mgr,hf0);

}

TEST_CASE("Dinge") {
    Cudd mgr(2);

    mgr.pushVariableName("x");
    mgr.pushVariableName("y");
    mgr.pushVariableName("z");

    std::vector<std::string> varnames = {"x", "y", "z"};
    std::vector<std::string> funnames = {"and", "or", "xor"};

    BDD x = mgr.bddVar(0);
    BDD y = mgr.bddVar(1);
    BDD z = mgr.bddVar(2);


    BDD and_bdd = x * y * z;
    BDD or_bdd = x + y + z;
    BDD xor_bdd = x ^y ^z;

    std::vector<BDD> funs{and_bdd, or_bdd, xor_bdd};


//    abo::util::dump_dot(mgr, funs, varnames, funnames);

    BDD and_bdd_x = and_bdd.Constrain(x);
    funs.push_back(and_bdd_x);
    funnames.push_back("and_x");

    BDD or_bdd_x = or_bdd.Constrain(x);
    funs.push_back(or_bdd_x);
    funnames.push_back("or_x");

    BDD xor_bdd_x = xor_bdd.Constrain(x);
    funs.push_back(xor_bdd_x);
    funnames.push_back("xor_x");


    BDD and_bdd_y = and_bdd.Constrain(y);
    funs.push_back(and_bdd_y);
    funnames.push_back("and_y");

    BDD or_bdd_y = or_bdd.Constrain(y);
    funs.push_back(or_bdd_y);
    funnames.push_back("or_y");

    BDD xor_bdd_y = xor_bdd.Constrain(y);
    funs.push_back(xor_bdd_y);
    funnames.push_back("xor_y");


    BDD and_bdd_ny = and_bdd.Constrain(!y);
    funs.push_back(and_bdd_ny);
    funnames.push_back("and_!y");

    BDD or_bdd_ny = or_bdd.Constrain(!y);
    funs.push_back(or_bdd_ny);
    funnames.push_back("or_!y");

    BDD xor_bdd_ny = xor_bdd.Constrain(!y);
    funs.push_back(xor_bdd_ny);
    funnames.push_back("xor_!y");

//    abo::util::dump_dot(mgr, funs, varnames, funnames);

}

TEST_CASE("Cofactor works as expected") {
    Cudd mgr(2);

    BDD x = mgr.bddVar(0);
    BDD y = mgr.bddVar(1);
    BDD z = mgr.bddVar(2);

    BDD or_bdd = x + y;
    BDD and_bdd = x * y;
    BDD xor_bdd = x ^y ^z;

    std::vector<std::string> varnames = {"x", "y", "z"};


    BDD m = or_bdd.Cofactor(x);
    CHECK(m.IsOne() == true);
    CHECK(m.IsZero() == false);
    CHECK(m.IsCube() == true);
//    abo::util::dump_dot(mgr,{m},varnames);

    m = and_bdd.Cofactor(x);
    CHECK(m.IsOne() == false);


    // warum ist das nicht false?!?!
//    CHECK(m.IsCube() == false);
    CHECK(m.IsZero() == false);


    m = or_bdd.Cofactor(!x);
    CHECK(m.IsOne() == false);
    CHECK(m.IsZero() == false);

    m = and_bdd.Cofactor(!x);
    CHECK(m.IsOne() == false);
    CHECK(m.IsZero() == true);
    CHECK(m.IsCube() == false); // const zero is no cube according to docs


    m = xor_bdd.Constrain(x * z);

}

TEST_CASE("Difference in Cofactor and Constrain(?)") {
    constexpr size_t n = 30;

    Cudd mgr(n);

    BDD or_bdd = mgr.bddZero();
    BDD and_bdd = mgr.bddOne();
    BDD xor_bdd = mgr.bddZero();

    // TODO Methoden für die generische Erzeugung von AND, OR, XOR etc. BDDs erzeugen

    for (size_t i = 0; i < n; i++) {
        or_bdd = or_bdd + mgr.bddVar(i);
        and_bdd = and_bdd * mgr.bddVar(i);
        xor_bdd = xor_bdd * mgr.bddVar(i);
    }


    // these loops are far from what i wanted to test but give me some confidence nevertheless
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= i; j++) {

            BDD pos_cube = mgr.bddOne();
            BDD neg_cube = mgr.bddOne();

            for (size_t k = 0; k <= j; k++) {
                pos_cube = pos_cube * mgr.bddVar(k);
                neg_cube = neg_cube * !mgr.bddVar(k);
            }
            BDD pos_or_cons = or_bdd.Constrain(pos_cube);
            BDD pos_or_cof = or_bdd.Cofactor(pos_cube);
            BDD neg_or_cons = or_bdd.Constrain(neg_cube);
            BDD neg_or_cof = or_bdd.Cofactor(neg_cube);

            CHECK(pos_or_cons == pos_or_cof);
            CHECK(neg_or_cons == neg_or_cof);

            BDD pos_and_cons = and_bdd.Constrain(pos_cube);
            BDD pos_and_cof = and_bdd.Cofactor(pos_cube);
            BDD neg_and_cons = and_bdd.Constrain(neg_cube);
            BDD neg_and_cof = and_bdd.Cofactor(neg_cube);

            CHECK(pos_and_cons == pos_and_cof);
            CHECK(neg_and_cons == neg_and_cof);

            BDD pos_xor_cons = xor_bdd.Constrain(pos_cube);
            BDD pos_xor_cof = xor_bdd.Cofactor(pos_cube);
            BDD neg_xor_cons = xor_bdd.Constrain(neg_cube);
            BDD neg_xor_cof = xor_bdd.Cofactor(neg_cube);

            CHECK(pos_xor_cons == pos_xor_cof);
            CHECK(neg_xor_cons == neg_xor_cof);
        }
    }

}

