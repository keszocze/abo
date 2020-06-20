#include "function.hpp"

namespace abo::util {

Function::Function(const Cudd &mgr, const std::vector<BDD> &value, NumberRepresentation representation) :
    mgr(mgr),
    value(value),
    representation(representation)
{ }

}
