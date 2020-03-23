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
     * @brief count_minterms Counts the number of minterms for each node in the given BDD in percent (between 0 and 1).
     * This counts how many percent of inputs that lead to a given node as an intermediate result also lead to 1.
     * @param bdd The BDD for which the minters are counted
     * @return A map containing every node in the bdd and its minterm count
     */
    std::map<DdNode*, double> count_minterms(const BDD &bdd);

    /**
     * @brief count_solutions For each node reachable from the given root, count the number of solutions
     * @param bdd The root node to search from
     * @return A map containing the number of solutions for each reachable node
     */
    std::map<DdNode*, double> count_solutions(const BDD &bdd);

    /**
     * @brief random_satisfying_input Computes a random variable assignment that satisfies the function given as a bdd
     * @param bdd The function that has to be satisfied by the assignment
     * @param minterm_count The minterm count for each node of the bdd, this must be computed by count_minterms called on the bdd
     * @param max_level The maximum variable level that should be present in the variable assignment
     * @return A random variable assignment satisfying bdd. The vector consists of zeros and ones, integers are only used for better compatibility with cudd functions
     */
    std::vector<int> random_satisfying_input(const BDD &bdd, const std::map<DdNode*, double> &minterm_count, int max_level);

    //! Returns the value of the given ADD node if it is a constant node and zero otherwise
    unsigned int const_ADD_value(const ADD &add);

    /**
     * @brief add_terminal_values Finds all terminal values present in the given ADD and how often each one is reached
     * @param add The function to compute the terminal values of
     * @return A list of terminal values and their frequencies. The first value of the pair is the terminal node value (unique in the list),
     * the second value is how often it is reached in the ADD
     */
    std::vector<std::pair<double, unsigned long> > add_terminal_values(const ADD &add);

    /**
     * @brief bdd_forest_to_add Computes the ADD equivalent of a function represented by a BDD forest
     * @param mgr The cudd node manager to create the ADD in
     * @param bdds The function to convert to an ADD
     * @param num_rep The number format in which the bdds represent the function
     * @return The given function as an ADD
     */
    ADD bdd_forest_to_add(const Cudd &mgr, const std::vector<BDD> &bdds,
                          const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

    /**
     * @brief xor_difference_add Computes the bit-wise XOR between the two given functions and converts the result to an ADD
     * The conversion creates unsigned integer values
     * @param mgr The cudd node manager to create the ADD in
     * @param f The first function to compute the difference of
     * @param f_hat The second function. Must have the same length as f
     * @return The XOR difference as an ADD
     */
    ADD xor_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    /**
     * @brief absolute_difference_add Computes the absolute difference between to functions represented as BDDs and returns the result as an ADD
     * @param mgr The cudd node manager to create the ADD in
     * @param f The first function to compute the difference of
     * @param f_hat The second function. Must have the same length as f
     * @param num_rep The number representation used for the interpretation of the functions f and f_hat
     * @return A function computing the absolute difference between f and f_hat for each input represented by an ADD
     */
    ADD absolute_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                const NumberRepresentation num_rep);

    /**
     * @brief dump_dot A helper function for calling the dump dot method of cudd for a BDD forest
     * @param mgr The cudd node manager to use
     * @param bdd The function to dump as a dot file string
     * @param innames The names of the input variables to show in the dot file. May be left empty
     * @param outnames The names for each bit of the function to show in the dot file. May be left empty
     */
    void dump_dot(
            const Cudd &mgr,
            const std::vector<BDD>& bdd,
            const std::vector <std::string> &innames=std::vector <std::string>(),
            const std::vector <std::string> &outnames=std::vector <std::string>());

    /**
     * @brief dump_dot A helper function for calling the dump dot method of cudd for a single BDD
     * @param mgr The cudd node manager to use
     * @param bdd The function to dump as a dot file string
     * @param inames The names of the input variables to show in the dot file. May be left empty
     * @param funname The name of the function to show in the dot file output
     */
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
     * @brief Creates BDD representing abs(f). For unsigned numbers, the original function will be returned
     * @param mgr The Cudd object manager
     * @param f Function in Two's Complement
     * @param num_rep The number representation for f
     * @return BDD representing abs(f)
     */
    std::vector<BDD> abs(const Cudd& mgr, const std::vector<BDD>& f, const NumberRepresentation num_rep);

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
