#ifndef DUMP_DOT_H
#define DUMP_DOT_H

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::util {

    void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds, std::ostream &output);

    void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds, std::string filename);

}

#endif // DUMP_DOT_H
