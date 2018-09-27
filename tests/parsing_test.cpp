//
// Created by keszocze on 21.09.18.
//

#include "../src/parsing/pla_parser.hpp"
#include "../src/util/string_helpers.hpp"
#include "../src/util/cudd_helpers.hpp"


using namespace abo;
using namespace abo::util;





int main() {

    Cudd mgr(2);

    std::vector<BDD> outputs;

    std::vector<std::string> innames;
    std::vector<std::string> outnames;
    std::vector<BDD> out;

    abo::parser::pla_parser test_parser(mgr,out, innames,outnames);

    lorina::read_pla("andor.pla", test_parser);

    std::cout << "read BDD with " << out.size() << " outputs\n";




    return 0;
}