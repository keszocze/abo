#include "dump_dot.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace abo::util {

/// Collects child nodes of the BDD and assigns them to different variable
/// levels
void collect_nodes_rec(DdNode *node, std::set<DdNode *> &visited,
                       std::vector<std::vector<DdNode *>> &level_nodes) {
    if (visited.find(node) != visited.end()) {
        return;
    }
    visited.insert(node);
    if (!Cudd_IsConstant(node)) {
        unsigned int level = Cudd_NodeReadIndex(node);
        while (level_nodes.size() <= level) {
            level_nodes.push_back({});
        }
        level_nodes[level].push_back(node);

        DdNode *N = Cudd_Regular(node);
        DdNode *then_node = Cudd_T(N);
        DdNode *else_node = Cudd_E(N);

        then_node = Cudd_NotCond(then_node, Cudd_IsComplement(node));
        else_node = Cudd_NotCond(else_node, Cudd_IsComplement(node));

        collect_nodes_rec(then_node, visited, level_nodes);
        collect_nodes_rec(else_node, visited, level_nodes);
    }
}

void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds,
                       std::ostream &output,
                       const std::vector<std::string> &function_names,
                       DotPresentationConfig conf) {

    assert(bdds.size() == function_names.size() || function_names.empty());

    /**
   * This function creates the UTF subscript numbers for the node names.
   *
   * f + subscript(23) becomes f_{23} (in LaTeX notation)
   */
    std::function<std::string(int)> subscript([](int n){
        std::string s;
        for (char c: std::to_string(n)) {
            s += std::string("&#832") + std::to_string(c - '0') + std::string(";");
        }
        return s;
    });

    /*
     * Populate function names if none are given
     */
    std::vector<std::string> function_names_;
    if (function_names.empty())
    {
        if( bdds.size() > 1) {
            for (std::size_t i = 0; i < bdds.size(); i++) {
                std::string name = "<<I>f";
                name += subscript(i);
                name += "</I>>";
                function_names_.push_back(name);
            }
        }
        if (bdds.size() == 1)
        {
            function_names_.push_back("f");
        }
    } else {
        for (auto function : function_names) {
            function_names_.push_back("<" + function + ">");
        }
    }
    //------------------------------------------------------------------------------------------

    std::vector<std::vector<DdNode *>> level_nodes;
    std::set<DdNode *> visited;
    for (BDD b : bdds) {
        collect_nodes_rec(b.getNode(), visited, level_nodes);
    }

    int indentation = 0;
    std::function<void()> indent([&output, &indentation]() {
        for (int i = 0; i < indentation; i++)
            output << "\t";
    });

    output << "digraph \"DD\"{" << std::endl;


    /*
     * Create graph header
     */
    indentation++;
    indent();output << "center = true;" << std::endl;
    indent();output << "edge [dir = non];" << std::endl;
    indent();output << "nodesep = \"" << conf.node_seperation << "\";" << std::endl;
    indent();output << "ranksep = \"" << conf.rank_seperation << "\";" << std::endl;
    indent();output << "margin=0;" << std::endl;
    //-------------------------------------------------------------------------------



    /*
     * create nodes for function names
     */


    output << std::endl; indent();output << "# create function name nodes" << std::endl;
    indent();output << "{" << std::endl;
    indentation++;
    indent();output << "rank = same;" << std::endl;
    indent();output << "node [shape = " << conf.function_name_shape
                    << ", fixedsize = true, width = " << conf.function_name_width << "];"
                    << std::endl;
    indent();output << "edge [style = invis];" << std::endl;



    for (std::size_t i = 0; i < function_names_.size(); i++) {
        indent();
        output << "\"" << function_names_[i] << "\" [label = " << function_names_[i]
                  << "];" << std::endl;
    }

    // manually arrange function names next to each other in the first layer of the dot file
    // this works as the edge style is "invis"
    if (function_names_.size() > 1)
    {
        output<< std::endl; indent(); output << "# Arrange function name nodes in correct order" << std::endl;
        indent();
        for (std::size_t i = 0; i < function_names_.size(); i++) {
            output << "\"" << function_names_[i] << "\""
                   << (i < function_names_.size() - 1 ? " -> " : "");
        }
        output << std::endl;
    }

    indentation--;

    indent();output << "}" << std::endl;
    //------------------------------------------------------------------------------


    /*
     * link function name nodes to their corresponding BDD node
     */

    output << std::endl; indent(); output << "# link function name nodes to the BDDs" << std::endl;
    indent();output << "{" << std::endl;
    indentation++;

    for (std::size_t i = 0; i < function_names_.size(); i++) {
        indent();
        output << "\"" << function_names_[i] << "\":s -> \"" << bdds[i].getNode()
               << "\":n [arrowhead = none, weight = 10];" << std::endl;
    }
    indentation--;
    indent();output << "}" << std::endl;
    //---------------------------------------------------------

    /*
     * create invisible connections to properly order the nodes
     */
    indent();output << "{" << std::endl;
    indentation++;

    indent();output << "edge [style = invis];" << std::endl;
    if (function_names_.size() > 0) {
        indent();
        output << "\"" << function_names_[0] << "\" -> ";
    }
    indent();
    for (unsigned int i = 0; i < level_nodes.size(); i++) {
        if (level_nodes[i].size() > 0) {
            output << "\"" << level_nodes[i][0] << "\" -> ";
        }
    }
    output << "\"" << mgr.bddOne().getNode() << "\";" << std::endl;

    indentation--;
    indent();output << "}" << std::endl;
    //-------------------------------------------------------



    /*
     * Create internal nodes level by level
     */
    for (unsigned int i = 0; i < level_nodes.size(); i++) {
        if (level_nodes[i].size() > 0) {
            output << std::endl; indent(); output << "# create nodes for level " << i << std::endl;
            indent();output << "{" << std::endl;
            indentation++;
            indent();output << "rank = same;" << std::endl;
            for (DdNode *n : level_nodes[i]) {
                indent();output << "\"" << n << "\" [label = <<I>x";
                output << subscript(i+1);
                output << "</I>>, shape = " << conf.node_shape
                       << ", fixedsize=true, width=" << conf.node_width << "];"
                       << std::endl;
            }
            indentation--;
            indent();output << "}" << std::endl;
        }
    }
    //----------------------------------------------------------------------

    /*
     * Create the terminal nodes
     */
    output << std::endl; indent(); output << "# create terminal nodes" << std::endl;
    indent();output << "{" << std::endl;
    indentation++;
    indent();output << "ranke = same;" << std::endl;
    indent();output << "\"" << mgr.bddOne().getNode()
                    << "\" [label = <&#8868;>, shape = " << conf.terminal_shape
                    << ", width=" << conf.terminal_width << ", fixedsize=true];"
                    << std::endl;
    indent(); output << "\"" << mgr.bddZero().getNode()
                     << "\" [label = <&#8869;>, shape = " << conf.terminal_shape
                     << ", width=" << conf.terminal_width << ", fixedsize=true];"
                     << std::endl;
    indentation--;
    indent();
    output << "}" << std::endl;
    //-----------------------------------------------------------------

    /*
     * Finally add the connections between the nodes
     */
    output << std::endl; indent(); output << "# create inter-node high-/low-child connections" << std::endl;
    for (auto &nodes : level_nodes) {
        for (auto node : nodes) {
            DdNode *N = Cudd_Regular(node);
            DdNode *then_node = Cudd_T(N);
            DdNode *else_node = Cudd_E(N);

            then_node = Cudd_NotCond(then_node, Cudd_IsComplement(node));
            else_node = Cudd_NotCond(else_node, Cudd_IsComplement(node));
            indent();output << "\"" << node << "\" -> \"" << then_node
                            << "\" [arrowhead = none];" << std::endl;
            indent();output << "\"" << node << "\" -> \"" << else_node
                            << "\" [arrowhead = none, style = dashed];" << std::endl;
        }
    }
    //----------------------------------------------------------------------


    indentation--;
    output << "}" << std::endl;
}

void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds,
                               std::string filename,
                               const std::vector<std::string> &function_names,
                               DotPresentationConfig conf) {
    std::ofstream outfile;
    outfile.open(filename, std::ios::out | std::ios::trunc);

    if (outfile.is_open()) {
        dump_dot_readable(mgr, bdds, outfile, function_names, conf);
        outfile.close();
    }
}

void dump_dot_readable(const Cudd &mgr, const BDD &bdd, std::ostream &output,
                       const std::vector<std::string> &function_names, DotPresentationConfig conf) {
    dump_dot_readable(mgr,std::vector<BDD>{bdd},output, function_names, conf);
}

void dump_dot_readable_to_file(const Cudd &mgr, const BDD &bdd,
                               std::string filename, const std::vector<std::string> &function_names, DotPresentationConfig conf) {
    dump_dot_readable_to_file(mgr, std::vector<BDD>{bdd}, filename, function_names, conf);
}

} // namespace abo::util
