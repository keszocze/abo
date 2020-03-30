//
// Created by keszocze on 15.10.18.
//

#pragma once

#include <cudd/cplusplus/cuddObj.hh>
#include <vector>

namespace abo::example_bdds {

/**
 * @brief BDD for Figure 1(a) of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate
 * computing, in: Asia and South Pacific Design Automation Conference. Macau, China.
 * https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Figure 1(a) of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function.
 * The caller has to make sure that this makes sense in hist context.
 */
std::vector<BDD> example1a(Cudd& mgr);

/**
 * @brief BDD for Figure 1(b) of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate
 * computing, in: Asia and South Pacific Design Automation Conference. Macau, China.
 * https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Figure 1(b) of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function.
 * The caller has to make sure that this makes sense in hist context.
 */
std::vector<BDD> example1b(Cudd& mgr);

/**
 * @brief BDD for Figure 1(c) of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate
 * computing, in: Asia and South Pacific Design Automation Conference. Macau, China.
 * https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Figure 1(c) of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function.
 * The caller has to make sure that this makes sense in hist context.
 */
std::vector<BDD> example1c(Cudd& mgr);

/**
 * @brief BDD for Figure 1(d) of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate
 * computing, in: Asia and South Pacific Design Automation Conference. Macau, China.
 * https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Figure 1(d) of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function.
 * The caller has to make sure that this makes sense in hist context.
 */
std::vector<BDD> example1d(Cudd& mgr);

/**
 * @brief BDD for Example 1 of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate
 * computing, in: Asia and South Pacific Design Automation Conference. Macau, China.
 * https://doi.org/10.1109/ASPDAC.2016.7428057
 *
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vector of BDDs representing the function in Example 1 of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function.
 * The caller has to make sure that this makes sense in hist context.
 */
std::vector<BDD> example3(Cudd& mgr);
} // namespace abo::example_bdds
