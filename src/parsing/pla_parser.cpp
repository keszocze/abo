//
// Created by keszocze on 21.09.18.
//

#include "pla_parser.hpp"

#include "../util/string_helpers.hpp"



namespace abo::parser {

    void pla_parser::on_number_of_inputs(std::size_t number_of_inputs) const {

        for (std::size_t i = 0; i < number_of_inputs; i++) {
            bdd_vars.push_back(mgr.bddVar(i));
        }
    }

    void pla_parser::on_number_of_outputs(std::size_t number_of_outputs) const {

        for (std::size_t i = 0; i < number_of_outputs; i++) {
            output_bdds.push_back(mgr.bddZero());
        }
    }

    /**
     * @brief Empty function doing nothing
     * @param number_of_terms  The number of terms specified in the .pla file
     *
     * As the parsing can not really make use of this information, it is completely ignored.
     */
    void pla_parser::on_number_of_terms(std::size_t number_of_terms) const {
        (void) number_of_terms; // this cast prevents a compile time warning [-Wunused-variable] and supposedly generates no code
    }

    bool pla_parser::on_keyword(const std::string &keyword, const std::string &value) const {
        if (keyword == "ilb") {
            abo::util::split(value, innames);
            for (std::string n: innames) {
                mgr.pushVariableName(n);
            }
            return true;
        }
        if (keyword == "ob") {
            abo::util::split(value, outnames);

            return true;
        }

        std::cout << "unrecognized keyword \"" << keyword << "\" (" << value << ")\n";
        return false;

    }

    void pla_parser::on_term(const std::string &term, const std::string &out) const {
        // term ist der cube, den man einlesen muss

        BDD cube = mgr.bddOne();

        // we should probably perform index checks here...
        for (std::size_t i = 0; i < term.size(); i++) {
            switch (term[i]) {
                case '0':
                    cube = cube * !bdd_vars[i];
                    break;
                case '1':
                    cube = cube * bdd_vars[i];
                    break;
                default: // TODO find files with don't cares and extend accordingly
                    break;
            }

        }

        for (std::size_t i = 0; i < out.size(); i++) {
            if (out[i] == '1') {
                output_bdds[i] = output_bdds[i] + cube;
            }
        }

    }
}