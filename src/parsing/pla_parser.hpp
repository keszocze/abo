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


        virtual ~pla_parser() = default;

        void on_number_of_inputs(std::size_t number_of_inputs) const override;

        void on_number_of_outputs(std::size_t number_of_outputs) const override;

        void on_number_of_terms(std::size_t number_of_terms) const override;

        bool on_keyword(const std::string &keyword, const std::string &value) const override;

        /**
         * @brief Method called on cube lines of a pla file.
         *
         * @param term The currently evaluated cube
         * @param out The functions, the cube belongs to
         *
         * Characters in the cube other than '1' and '0' are treated as don't care values.
         *
         * Characters in the list of functions other than '1' are ignored.
         *
         * @note This method performs no range checks whatsoever. This means that syntactically incorrect pla files will
         * result in undefined behavior or crashes.
         */
        void on_term(const std::string &term, const std::string &out) const override;
    };

}
#endif //ABO_PLA_PARSER_HPP
