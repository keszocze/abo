//
// Created by keszocze on 27.09.18.
//

#ifndef ABO_CUDD_HELPERS_HPP
#define ABO_CUDD_HELPERS_HPP

#include <vector>
#include <string>
#include <map>


#include <cudd/cplusplus/cuddObj.hh>

namespace abo::util {

    unsigned int terminal_level(const std::vector<std::vector<BDD>>& bdds);

    long eval_adder(const std::vector<BDD> &adder, long input1, long input2, int bits);

    std::map<DdNode*, double> count_minterms(const BDD &bdd);

    std::vector<int> random_satisfying_input(const BDD &bdd, const std::map<DdNode*, double> &minterm_count, int max_level);

    // return the value of the given add node, it must be a constant
    unsigned int const_add_value(const ADD &add);
    // returns a list of values (first) and how often they are reached (second)
    std::vector<std::pair<double, unsigned long> > add_terminal_values(const ADD &add);

    // assumes that the function represents an unsigned integer
    ADD bdd_forest_to_add(const Cudd &mgr, const std::vector<BDD> &bdds);

    ADD xor_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);
    ADD absolute_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    void dump_dot(
            const Cudd &mgr,
            const std::vector<BDD>& bdd,
            const std::vector <std::string> &innames={},
            const std::vector <std::string> &outnames={});

    void dump_dot(
            const Cudd &mgr,
            const BDD& bdd,
            const std::vector<std::string>& inames={},
            const std::string& funname=std::string());

    BDD high(const Cudd& mgr, const BDD& v);
    BDD low(const Cudd& mgr, const BDD& v);


    std::pair<BDD,BDD> full_adder(const BDD& f, const BDD& g, const BDD &carry_in);


    /**
     * @brief Creates a (vector of) BDDs that represent the difference between to given (vectors of) BDDs
     *
     * The supplied BDDs are supposed to compute values represented in Two's Copmlement. The BDD returned by this function
     * computes the difference between the outputs of these BDDs, again represented in Two's Complement.
     *
     * @param minuend Minuend in Two's Complement
     * @param subtrahend  Subtrahend in Two's Complement
     * @return BDD computing the difference of the outputs of the two suupplied BDDs
     */
    std::vector<BDD> bdd_subtract(const Cudd &mgr, const std::vector<BDD> &minuend, const std::vector<BDD> &subtrahend);

    std::vector<BDD> bdd_add(const Cudd &mgr, const std::vector<BDD> &f1, const std::vector<BDD> &f2);


    std::vector<BDD> abs(const Cudd& mgr, const std::vector<BDD>& f);

    /**
     * @brief bdd_shift shifts a function f by a number of bits left or right
     *
     * The shifted result has the same number of bits as the input, bits that are shifted above or below
     * the original function bit range will be discarded
     *
     * @param f function to shift
     * @param bits_to_shift number of bits to shift. Can be both positive or negative
     * @return
     */
    std::vector<BDD> bdd_shift(const Cudd &mgr, const std::vector<BDD> &f, int bits_to_shift);

    /**
     * @brief bdd_multiply_constant Multiplies a bdd function with a constant
     * @param f
     * @param factor
     * @return
     */
    std::vector<BDD> bdd_multiply_constant(const Cudd &mgr, const std::vector<BDD> &f, double factor, unsigned int num_extra_bits = 16);

    /**
     * @brief exists_greater_equals Checks if an input x exists such that int(f1(x)) >= int(f2(x))
     * @param f1
     * @param f2
     * @return
     */
    bool exists_greater_equals(const Cudd &mgr, const std::vector<BDD> &f1, const std::vector<BDD> &f2);

    BDD greater_equals(const Cudd &mgr, const std::vector<BDD> &f1, const std::vector<BDD> &f2);

    /**
     * @brief max_one Computes a function g such that int(g(x)) = max(1, int(f(x)) for an unsigned function f
     * @return g (same length as f)
     */
    std::vector<BDD> bdd_max_one(const Cudd &mgr, const std::vector<BDD> &f);

    /**
     * @brief divide Computes a function h such that int(h(x)) = int(f(x)) / int(g(x))
     * @param f int(f(x)) = 0 must not be possible, f is interpreted as an unsigned integer
     * @param g is interpreted as an unsigned integer
     * @param extra_bits
     * @return
     */
    std::vector<BDD> bdd_divide(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g, unsigned int extra_bits);

}
#endif //ABO_CUDD_HELPERS_HPP

