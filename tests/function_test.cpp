#include <catch2/catch.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <function.hpp>

using abo::util::Function;

TEST_CASE("operator []") {
    Cudd mgr;
    BDD f1 = mgr.bddOne();
    BDD f2 = mgr.bddVar(1);
    BDD f3 = mgr.bddVar(2);

    Function function(mgr, {f1, f2}, abo::util::NumberRepresentation::BaseTwo);
    REQUIRE(function[0] == f1);
    REQUIRE(function[1] == f2);

    // modifying
    function[0] = f3;
    REQUIRE(function[0] == f3);
    REQUIRE(function[1] == f2);
}

TEST_CASE("operator &") {
    Cudd mgr;
    BDD f1 = mgr.bddVar(0);
    BDD f2 = mgr.bddVar(1);
    BDD f3 = mgr.bddVar(2);

    Function function(mgr, {f1, f2}, abo::util::NumberRepresentation::BaseTwo);
    Function res = function & f3;
    REQUIRE(res[0] == (f1 & f3));
    REQUIRE(res[1] == (f2 & f3));
}

TEST_CASE("operator |") {
    Cudd mgr;
    BDD f1 = mgr.bddVar(0);
    BDD f2 = mgr.bddVar(1);
    BDD f3 = mgr.bddVar(2);

    Function function(mgr, {f1, f2}, abo::util::NumberRepresentation::BaseTwo);
    Function res = function | f3;
    REQUIRE(res[0] == (f1 | f3));
    REQUIRE(res[1] == (f2 | f3));
}

TEST_CASE("operator &=") {
    Cudd mgr;
    BDD f1 = mgr.bddVar(0);
    BDD f2 = mgr.bddVar(1);
    BDD f3 = mgr.bddVar(2);

    Function function(mgr, {f1, f2}, abo::util::NumberRepresentation::BaseTwo);
    function &= f3;
    REQUIRE(function[0] == (f1 & f3));
    REQUIRE(function[1] == (f2 & f3));
}

TEST_CASE("operator |=") {
    Cudd mgr;
    BDD f1 = mgr.bddVar(0);
    BDD f2 = mgr.bddVar(1);
    BDD f3 = mgr.bddVar(2);

    Function function(mgr, {f1, f2}, abo::util::NumberRepresentation::BaseTwo);
    function |= f3;
    REQUIRE(function[0] == (f1 | f3));
    REQUIRE(function[1] == (f2 | f3));
}

TEST_CASE("num bits") {
    Cudd mgr;
    BDD f = mgr.bddVar(1);

    Function function(mgr, {f, f, f, f}, abo::util::NumberRepresentation::BaseTwo);
    REQUIRE(function.num_bits() == 4UL);
}

TEST_CASE("is signed") {
    Cudd mgr;
    BDD f = mgr.bddVar(1);

    Function function(mgr, {f, f}, abo::util::NumberRepresentation::BaseTwo);
    REQUIRE(!function.is_signed());

    function = Function(mgr, {f, f}, abo::util::NumberRepresentation::TwosComplement);
    REQUIRE(function.is_signed());
}
