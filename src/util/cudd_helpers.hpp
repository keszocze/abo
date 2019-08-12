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


    std::vector<BDD> abs(const Cudd& mgr, const std::vector<BDD>& f);

}
#endif //ABO_CUDD_HELPERS_HPP

