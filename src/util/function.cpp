#include "function.hpp"
#include <algorithm>
#include <cassert>

namespace abo::util {

Function::Function(const Cudd &mgr, const std::vector<BDD> &value, NumberRepresentation representation) :
    mgr(mgr),
    value(value),
    representation(representation)
{
    assert(value.size() > 0);
}

BDD& Function::operator[](std::size_t index) {
    return value.at(index);
}

const BDD& Function::operator[](std::size_t index) const {
    return value.at(index);
}

Function Function::operator&(const BDD& b) const {
    Function result = *this;
    result &= b;
    return result;
}

Function Function::operator|(const BDD& b) const {
    Function result = *this;
    result |= b;
    return result;
}

void Function::operator&=(const BDD& b) {
    for (BDD& own : value) {
        own &= b;
    }
}

void Function::operator|=(const BDD& b) {
    for (BDD& own : value) {
        own |= b;
    }
}

}
