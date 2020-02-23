//
// Created by keszocze on 27.09.18.
//

#pragma once

#include <vector>
#include <string>
#include <map>

#include <cudd/cplusplus/cuddObj.hh>

#include "number_representation.hpp"

namespace abo::util {

    /**
     * @brief Computes the label/number of the lowest terminal level of a given BDD (forest)
     * @param bdds BDDs in the forest
     * @return Lowest level in the forest
     */
    unsigned int terminal_level(const std::vector<std::vector<BDD>>& bdds);

    /**
     * @brief Evaluates the sum of a and b for an adder given as a BDD forest
     * @param adder BDD forest representing an adder
     * @param a The first summand
     * @param b The second summand
     * @param bits bit width of the adder
     * @return The value of a + b
     */
    long eval_adder(const std::vector<BDD> &adder, long a, long b, int bits);

    /**
     * @TODO describe what this does
     * @param bdd The BDD for which the minters are counted
     * @return
     */
    std::map<DdNode*, double> count_minterms(const BDD &bdd);

    /**
     * @brief count_solutions For each node reachable from the given root, count the number of solutions
     * @param bdd The root node to search from
     * @return A map containing the number of solutions for each reachable node
     */
    std::map<DdNode*, double> count_solutions(const BDD &bdd);

    std::vector<int> random_satisfying_input(const BDD &bdd, const std::map<DdNode*, double> &minterm_count, int max_level);

    // return the value of the given add node, it must be a constant
    unsigned int const_ADD_value(const ADD &add);

    // returns a list of values (first) and how often they are reached (second)
    std::vector<std::pair<double, unsigned long> > add_terminal_values(const ADD &add);

    // assumes that the function represents an unsigned integer
    ADD bdd_forest_to_add(const Cudd &mgr, const std::vector<BDD> &bdds,
                          const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

    ADD xor_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);


    ADD absolute_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                const NumberRepresentation num_rep);

    void dump_dot(
            const Cudd &mgr,
            const std::vector<BDD>& bdd,
            const std::vector <std::string> &innames=std::vector <std::string>(),
            const std::vector <std::string> &outnames=std::vector <std::string>());

    void dump_dot(
            const Cudd &mgr,
            const BDD& bdd,
            const std::vector<std::string>& inames=std::vector <std::string>(),
            const std::string& funname=std::string());


    /**
     * @brief Retrives high-child of a BDD v
     * @param mgr The Cudd object manager
     * @param v
     * @return low(v)
     */
    BDD high(const Cudd& mgr, const BDD& v);

    /**
     * @brief Retrives low-child of a BDD v
     * @param mgr The Cudd object manager
     * @param v
     * @return low(v)
     */
    BDD low(const Cudd& mgr, const BDD& v);


    /**
     * @brief Creates a full adder of two BDDs and a carry BDD
     * @param f First summand BDD
     * @param g Second summand BDD
     * @param carry_in Carry BDD
     * @return Tuple of BDDs: Summ BDD and Carry BDD
     */
    std::pair<BDD,BDD> full_adder(const BDD& f, const BDD& g, const BDD &carry_in);


    /**
     * @brief Creates a (vector of) BDDs that represent the difference between to given (vectors of) BDDs
     *
     * The supplied BDDs are supposed to compute values represented in Two's Copmlement. The BDD returned by this function
     * computes the difference between the outputs of these BDDs, again represented in Two's Complement.
     *
     * @param mgr The Cudd object manager
     * @param minuend Minuend in Two's Complement
     * @param subtrahend  Subtrahend in Two's Complement
     * @return BDD computing the difference of the outputs of the two supplied BDDs
     */
    std::vector<BDD> bdd_subtract(const Cudd &mgr, const std::vector<BDD> &minuend, const std::vector<BDD> &subtrahend);

    /**
     * @brief Creates a (vector of) BDDs that represent the absolute difference between to given (vectors of) BDDs
     *
     * The supplied BDDs are supposed to compute values represented in Two's Copmlement. The BDD returned by this function
     * computes the difference between the outputs of these BDDs. It will always return an unsigned number, not a number in Two's complement.
     *
     * @param mgr The Cudd object manager
     * @param f Function in Two's Complement
     * @param g Funtion in Two's Complement
     * @param num_rep The number representation for f and g
     * @return BDD computing the absolute difference of the outputs of the two supplied BDDs
     */
    std::vector<BDD> bdd_absolute_difference(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g,
                                             const NumberRepresentation num_rep);


    /**
     * @brief Creates the BDD representing the sum of two functions
     * @param mgr The Cudd object manager
     * @param f First summand
     * @param g Second summand
     * @return BDD representing f+g
     */
    std::vector<BDD> bdd_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g);

    /**
     * @brief Creates BDD representing abs(f)
     * @param mgr The Cudd object manager
     * @param f Function in Two's Complement
     * @return BDD representing abs(f)
     */
    std::vector<BDD> abs(const Cudd& mgr, const std::vector<BDD>& f);

    /**
     * @brief Shifts a function f by a number of bits left or right
     *
     * The shifted result has the same number of bits as the input, bits that are shifted above or below
     * the original function bit range will be discarded
     *
     * @param mgr The Cudd object manager
     * @param f Function to shift
     * @param bits_to_shift Number of bits to shift. Can be both positive or negative
     * @return Shifted BDD
     */
    std::vector<BDD> bdd_shift(const Cudd &mgr, const std::vector<BDD> &f, int bits_to_shift);

    /**
     * @brief Multiplies a bdd function with a constant
     * @param mgr The Cudd object manager
     * @param f Function that is to be multiplied. The function is interpreted as returning an unsigned integer
     * @param factor Constant multiplication factor
     * @param num_extra_bits Additional bits used to make sure that the result can be stored (must be less than sizeof(unsigned long) * 8)
     * @return BDD representing the function "factor*f" (extended by up to num_extra_bits)
     */
    std::vector<BDD> bdd_multiply_constant(const Cudd &mgr, const std::vector<BDD> &f, double factor, const unsigned int num_extra_bits = 16);

    /**
     * @brief Checks if an input x exists such that int(f1(x)) >= int(f2(x))
     * @param mgr The Cudd object manager
     * @param f1 Function to compare in base two (interpreted as returning an unsigned integer)
     * @param f2 Function to compare in base two (interpreted as returning an unsigned integer)
     * @return {ge, e}
     * ge is true iff there exists an x such that int(f1(x)) >= int(f2(x))
     * e is true iff there exists an x such that int(f1(x)) = int(f2(x)), but no x exists with int(f1(x)) > int(f2(x))
     */
    std::pair<bool, bool> exists_greater_equals(const Cudd &mgr, const std::vector<BDD> &f1, const std::vector<BDD> &f2);


    /**
     * @brief Creates a BDD representing f >= g
     * @param mgr The Cudd object manager
     * @param f Function to compare in base two (interpreted as returning an unsigned integer)
     * @param g Function to compare in base two (interpreted as returning an unsigned integer)
     * @return BDD representing f >= g
     */
    BDD greater_equals(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g);

    /**
     * @brief Computes a function g such that int(g(x)) = max(1, int(f(x)) for an unsigned function f
     * @param mgr The Cudd object manager
     * @param f The function to use. Is interpreted as unsigned integers
     * @return Function g as vector of BDDS of same length as f
     */
    std::vector<BDD> bdd_max_one(const Cudd &mgr, const std::vector<BDD> &f);

    /**
     * @brief divide Computes a function h such that int(h(x)) = int(f(x)) / int(g(x))
     * @param mgr The Cudd object manager
     * @param f int(f(x)) = 0 must not be possible, f is interpreted as an unsigned integer
     * @param g is interpreted as an unsigned integer
     * @param extra_bits
     * @return
     */
    std::vector<BDD> bdd_divide(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g, unsigned int extra_bits);

}
