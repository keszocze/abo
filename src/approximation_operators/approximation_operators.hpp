#pragma once

#include <cudd/cplusplus/cuddObj.hh>

/**
 * @brief Namespace containing all approximation operators provided by abo
 */
namespace abo::operators {

/**
 * @brief subset_light_child Approximates the given BDD by setting one child of each node with a
 * variable level between level_start and level_end to 0. The child to subset is chosen as the one
 * with the smaller ON-set as computed by abo::util::count_minterms
 * @param mgr The cudd object manager to create nodes in
 * @param bdd The bdd to approximate
 * @param level_start The variable level to start approximating from
 * @param level_end The last variable level at which the approximation is applied (may be equal to
 * level_start when approximating only one level)
 * @return The approximated BDD
 */
BDD subset_light_child(const Cudd& mgr, const BDD& bdd, unsigned int level_start,
                       unsigned int level_end);

/**
 * @brief subset_light_child Approximates the given BDD by setting one child of each node with a
 * variable level between level_start and level_end to 1. The child to superset is chosen as the one
 * with the larger ON-set as computed by abo::util::count_minterms
 * @param mgr The cudd object manager to create nodes in
 * @param bdd The bdd to approximate
 * @param level_start The variable level to start approximating from
 * @param level_end The last variable level at which the approximation is applied (may be equal to
 * level_start when approximating only one level)
 * @return The approximated BDD
 */
BDD superset_heavy_child(const Cudd& mgr, const BDD& bdd, unsigned int level_start,
                         unsigned int level_end);

/**
 * @brief subset_light_child Approximates the given BDD by setting one child of each node with a
 * variable level between level_start and level_end to 1. The child to superset is chosen as the one
 * with the smaller ON-set as computed by abo::util::count_minterms
 * @param mgr The cudd object manager to create nodes in
 * @param bdd The bdd to approximate
 * @param level_start The variable level to start approximating from
 * @param level_end The last variable level at which the approximation is applied (may be equal to
 * level_start when approximating only one level)
 * @return The approximated BDD
 */
BDD superset_light_child(const Cudd& mgr, const BDD& bdd, unsigned int level_start,
                         unsigned int level_end);

/**
 * @brief subset_light_child Approximates the given BDD by setting one child of each node with a
 * variable level between level_start and level_end to 0. The child to subset is chosen as the one
 * with the larger ON-set as computed by abo::util::count_minterms
 * @param mgr The cudd object manager to create nodes in
 * @param bdd The bdd to approximate
 * @param level_start The variable level to start approximating from
 * @param level_end The last variable level at which the approximation is applied (may be equal to
 * level_start when approximating only one level)
 * @return The approximated BDD
 */
BDD subset_heavy_child(const Cudd& mgr, const BDD& bdd, unsigned int level_start,
                       unsigned int level_end);

/**
 * @brief Implements the round approximation operation from Soeken2016
 * @param mgr The manager for the BDD
 * @param bdd The BDD to be rounded-up
 * @param level The level at which to start the rounding
 * @return The rounded BDD
 */
BDD round_bdd(const Cudd& mgr, const BDD& bdd, unsigned int level);

/**
 * @brief New operation introduced for testing purposes. Like for example subset_light_child, it
 * chooses one child for each node to replace by a terminal node, but it chooses the child and the
 * corresponding terminal to minimize the resulting error rate
 * @param mgr The cudd object manager to create nodes in
 * @param bdd The bdd to approximate
 * @param level_start The variable level to start approximating from
 * @param level_end The last variable level at which the approximation is applied (may be equal to
 * level_start when approximating only one level)
 * @return The approximated BDD
 */
BDD round_best(const Cudd& mgr, const BDD& bdd, unsigned int level_start, unsigned int level_end);

/**
 * @brief Implements the round-up approximation operation from Soeken2016
 * @param mgr The manager for the BDD
 * @param bdd The BDD to be rounded-up
 * @param level_start The highest BDD level for the rounding operation
 * @param level_end The lowest BDD level (always the level of the terminals in the original paper)
 * @return The rounded BDD
 */
BDD round_up(const Cudd& mgr, const BDD& bdd, unsigned int level_start, unsigned int level_end);

/**
 * @brief Implements the round-down operation from Soeken2016
 * @param mgr The manager for the BDD
 * @param bdd The BDD to be rounded-down
 * @param level_start The highest BDD level for the rounding operation
 * @param level_end The lowest BDD level (always the level of the terminals in the original paper)
 * @return The rounded BDD
 */
BDD round_down(const Cudd& mgr, const BDD& bdd, unsigned int level_start, unsigned int level_end);

} // namespace abo::operators
