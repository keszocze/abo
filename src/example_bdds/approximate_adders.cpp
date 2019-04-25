#include "approximate_adders.hpp"
#include <cassert>
#include <cudd_helpers.hpp>

namespace abo::example_bdds {

    static BDD carry_generate(const BDD &a, const BDD &b, const BDD carry_in) {
        return (a * b) | (a * carry_in) | (b * carry_in);
    }

    std::vector <BDD> almost_correct_adder_1(Cudd &mgr, unsigned int num_bits, unsigned int partition_size) {
        assert(num_bits >= partition_size && partition_size > 0);

        std::vector<BDD> input1, input2;
        input1.reserve(num_bits);
        input2.reserve(num_bits);
        for (unsigned int i = 0;i<num_bits;i++) {
            input1.push_back(mgr.bddVar(i));
            input2.push_back(mgr.bddVar(i + num_bits));
        }

        std::vector<BDD> carry_outs;
        carry_outs.reserve(num_bits);
        for (unsigned int i = 0;i<partition_size-1;i++) {
            if (i == 0) {
                carry_outs.push_back(carry_generate(input1[i], input2[i], mgr.bddZero()));
            } else {
                carry_outs.push_back(carry_generate(input1[i], input2[i], carry_outs.back()));
            }
        }

        for (unsigned int i = partition_size-1;i<num_bits;i++) {
            BDD temp_carry = mgr.bddZero();
            for (int j = partition_size-2;j >= 0;j--) {
                if (i-j >= 0) {
                    BDD a = input1[i-j];
                    BDD b = input2[i-j];
                    temp_carry = carry_generate(a, b, temp_carry);
                }
            }
            carry_outs.push_back(temp_carry);
        }

        std::vector<BDD> sum;
        sum.reserve(num_bits + 1);
        for (unsigned int i = 0;i<num_bits;i++) {
            BDD a = input1[i];
            BDD b = input2[i];
            if (i == 0) {
                sum.push_back(a ^ (b ^ mgr.bddZero()));
            } else {
                sum.push_back(a ^ (b ^ carry_outs[i-1]));
            }
        }

        // final carry out
        sum.push_back(carry_outs.back());

        return sum;
    }

    std::vector<BDD> regular_adder(const Cudd &mgr, unsigned int num_bits) {

        std::vector<BDD> input1, input2;
        input1.reserve(num_bits);
        input2.reserve(num_bits);
        for (unsigned int i = 0;i<num_bits;i++) {
            input1.push_back(mgr.bddVar(i));
            input2.push_back(mgr.bddVar(i + num_bits));
        }

        std::vector<BDD> sum;
        sum.reserve(num_bits + 1);

        BDD carry = mgr.bddZero();
        for (size_t i = 0; i < num_bits; ++i) {
            auto tmp  = abo::util::full_adder(input1[i], input2[i], carry);
            sum.push_back(tmp.first);
            carry = tmp.second;

        }
        sum.push_back(carry);
        return sum;
    }

}
