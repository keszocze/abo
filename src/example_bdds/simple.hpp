//
// Created by keszocze on 12.10.18.
//

#ifndef ABO_SIMPLE_HPP
#define ABO_SIMPLE_HPP

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::example_bdds {
    BDD and_bdd(const Cudd& mgr, const size_t n_vars);
    BDD or_bdd(const Cudd& mgr, const size_t n_vars);
    BDD xor_bdd(const Cudd& mgr, const size_t n_vars);
}

#endif //ABO_SIMPLE_HPP
