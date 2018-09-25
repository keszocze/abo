//
// Created by keszocze on 21.09.18.
//

#include "pla_parser.hpp"

namespace abo::parser {

    void pla_parser::on_number_of_inputs(std::size_t number_of_inputs) const {
        inputs.reserve(number_of_inputs);
        std::cout << "found " << number_of_inputs << " inputs\n";

        n_in = number_of_inputs;

        for (std::size_t i = 0; i < number_of_inputs; i++) {
            bdd_vars.push_back(mgr.bddVar());
        }
    }

    void pla_parser::on_number_of_outputs(std::size_t number_of_outputs) const {
        outputs.reserve(number_of_outputs);
        std::cout << "found " << number_of_outputs << " outputs\n";

        // alle Ausgaben können erstmal sicher auf "false" gesetzt werden. So lange kein Cube sie "trifft" ist ja alles gut
        // TODO benötige ich diese Variablen? (vermutlich nicht)
        n_out = number_of_outputs;

        for (std::size_t i = 0; i < number_of_outputs; i++) {
            output_bdds.push_back(mgr.bddZero());
        }
    }

    void pla_parser::on_number_of_terms(std::size_t number_of_terms) const {
        pla_reader::on_number_of_terms(number_of_terms);
    }

    bool pla_parser::on_keyword(const std::string &keyword, const std::string &value) const {

        // TODO namen auch tatsächlich auslesen
        if (keyword == "ilb") {
//            std::cout << "found inputs: " << value << "\n";
            return true;
        }

        if (keyword == "ob") {
//            std::cout << "found outputs: " << value << "\n";
            return true;
        }
        std::cout << "unrecognized keyword \"" << keyword << "\" (" << value << ")\n";

        return false;
    }

    void pla_parser::on_end() const {

        const char* out_names[] = {"or", "and"};
        const char* in_names[] = {"a", "b"};
        mgr.DumpDot(output_bdds, nullptr, out_names);
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