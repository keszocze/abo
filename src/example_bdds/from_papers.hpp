//
// Created by keszocze on 15.10.18.
//

#ifndef ABO_FROM_PAPERS_HPP
#define ABO_FROM_PAPERS_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::example_bdds {

/**
 * @brief BDD for Example 1 of Soeken et al. BDD Minimization for Approximate Computing
 *
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., & Drechsler, R. (2016, January).
 * BDD minimization for approximate computing. In Design Automation Conference (ASP-DAC),
 * 2016 21st Asia and South Pacific (pp. 474-479). IEEE.
 *
 *
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Example 1 of the aforementioned paper
 * @warning The method uses the first four bdd variables within the manager to create the function. The caller has to
 * make sure that this makes sense in hist context.
 */
    std::vector <BDD> example3(const Cudd &mgr);

/**
 * @brief BDD for Example 3 of Soeken et al. BDD Minimization for Approximate Computing
 *
 * This function represents the absolute output differences of a function f and an approximated version of that
 * functions, f_hat. Computing the maximal value of thie BDD returned by this function therefore computes the worst
 * case error between f and f_hat.
 *
 * The paper is:
 * Soeken, M., Große, D., Chandrasekharan, A., & Drechsler, R. (2016, January).
 * BDD minimization for approximate computing. In Design Automation Conference (ASP-DAC),
 * 2016 21st Asia and South Pacific (pp. 474-479). IEEE.
 *
 * @param mgr The manager used for creating the BDD
 * @return Vectr of BDDs representing the function in Example 3 of the abovementioned paper
 * @warning The method uses the first two bdd variables within the manager to create the function. The caller has to
 * make sure that this makes sense in hist context.
 */
    std::vector <BDD> example3(const Cudd &mgr);
}
#endif //ABO_FROM_PAPERS_HPP
