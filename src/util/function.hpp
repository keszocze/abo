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

    // bit access
    BDD& operator[](std::size_t index);
    const BDD& operator[](std::size_t index) const;

    // bit-wise operations
    Function operator&(const BDD& b) const;
    Function operator|(const BDD& b) const;
    void operator&=(const BDD& b);
    void operator|=(const BDD& b);

private:
    Cudd mgr; // the manager is refcounted, each function can have a copy (safer than storing just a reference)
    std::vector<BDD> value;
    NumberRepresentation representation;
};

}
