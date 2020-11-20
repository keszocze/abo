//
// Created by keszocze on 15.10.18.
//

#include "from_papers.hpp"

#include <iostream>

namespace abo::example_bdds {

constexpr bool USE_VARIABLE_NAMES = false;

std::vector<BDD> example1a(Cudd& mgr)
{
    BDD x1 = mgr.bddVar(0);
    BDD x2 = mgr.bddVar(1);
    BDD x3 = mgr.bddVar(2);
    BDD x4 = mgr.bddVar(3);

    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_1");
        mgr.pushVariableName("x_2");
        mgr.pushVariableName("x_3");
        mgr.pushVariableName("x_4");
    }

    BDD cube01 = !x1 * x2;
    BDD cube02 = x1 * !x2;

    BDD cube11 = !x1 * x3 * !x4;
    BDD cube12 = !x1 * !x3 * x4;
    BDD cube13 = x1 * !x2 * x3 * !x4;
    BDD cube14 = x1 * !x2 * !x3 * x4;
    BDD cube15 = x1 * x2 * x3 * x4;
    BDD cube16 = x1 * x2 * !x3 * !x4;

    BDD cube21 = x1 * x2 * x3;
    BDD cube22 = x1 * x2 * !x3 * x4;
    BDD cube23 = x1 * !x2 * x3 * x4;
    BDD cube24 = !x1 * x3 * x4;

    BDD f0 = cube01 | cube02;
    BDD f1 = cube11 | cube12 | cube13 | cube14 | cube15 | cube16;
    BDD f2 = cube21 | cube22 | cube23 | cube24;

    return {f0, f1, f2};
}

std::vector<BDD> example1b(Cudd& mgr)
{
    BDD x1 = mgr.bddVar(0);
    BDD x2 = mgr.bddVar(1);
    BDD x3 = mgr.bddVar(2);
    BDD x4 = mgr.bddVar(3);

    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_1");
        mgr.pushVariableName("x_2");
        mgr.pushVariableName("x_3");
        mgr.pushVariableName("x_4");
    }

    BDD cube01 = !x1 * x2;
    BDD cube02 = x1 * !x2;

    BDD cube11 = x1 * x2 * !x3 * !x4;
    BDD cube12 = !x1 * !x3 * x4;
    BDD cube13 = x1 * !x2 * !x3 * x4;

    BDD cube21 = x1 * x2 * !x3 * x4;
    BDD cube22 = x1 * !x2 * x3 * x4;
    BDD cube23 = !x1 * x3 * x4;

    BDD f0 = cube01 | cube02;
    BDD f1 = cube11 | cube12 | cube13;
    BDD f2 = cube21 | cube22 | cube23;

    return {f0, f1, f2};
}

std::vector<BDD> example1c(Cudd& mgr)
{
    BDD x1 = mgr.bddVar(0);
    BDD x2 = mgr.bddVar(1);
    BDD x3 = mgr.bddVar(2);
    BDD x4 = mgr.bddVar(3);

    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_1");
        mgr.pushVariableName("x_2");
        mgr.pushVariableName("x_3");
        mgr.pushVariableName("x_4");
    }

    BDD f0 = (x1 * !x2) | (!x1 * x2);
    BDD f1 = mgr.bddOne();
    BDD f2 = mgr.bddOne();

    return {f0, f1, f2};
}

std::vector<BDD> example1d(Cudd& mgr)
{
    BDD x1 = mgr.bddVar(0);
    BDD x2 = mgr.bddVar(1);
    BDD x3 = mgr.bddVar(2);
    BDD x4 = mgr.bddVar(3);

    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_1");
        mgr.pushVariableName("x_2");
        mgr.pushVariableName("x_3");
        mgr.pushVariableName("x_4");
    }

    BDD f0 = (x1 * !x2) | (!x1 * x2);
    BDD f1 = mgr.bddZero();
    BDD f2 = x1 * x2;

    return {f0, f1, f2};
}

std::vector<BDD> example3(Cudd& mgr)
{

    BDD x1 = mgr.bddVar(0);
    BDD x2 = mgr.bddVar(1);

    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_1");
        mgr.pushVariableName("x_2");
    }

    BDD cube1 = !x2 * !x1;
    BDD cube2 = !x2 * x1;
    BDD cube3 = x2 * !x1;
    BDD cube4 = x2 * x1;

    BDD d0 = cube3;
    BDD d1 = cube1 | cube2;
    BDD d2 = cube2 | cube3 | cube4;
    BDD d3 = cube1 | cube3 | cube4;
    BDD d4 = mgr.bddZero();

    return {d0, d1, d2, d3, d4};
}


std::vector<BDD> running_example(Cudd& mgr)
{

    BDD x1 = mgr.bddVar(2);
    BDD x2 = mgr.bddVar(1);
    BDD x3 = mgr.bddVar(0);

//    if (USE_VARIABLE_NAMES) {
        mgr.pushVariableName("x_a1");
        mgr.pushVariableName("x_2");
        mgr.pushVariableName("x_3");
//    }

    BDD cube1 = !x3 * !x2 * !x1;
    BDD cube2 = !x3 * !x2 * x1;
    BDD cube3 = !x3 * x2 * !x1;
    BDD cube4 = !x3 * x2 * x1;
    BDD cube5 = x3 * !x2 * !x1;
    BDD cube6 = x3 * !x2 * x1;
    BDD cube7 = x3 * x2 * !x1;
    BDD cube8 = x3 * x2 * x1;

    BDD d0 = cube1 | cube2 | cube3 | cube5 | cube8;
    BDD d1 = cube4 | cube5 | cube6 | cube7;

    return {d0, d1};
}

} // namespace abo::example_bdds
