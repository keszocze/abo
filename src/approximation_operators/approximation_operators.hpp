#ifndef ABO_APPROXIMATION_OPERATORS_HPP
#define ABO_APPROXIMATION_OPERATORS_HPP

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::operators {

    BDD round_down(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD round_up(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD round(const Cudd &mgr, const BDD &bdd, unsigned int level);

}

#endif //ABO_APPROXIMATION_OPERATORS_HPP

