//
// Created by keszocze on 21.09.18.
//

#include "../src/parsing/pla_parser.hpp"


int main() {

    Cudd mgr(2);

    abo::parser::pla_parser test_parser(mgr);

    lorina::read_pla("/home/keszocze/fau/projekte/approx/abo/tests/andor.pla", test_parser);

    return 0;
}