#ifndef ABO_APPROXIMATION_OPERATORS_HPP
#define ABO_APPROXIMATION_OPERATORS_HPP

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::operators {

    BDD subset_light_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD superset_heavy_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD superset_light_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD subset_heavy_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD round(const Cudd &mgr, const BDD &bdd, unsigned int level);

}

#endif //ABO_APPROXIMATION_OPERATORS_HPP

