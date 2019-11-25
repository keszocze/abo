#include "dump_dot.hpp"

#include <set>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <cassert>

namespace abo::util {

    /// Collects child nodes of the BDD and assigns them to different variable levels
    void collect_nodes_rec(DdNode *node, std::set<DdNode*> &visited, std::vector<std::vector<DdNode*>> &level_nodes) {
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

    void dump_dot_readable(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                           std::ostream &output, DotPresentationConfig conf) {

        assert(bdds.size() == function_names.size() || function_names.empty());

        // default populate function names
        std::vector<std::string> function_names_ = function_names;
        if (function_names.empty() && bdds.size() > 1) {
            for (std::size_t i = 0;i<bdds.size();i++) {
                std::string name = "<<I>f";
                for (char c : std::to_string(i)) {
                    name += std::string("&#832") + std::to_string(c - '0') + std::string(";");
                }
                name += "</I>>";
                function_names_.push_back(name);
            }
        }

        std::vector<std::vector<DdNode*>> level_nodes;
        std::set<DdNode*> visited;
        for (BDD b : bdds) {
            collect_nodes_rec(b.getNode(), visited, level_nodes);
        }

        int indentation = 0;
        std::function<void()> indent([&]() { for (int i = 0;i<indentation;i++) output <<"\t"; });

        output <<"digraph \"DD\"{"<<std::endl;
        indentation++;

        indent(); output <<"center = true;"<<std::endl;
        indent(); output <<"edge [dir = non];"<<std::endl;
        indent(); output <<"nodesep = \""<<conf.node_seperation<<"\";"<<std::endl;
        indent(); output <<"ranksep = \""<<conf.rank_seperation<<"\";"<<std::endl;
        indent(); output <<"margin=0;"<<std::endl;

        // create nodes for function names
        indent(); output <<"{"<<std::endl;
        indentation++;
        indent(); output <<"rank = same;"<<std::endl;
        indent(); output <<"node [shape = "<<conf.function_name_shape<<", fixedsize = true, width = "<<conf.function_name_width<<"];"<<std::endl;
        indent(); output <<"edge [style = invis];"<<std::endl;
        for (std::size_t i = 0;i<function_names_.size();i++) {
            indent(); output <<"\""<<function_names_[i]<<"\" [label = "<<function_names_[i]<<"];"<<std::endl;
        }
        indent();
        for (std::size_t i = 0;i<function_names_.size();i++) {
            output <<"\""<<function_names_[i]<<"\" "<<(i < function_names_.size()-1 ? " -> " : "");
        }
        output <<std::endl;
        indentation--;
        indent(); output <<"}"<<std::endl;

        indent(); output <<"{"<<std::endl;
        indentation++;
        for (std::size_t i = 0;i<function_names_.size();i++) {
            indent(); output <<"\""<<function_names_[i]<<"\":s -> \""<<bdds[i].getNode()<<"\":n [arrowhead = none];"<<std::endl;
        }
        indentation--;
        indent(); output <<"}"<<std::endl;

        // create invisible connections to properly order the nodes
        indent(); output <<"{"<<std::endl;
        indentation++;

        indent(); output <<"edge [style = invis];"<<std::endl;
        if (function_names_.size() > 0) {
            indent(); output <<"\""<<function_names_[0]<<"\" -> ";
        }
        indent();
        for (unsigned int i = 0;i<level_nodes.size();i++) {
            if (level_nodes[i].size() > 0) {
                output <<"\""<<level_nodes[i][0]<<"\" -> ";
            }
        }
        output <<"\""<<mgr.bddOne().getNode()<<"\";"<<std::endl;

        indentation--;
        indent(); output <<"}"<<std::endl;

        for (unsigned int i = 0;i<level_nodes.size();i++) {
            if (level_nodes[i].size() > 0) {
                indent(); output <<"{"<<std::endl;
                indentation++;
                indent(); output <<"rank = same;"<<std::endl;
                for (DdNode *n : level_nodes[i]) {
                    indent(); output <<"\""<<n<<"\" [label = <<I>x";
                    for (char c : std::to_string(i+1)) {
                        output <<"&#832"<<c<<";";
                    }
                    output <<"</I>>, shape = "<<conf.node_shape<<", fixedsize=true, width="<<conf.node_width<<"];"<<std::endl;
                }
                indentation--;
                indent(); output <<"}"<<std::endl;
            }
        }

        // terminal nodes
        indent(); output <<"{"<<std::endl;
        indentation++;
        indent(); output <<"ranke = same;"<<std::endl;
        indent(); output <<"\""<<mgr.bddOne().getNode()<<"\" [label = <&#8868;>, shape = "<<conf.terminal_shape<<", width="<<conf.terminal_width<<", fixedsize=true];"<<std::endl;
        indent(); output <<"\""<<mgr.bddZero().getNode()<<"\" [label = <&#8869;>, shape = "<<conf.terminal_shape<<", width="<<conf.terminal_width<<", fixedsize=true];"<<std::endl;
        indentation--;
        indent(); output <<"}"<<std::endl;

        for (auto &nodes : level_nodes) {
            for (auto node : nodes) {
                DdNode *N = Cudd_Regular(node);
                DdNode *then_node = Cudd_T(N);
                DdNode *else_node = Cudd_E(N);

                then_node = Cudd_NotCond(then_node, Cudd_IsComplement(node));
                else_node = Cudd_NotCond(else_node, Cudd_IsComplement(node));
                indent(); output <<"\""<<node<<"\" -> \""<<then_node<<"\" [arrowhead = none];"<<std::endl;
                indent(); output <<"\""<<node<<"\" -> \""<<else_node<<"\" [arrowhead = none, style = dashed];"<<std::endl;
            }
        }

        indentation--;
        output <<"}"<<std::endl;
    }

    void dump_dot_readable_to_file(const Cudd &mgr, const std::vector<BDD> &bdds, const std::vector<std::string> &function_names,
                                   std::string filename, DotPresentationConfig conf) {
        std::ofstream outfile;
        outfile.open(filename, std::ios::out | std::ios::trunc);

        if (outfile.is_open()) {
            dump_dot_readable(mgr, bdds, function_names, outfile, conf);
        }
    }

}
