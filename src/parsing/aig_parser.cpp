//
// Created by keszocze on 21.09.18.
//

#include "aig_parser.hpp"

#include <cassert>

namespace abo::parser {

void aig_parser::on_header(std::size_t m, std::size_t i,
                           std::size_t, std::size_t o,
                           std::size_t a) const
{
    num_variables = m;
    num_literals = m * 2 + 2;
    literals.resize(num_literals);
    literals[0] = mgr.bddZero();
    literals[1] = mgr.bddOne();

    num_inputs = i;
    input_names.resize(num_inputs);
    num_outputs = o;
    output_indices.resize(num_outputs);
    output_names.resize(num_outputs);
    num_gates = a;
}

void aig_parser::on_input(unsigned index, unsigned lit) const
{
    assert(index < num_inputs);
    assert(lit < num_literals);

    literals[lit] = mgr.bddVar(static_cast<int>(index));
}

void aig_parser::on_output(unsigned index, unsigned lit) const
{
    assert(index < num_outputs);
    assert(lit < num_literals);

    output_indices[index] = lit;
}

void aig_parser::on_and(unsigned index, unsigned left_lit,
                        unsigned right_lit) const
{
    assert(index * 2 <= num_literals);
    assert(left_lit < num_literals && right_lit < num_literals);
    // duplicate assignment of the same variable makes no sense
    assert(!literals[index * 2]);

    const BDD& left = find_literal(left_lit);
    const BDD& right = find_literal(right_lit);
    literals[index * 2] = left & right;
}

void aig_parser::on_input_name(unsigned index, const std::string& name) const
{
    assert(index <= num_inputs);

    input_names[index] = name;
}

void aig_parser::on_output_name(unsigned index, const std::string& name) const
{
    assert(index <= num_outputs);

    output_names[index] = name;
}

const std::vector<BDD> aig_parser::get_outputs() const
{
    std::vector<BDD> result;

    for (const unsigned output : output_indices)
    {
        result.push_back(find_literal(output));
    }

    return result;
}

const BDD& aig_parser::find_literal(unsigned literal) const
{
    if (literals[literal])
    {
        return literals[literal].value();
    }

    // inverted literal
    if ((literal & 1) && literals[literal - 1])
    {
        const BDD& original = literals[literal - 1].value();
        literals[literal] = !original;
        return literals[literal].value();
    }

    // literal is not known yet, they should be created in order
    throw std::invalid_argument("AIG parser: literal should have been declared before");
}

// unsupported operations
void aig_parser::on_latch(unsigned, unsigned, latch_init_value) const
{
    assert(false);
}

void aig_parser::on_bad_state(unsigned, unsigned) const
{
    assert(false);
}

void aig_parser::on_constraint(unsigned, unsigned) const
{
    assert(false);
}

void aig_parser::on_fairness(unsigned, unsigned) const
{
    assert(false);
}

void aig_parser::on_justice(unsigned, const std::vector<unsigned>&) const
{
    assert(false);
}
} // namespace abo::parser
