#pragma once

#include "number_representation.hpp"
#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::util {

// for integer (signed or unsigned) functions
class Function
{
public:
    Function(const Cudd &mgr, const std::vector<BDD> &value, NumberRepresentation representation);

private:
    Cudd mgr; // the manager is refcounted, each function can have a copy (safer than storing just a reference)
    std::vector<BDD> value;
    NumberRepresentation representation;
};

}
