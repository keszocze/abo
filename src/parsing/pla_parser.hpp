//
// Created by keszocze on 21.09.18.
//

#pragma once

#include <lorina/pla.hpp>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::parser {

    /**
     * @brief Class for parsing PLA files to BDD forests
     * The actual parsing is done by lorina, this class only converts the result to BDDs.
     * The usage can be for example as follows:
     *
     * abo::parser::pla_parser parser(mgr, output_function, argument_names, output_names);
     * lorina::read_pla(filename, parser);
     */
    class pla_parser : public lorina::pla_reader {
    private:

        const Cudd &mgr;
        mutable std::vector<BDD> bdd_vars;
        std::vector<BDD> &output_bdds;

        std::vector<std::string> &innames;
        std::vector<std::string> &outnames;

    public:

        /**
         * @brief pla_parser
         * @param mgr The Cudd object manager to create the BDDs in
         * @param out The vector to save the resulting function in
         * @param innames The vector to save the functions input names in
         * @param outnames The vector to save the function output bit names in
         */
        pla_parser( Cudd &mgr, std::vector<BDD> &out,
                    std::vector<std::string> &innames,
                    std::vector<std::string> &outnames) :
                        mgr(mgr), output_bdds(out), innames(innames), outnames(outnames) {}


        virtual ~pla_parser() = default;

    private:
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
