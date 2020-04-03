#pragma once

#include <cudd/cplusplus/cuddObj.hh>
#include <vector>

namespace abo::example_bdds {
/*
 * Various approximate adders.
 * These are ported from the matlab implementations from http://ces.itec.kit.edu/GeAR.php
 */

// num_bits is sometimes referred to as 'N', while partition size can be 'Q'
std::vector<BDD> almost_correct_adder_1(Cudd& mgr, unsigned int num_bits,
                                        unsigned int partition_size);

// partition size must be an even number
std::vector<BDD> almost_correct_adder_2(Cudd& mgr, unsigned int num_bits,
                                        unsigned int partition_size);

std::vector<BDD> gracefully_degrading_adder(Cudd& mgr, unsigned int num_bits,
                                            unsigned int num_partitions,
                                            unsigned int prediction_bits);

std::vector<BDD>
generic_accuracy_reconfigurable_adder(Cudd& mgr, unsigned int num_bits,
                                       unsigned int resultant_bits,
                                       unsigned int previous_bits);

std::vector<BDD> regular_adder(const Cudd& mgr, unsigned int num_bits);

} // namespace abo::example_bdds
