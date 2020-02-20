#pragma once

#include <cudd/cplusplus/cuddObj.hh>

/**
 * @brief Namespace containing all approximation operators provided by abo
 */
namespace abo::operators {

    /**
     * @brief Implements the round-down operation from Soeken2016
     * @param mgr The manager for the BDD
     * @param bdd The BDD to be rounded-down
     * @param level_start The highest BDD level for the rounding operation
     * @param level_end The lowest BDD level (always the level of the terminals in the original paper)
     * @return The rounded BDD
     */
    BDD subset_light_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD superset_heavy_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);


    /**
     * @brief Implements the round-up approximation operation from Soeken2016
     * @param mgr The manager for the BDD
     * @param bdd The BDD to be rounded-up
     * @param level_start The highest BDD level for the rounding operation
     * @param level_end The lowest BDD level (always the level of the terminals in the original paper)
     * @return The rounded BDD
     */
    BDD superset_light_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

    BDD subset_heavy_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);


    /**
     * @brief Implements the round approximation operation from Soeken2016
     * @param mgr The manager for the BDD
     * @param bdd The BDD to be rounded-up
     * @param level The level at which to start the rounding
     * @return The rounded BDD
     */
    BDD round(const Cudd &mgr, const BDD &bdd, unsigned int level);

    /**
     * @brief New operation by
     * @param mgr
     * @param bdd
     * @param level_start
     * @param level_end
     * @return
     */
    BDD round_best(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end);

}
