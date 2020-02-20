//
// Created by keszocze on 21.09.18.
//

#pragma once

#include <lorina/aiger.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <vector>
#include <optional>

namespace abo::parser {

    class aig_parser : public lorina::aiger_reader {
    private:
        const Cudd &mgr;

        // TODO: muss das noch entfernt werden?
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

        const std::vector<BDD> get_outputs() const;

        const std::vector<std::string> & get_input_names() const { return input_names; }

        const std::vector<std::string> & get_output_names() const { return output_names; }

        explicit aig_parser( Cudd &mgr ) :
                        mgr(mgr) {}

        virtual ~aig_parser() = default;

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

    private:

        const BDD & find_literal( unsigned literal ) const;

    };

}
