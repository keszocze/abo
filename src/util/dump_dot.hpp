#ifndef DUMP_DOT_H
#define DUMP_DOT_H

#include <cudd/cplusplus/cuddObj.hh>

#include <string>

namespace abo::util {

    struct DotPresentationConfig {
        std::string node_shape = "circle";
        float node_width = 0.35f;
        std::string terminal_shape = "square";
        float terminal_width = 0.35f;
        std::string function_name_shape = "none";
        float function_name_width = 0.2f;
        float node_seperation = 0.2f;
        float rank_seperation = 0.25f;
    };

    void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                           std::ostream &output, DotPresentationConfig conf = {});

    void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                                   std::string filename, DotPresentationConfig conf = {});

}

#endif // DUMP_DOT_H
