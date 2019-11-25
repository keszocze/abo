#ifndef DUMP_DOT_H
#define DUMP_DOT_H

#include <cudd/cplusplus/cuddObj.hh>

#include <string>

namespace abo::util {

    void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names, std::ostream &output);

    void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names, std::string filename);

}

#endif // DUMP_DOT_H
