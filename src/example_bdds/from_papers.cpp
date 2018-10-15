//
// Created by keszocze on 15.10.18.
//

#include "from_papers.hpp"


namespace abo::example_bdds {

    std::vector <BDD> example3(const Cudd &mgr) {
        std::vector<BDD> fun;

        BDD x1 = mgr.bddVar(0);
        BDD x2 = mgr.bddVar(1);

        BDD cube1 = !x2 * !x1;
        BDD cube2 = !x2 * x1;
        BDD cube3 = x2 * !x1;
        BDD cube4 = x2*x1;

        BDD d0 = cube3;
        BDD d1 = cube1 | cube2;
        BDD d2 = cube2 | cube3 | cube4;
        BDD d3 = cube1 | cube3 | cube4;
        BDD d4 = mgr.bddZero();

        return {d0,d1,d2,d3,d4};
    }

}