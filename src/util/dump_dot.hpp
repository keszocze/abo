#ifndef DUMP_DOT_H
#define DUMP_DOT_H

#include <cudd/cplusplus/cuddObj.hh>

#include <string>
#include <optional>

namespace abo::util {


    struct DotPresentationConfig {
        //! The shape of an inner BDD node (i.e. the ones for the variables)
        std::string node_shape = "circle";
        float node_width = 0.35f;

        //! The shape of a terminal node (i.e. the ones for true or false)
        std::string terminal_shape = "square";
        float terminal_width = 0.35f;

        //! The shape of the nodes for function names
        std::string function_name_shape = "none";
        float function_name_width = 0.2f;
        float node_seperation = 0.2f;
        float rank_seperation = 0.25f;
    };

    /**
     * @brief Dump the given BDD forest as text that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdds The functions to show in the dot representation
     * @param output The stream to write the result to
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param variable_names The names of the variables used in the BDD. It will be default populated to x_i
     * for the i'th variable if an empty vector is given. Must be either empty or containt names for all used variables
     * @param enforce_function_order Whether or not to enforce the order of function from left to right in which they occur in the input
     * @param left_terminal Whether or not and how the terminal nodes should be arranged. If the optional is empty,
     * no ordering on the terminal nodes is enforced and dot will select one. If it is set, the boolean specifies which terminal
     * the leftmost one should be
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable(const Cudd &mgr,
                           const std::vector<BDD> &bdds,
                           std::ostream &output,
                           const std::vector<std::string> &function_names = std::vector<std::string>(),
                           const std::vector<std::string> &variable_names = std::vector<std::string>(),
                           const bool enforce_function_order = true,
                           const std::optional<bool> left_terminal = std::optional<bool>(),
                           DotPresentationConfig conf = DotPresentationConfig());

    /**
     * @brief Dump the given BDD forest as text that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdd The function to show in the dot representation
     * @param output The stream to write the result to
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param variable_names The names of the variables used in the BDD. It will be default populated to x_i
     * for the i'th variable if an empty vector is given. Must be either empty or containt names for all used variables
     * @param left_terminal Whether or not and how the terminal nodes should be arranged. If the optional is empty,
     * no ordering on the terminal nodes is enforced and dot will select one. If it is set, the boolean specifies which terminal
     * the leftmost one should be
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable(const Cudd &mgr,
                           const BDD &bdd,
                           std::ostream &output,
                           const std::vector<std::string> &function_names = std::vector<std::string>(),
                           const std::vector<std::string> &variable_names = std::vector<std::string>(),
                           const std::optional<bool> left_terminal = std::optional<bool>(),
                           DotPresentationConfig conf = DotPresentationConfig());

    /**
     * @brief Dump the given BDD forest to a file that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdds The functions to print to the dot file
     * @param filename The name of the file to output the result to. Must be writable
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param variable_names The names of the variables used in the BDD. It will be default populated to x_i
     * for the i'th variable if an empty vector is given. Must be either empty or containt names for all used variables
     * @param enforce_function_order Whether or not to enforce the order of function from left to right in which they occur in the input
     * @param left_terminal Whether or not and how the terminal nodes should be arranged. If the optional is empty,
     * no ordering on the terminal nodes is enforced and dot will select one. If it is set, the boolean specifies which terminal
     * the leftmost one should be
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable_to_file(const Cudd &mgr, const
                                   std::vector<BDD> &bdds,
                                   std::string filename,
                                   const std::vector<std::string> &function_names = std::vector<std::string>(),
                                   const std::vector<std::string> &variable_names = std::vector<std::string>(),
                                   const bool enforce_function_order = true,
                                   const std::optional<bool> left_terminal = std::optional<bool>(),
                                   DotPresentationConfig conf = DotPresentationConfig());


    /**
     * @brief Dump the given BDD forest to a file that can be given to dot. The graph created by this function
     * is more readable than the Cudd variant as no inverted edges are used. The corresponding variable is used as a nodes text
     * @param mgr The BDD object manager
     * @param bdds The function to print to the dot file
     * @param filename The name of the file to output the result to. Must be writable
     * @param function_names The names of the functions. Will be default populated to f_i for the i'th function
     * if an empty vector is given. Must always either be exactly the same length as bdds or empty
     * @param variable_names The names of the variables used in the BDD. It will be default populated to x_i
     * for the i'th variable if an empty vector is given. Must be either empty or containt names for all used variables
     * @param left_terminal Whether or not and how the terminal nodes should be arranged. If the optional is empty,
     * no ordering on the terminal nodes is enforced and dot will select one. If it is set, the boolean specifies which terminal
     * the leftmost one should be
     * @param conf The presentation parameters like shapes and distances
     */
    void dump_dot_readable_to_file(const Cudd &mgr,
                                   const BDD &bdd,
                                   std::string filename,
                                   const std::vector<std::string> &function_names = std::vector<std::string>(),
                                   const std::vector<std::string> &variable_names = std::vector<std::string>(),
                                   const std::optional<bool> left_terminal = std::optional<bool>(),
                                   DotPresentationConfig conf = DotPresentationConfig());
}

#endif // DUMP_DOT_H
