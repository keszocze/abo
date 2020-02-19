//
// Created by keszocze on 15.10.18.
//

#pragma once

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::example_bdds {

/**
 * @brief BDD for Example 1 of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate computing,
 * in: Asia and South Pacific Design Automation Conference. Macau, China. https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vector of BDDs representing the function in Example 1 of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function. The caller has to
 * make sure that this makes sense in hist context.
 */
    std::vector<BDD> example3(Cudd &mgr);
}
