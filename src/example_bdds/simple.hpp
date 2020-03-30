//
// Created by keszocze on 12.10.18.
//

#pragma once

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::example_bdds {
BDD and_bdd(const Cudd& mgr, const size_t n_vars);
BDD or_bdd(const Cudd& mgr, const size_t n_vars);
BDD xor_bdd(const Cudd& mgr, const size_t n_vars);
} // namespace abo::example_bdds
