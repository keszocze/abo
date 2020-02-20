//
// Created by keszocze on 21.09.18.
//

#pragma once

#include <lorina/aiger.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <vector>
#include <optional>

namespace abo::parser {

    /**
     * @brief Class for parsing AIG files to BDD forests
     * The actual parsing is done by lorina, this class only converts the result to BDDs.
     * The usage can be for example as follows:
     *
     * abo::parser::aig_parser parser(mgr);
     * lorina::read_aiger(filename, parser);
     * std::vector<BDD> function = parser.get_outputs();
     */
    class aig_parser : public lorina::aiger_reader {
    private:
        const Cudd &mgr;

        mutable std::size_t num_variables = 0;
        mutable std::size_t num_literals = 0;
        mutable std::size_t num_inputs = 0;
        mutable std::size_t num_outputs = 0;
        mutable std::size_t num_gates = 0;

        mutable std::vector<unsigned> output_indices;
        mutable std::vector<std::string> input_names;
        mutable std::vector<std::string> output_names;
        mutable std::vector<std::optional<BDD>> literals;

    public:

        //! Returns the parsed function from the input file
        const std::vector<BDD> get_outputs() const;

        //! Returns the names of the input variables
        const std::vector<std::string> & get_input_names() const { return input_names; }

        //! Returns the names of the individual function bits
        const std::vector<std::string> & get_output_names() const { return output_names; }

        /**
         * @brief aig_parser Constructor for the AIG parser
         * @param mgr The Cudd object manager to create the BDDs in
         */
        explicit aig_parser( Cudd &mgr ) :
                        mgr(mgr) {}

        virtual ~aig_parser() = default;

    private:

        virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a ) const override;

        virtual void on_input( unsigned index, unsigned lit ) const override;

        virtual void on_output( unsigned index, unsigned lit ) const override;

        virtual void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const override;

        virtual void on_input_name( unsigned index, const std::string& name ) const override;

        virtual void on_output_name( unsigned index, const std::string& name ) const override;

        // unsupported operations
        virtual void on_latch( unsigned index, unsigned next, latch_init_value reset ) const override;

        virtual void on_bad_state( unsigned index, unsigned lit ) const override;

        virtual void on_constraint( unsigned index, unsigned lit ) const override;

        virtual void on_fairness( unsigned index, unsigned lit ) const override;

        virtual void on_justice( unsigned index, const std::vector<unsigned>& lits ) const override;

        const BDD & find_literal( unsigned literal ) const;

    };

}
