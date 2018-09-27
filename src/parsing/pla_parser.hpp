//
// Created by keszocze on 21.09.18.
//

#ifndef ABO_PLA_PARSER_HPP
#define ABO_PLA_PARSER_HPP

#include <lorina/pla.hpp>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::parser {

    class pla_parser : public lorina::pla_reader {
    private:

        // TODO im Endeffekt natürlich den Krams per call parameter rausreichen
        const Cudd &mgr;
        mutable std::vector<BDD> bdd_vars;
        std::vector<BDD> &output_bdds;

        std::vector<std::string> &innames;
        std::vector<std::string> &outnames;

    public:

        pla_parser( Cudd &mgr, std::vector<BDD> &out,
                    std::vector<std::string> &innames,
                    std::vector<std::string> &outnames) :
                        mgr(mgr), output_bdds(out), innames(innames), outnames(outnames) {}

        void on_number_of_inputs(std::size_t number_of_inputs) const override;

        void on_number_of_outputs(std::size_t number_of_outputs) const override;

        void on_number_of_terms(std::size_t number_of_terms) const override;

        bool on_keyword(const std::string &keyword, const std::string &value) const override;

        void on_end() const override;

        void on_term(const std::string &term, const std::string &out) const override;
    };

}
#endif //ABO_PLA_PARSER_HPP
