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

    /**
     * @brief Dump the given BDD forest as text that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdds The functions to show in the dot file
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param output The stream to write the result to
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                           std::ostream &output, DotPresentationConfig conf = {});

    /**
     * @brief Dump the given BDD forest to a file that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdds The functions to show in the dot file
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param filename The name of the file to output the result to. Must be writable
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                                   std::string filename, DotPresentationConfig conf = {});

}

#endif // DUMP_DOT_H
