//
// Created by keszocze on 21.09.18.
//

#ifndef ABO_PLA_PARSER_HPP
#define ABO_PLA_PARSER_HPP

#include <lorina/pla.hpp>
#include "../../lib/cudd/cplusplus/cuddObj.hh"

namespace abo::parser {

    class pla_parser : public lorina::pla_reader {
    private:

        // TODO im Endeffekt natürlich den Krams per call parameter rausreichen
        Cudd& mgr;
        mutable std::vector<BDD> bdd_vars;
        mutable std::vector<BDD> output_bdds;

        mutable std::size_t n_in, n_out;

        mutable std::vector<std::string> inputs;
        mutable std::vector<std::string> outputs;

    public:

        pla_parser(Cudd& mgr): mgr(mgr) {}

        void on_number_of_inputs(std::size_t number_of_inputs) const override;

        void on_number_of_outputs(std::size_t number_of_outputs) const override;

        void on_number_of_terms(std::size_t number_of_terms) const override;

        bool on_keyword(const std::string &keyword, const std::string &value) const override;

        void on_end() const override;

        void on_term(const std::string &term, const std::string &out) const override;
    };

}
#endif //ABO_PLA_PARSER_HPP
