//
// Created by keszocze on 27.09.18.
//

#ifndef ABO_CUDD_HELPERS_HPP
#define ABO_CUDD_HELPERS_HPP

#include <vector>
#include <string>


#include <cudd/cplusplus/cuddObj.hh>

namespace abo::util {
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
}
#endif //ABO_CUDD_HELPERS_HPP
