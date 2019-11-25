#include "dump_dot.hpp"

#include <set>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>

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

    void dump_dot_readable(const Cudd &mgr, const BDD &b, std::string filename) {
        std::ofstream outfile;
        outfile.open(filename, std::ios::out | std::ios::trunc );

        std::vector<std::vector<DdNode*>> level_nodes;
        std::set<DdNode*> visited;
        collect_nodes_rec(b.getNode(), visited, level_nodes);

        int indentation = 0;
        std::function<void()> indent([&]() { for (int i = 0;i<indentation;i++) outfile <<"\t"; });
        outfile <<"digraph \"DD\"{"<<std::endl;
        indentation++;

        indent(); outfile <<"size = \"5,10\""<<std::endl;
        indent(); outfile <<"center = true;"<<std::endl;
        indent(); outfile <<"edge [dir = non];"<<std::endl;
        indent(); outfile <<"nodesep = \"0.2\";"<<std::endl;
        indent(); outfile <<"ranksep = \"0.25\";"<<std::endl;
        indent(); outfile <<"margin=0;"<<std::endl;

        indent(); outfile <<"{"<<std::endl;
        indentation++;

        indent(); outfile <<"edge [style = invis];"<<std::endl;
        indent();
        for (unsigned int i = 0;i<level_nodes.size();i++) {
            if (level_nodes[i].size() > 0) {
                outfile <<"\""<<level_nodes[i][0]<<"\" -> ";
            }
        }
        outfile <<"\""<<mgr.bddOne().getNode()<<"\";"<<std::endl;

        indentation--;
        indent(); outfile <<"}"<<std::endl;

        for (unsigned int i = 0;i<level_nodes.size();i++) {
            if (level_nodes[i].size() > 0) {
                indent(); outfile <<"{"<<std::endl;
                indentation++;
                indent(); outfile <<"rank = same;"<<std::endl;
                for (DdNode *n : level_nodes[i]) {
                    indent(); outfile <<"\""<<n<<"\" [label = <<I>x&#832"<<i+1<<";</I>>, shape = circle, fixedsize=true, width=0.35];"<<std::endl;
                }
                indentation--;
                indent(); outfile <<"}"<<std::endl;
            }
        }

        indent(); outfile <<"{ ranke = same;"<<std::endl;
        indent(); outfile <<"node [shape = box]; \""<<mgr.bddOne().getNode()<<"\" [label = <&#8868;>, shape = square, fixedsize=true, width=0.35];"<<std::endl;
        indent(); outfile <<"\""<<mgr.bddZero().getNode()<<"\" [label = <&#8869;>, shape = square, fixedsize=true, width=0.35]; }"<<std::endl;

        for (auto &nodes : level_nodes) {
            for (auto node : nodes) {
                DdNode *N = Cudd_Regular(node);
                DdNode *then_node = Cudd_T(N);
                DdNode *else_node = Cudd_E(N);

                then_node = Cudd_NotCond(then_node, Cudd_IsComplement(node));
                else_node = Cudd_NotCond(else_node, Cudd_IsComplement(node));
                indent(); outfile <<"\""<<node<<"\" -> \""<<then_node<<"\" [arrowhead = none];"<<std::endl;
                indent(); outfile <<"\""<<node<<"\" -> \""<<else_node<<"\" [arrowhead = none, style = dashed];"<<std::endl;
            }
        }

        indentation--;
        outfile <<"}"<<std::endl;
    }

}
