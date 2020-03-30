//
// Created by keszocze on 12.10.18.
//

#include "simple.hpp"

namespace abo::example_bdds {
BDD and_bdd(const Cudd& mgr, const size_t n_vars)
{

    BDD and_bdd = mgr.bddOne();

    for (size_t i = 0; i < n_vars; i++)
    {
        and_bdd = and_bdd * mgr.bddVar(i);
    }
    return and_bdd;
}

BDD or_bdd(const Cudd& mgr, const size_t n_vars)
{

    BDD or_bdd = mgr.bddZero();

    for (size_t i = 0; i < n_vars; i++)
    {
        or_bdd = or_bdd + mgr.bddVar(i);
    }
    return or_bdd;
}

BDD xor_bdd(const Cudd& mgr, const size_t n_vars)
{

    BDD xor_bdd = mgr.bddZero();

    for (size_t i = 0; i < n_vars; i++)
    {
        xor_bdd = xor_bdd ^ mgr.bddVar(i);
    }
    return xor_bdd;
}
} // namespace abo::example_bdds