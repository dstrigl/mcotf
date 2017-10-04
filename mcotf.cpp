// On-the-Fly Algorithm for Emptyness of NBAs [Yannakakis et. al].
// Model Checking, Institute of Computer Science, University of Innsbruck.
// Written by Daniel Strigl.

//          Copyright Daniel Strigl 2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace boost;

// Typedefs and defines
typedef adjacency_list<vecS, vecS, directedS> graph_t;
typedef graph_traits<graph_t>::vertex_descriptor vertex_t;
#define _countof(array) (sizeof(array) / sizeof(array[0]))

///////////////////////////////////////////////////////////////////////////////

#if 1

// ----------------------------------------------------------------------------
//  Sample 1 
//    [http://cl-informatik.uibk.ac.at/teaching/ss08/mc/exercises.pdf, Ex. 1]  
// ----------------------------------------------------------------------------

// Set up the vertex names
enum { _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, N };
char* name[] = { 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" };

// Specify the edges in the graph
typedef std::pair<int, int> E;
E edge_array[] = { 
    E( _0,  _1), E( _0,  _2), E( _0,  _3),
    E( _1,  _0),
    E( _2,  _3), E( _2,  _8),
    E( _3,  _4),
    E( _4,  _5), E( _4,  _6), E( _4,  _7),
    E( _5,  _7),
    E( _6,  _2), E( _6,  _7),
    E( _7, _11),
    E( _8,  _9),
    E( _9,  _3), E( _9,  _6), E( _9, _10),
    E(_10,  _6), E(_10,  _7), E(_10,  _9), E(_10, _11),
    E(_11,  _7)
};

// Specify the final (accepting) states
vertex_t F[] = { _0, _4, _5, _9, _10 };

// Specify the initial state
vertex_t S = _0;

#else /////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
//  Sample 2 
//    [http://cl-informatik.uibk.ac.at/teaching/ss08/mc/ohp/1x1.pdf, Slide 23]  
// ----------------------------------------------------------------------------

// Set up the vertex names
enum { _q0, _q1, _q2, _q3, N };
char* name[] = { "q0", "q1", "q2", "q3" };

// Specify the edges in the graph
typedef std::pair<int, int> E;
E edge_array[] = { 
    E(_q0, _q1), E(_q0, _q2),
    E(_q1, _q2), E(_q1, _q3),
    E(_q2, _q3),
    E(_q3, _q2),
};

// Specify the final (accepting) states
vertex_t F[] = { _q1, _q2 };

// Specify the initial state
vertex_t S = _q0;

#endif

///////////////////////////////////////////////////////////////////////////////

static std::vector<vertex_t> finalStates(F, F + _countof(F));
static graph_t g(edge_array, edge_array + sizeof(edge_array) / sizeof(E), N);
static std::vector<bool> marked(num_vertices(g), false);
static std::vector<bool> flagged(num_vertices(g), false);
static std::vector<vertex_t> outer_dfs_stack;
static std::vector<vertex_t> inner_dfs_stack;
static std::basic_ostringstream<char> word;

static void plot_list(std::vector<bool>& l)
{
    std::vector<vertex_t> tmp;
    for (size_t i = 0; i < l.size(); ++i)
        if (l[i])
            tmp.push_back(i);

    if (tmp.empty())
    {
        std::cout << "$\\varnothing$";
        return;
    }

    std::cout << "\\{";

    for (size_t i = 0; i < tmp.size(); ++i)
    {
        std::cout << name[tmp[i]];
        if (i < tmp.size() - 1)
            std::cout << ", ";
    }

    std::cout << "\\}";
}

static void plot_stack(std::vector<vertex_t>& s)
{
    if (s.empty())
    {
        std::cout << "$\\varepsilon$";
        return;
    }

    for (size_t i = s.size(); i > 0; --i)
    {
        std::cout << name[s[i-1]];
        if (i > 1)
            std::cout << ", ";
    }
}

static void plot_stat()
{
    plot_stack(outer_dfs_stack);
    std::cout << " & ";
    plot_stack(inner_dfs_stack); 
    std::cout << " & ";
    plot_list(marked);
    std::cout << " & ";
    plot_list(flagged);
    std::cout << " \\\\" << std::endl;
}

static bool inner_dfs(const graph_t& g, vertex_t q)
{
    inner_dfs_stack.push_back(q);

    flagged[q] = true;

    plot_stat();

    graph_traits<graph_t>::adjacency_iterator vi, vi_end;

    for (tie(vi, vi_end) = adjacent_vertices(q, g); vi != vi_end; ++vi)
    {
        std::vector<vertex_t>::const_iterator res;

        if ((res = find(outer_dfs_stack.begin(), outer_dfs_stack.end(), *vi)) 
            != outer_dfs_stack.end())
        {
            for (std::vector<vertex_t>::const_iterator 
                itr = outer_dfs_stack.begin();
                itr != outer_dfs_stack.end(); ++itr)
            {
                word << name[*itr] << "\\:";
            }

            word << "\\left(\\:";

            for (std::vector<vertex_t>::const_iterator 
                itr = inner_dfs_stack.begin() + 1;
                itr != inner_dfs_stack.end(); ++itr)
            {
                word << name[*itr] << "\\:";
            }

            for (std::vector<vertex_t>::const_iterator itr = res;
                itr != outer_dfs_stack.end(); ++itr)
            {
                word << name[*itr] << "\\:";
            }

            word << "\\right)" << "^{\\omega}";

            return false;
        }
        else if (!flagged[*vi])
        {
            if (!inner_dfs(g, *vi))
                return false;
        }
    }

    inner_dfs_stack.pop_back();

    return true;
}

static bool outer_dfs(const graph_t& g, vertex_t q)
{
    outer_dfs_stack.push_back(q);

    marked[q] = true;

    plot_stat();

    graph_traits<graph_t>::adjacency_iterator vi, vi_end;

    for (tie(vi, vi_end) = adjacent_vertices(q, g); vi != vi_end; ++vi)
    {
        if (!marked[*vi])
        {
            if (!outer_dfs(g, *vi))
                return false;
        }
    }

    if (find(finalStates.begin(), finalStates.end(), q) != finalStates.end())
        if (!inner_dfs(g, q))
            return false;

    outer_dfs_stack.pop_back();

    return true;
}

int main(int argc, char* argv[])
{
    std::cout << "\\documentclass[landscape]{article}" << std::endl;
    std::cout << "\\usepackage{lscape}" << std::endl;
    std::cout << "\\usepackage[latin1]{inputenc}" << std::endl;
    std::cout << "\\usepackage{amssymb}" << std::endl;
    std::cout << std::endl;
    std::cout << "\\begin{document}" << std::endl;
    std::cout << std::endl;
    std::cout << "\\begin{table}[htb]" << std::endl;
    std::cout << "\\begin{center}" << std::endl;
    std::cout << "\\begin{tabular}{cccc}" << std::endl;
    std::cout << "\\textbf{outer\\_dfs-stack} & \\textbf{inner\\_dfs-stack} &"\
        " \\textbf{marked} & \\textbf{flagged} \\\\" << std::endl;
    std::cout << "\\hline" << std::endl;
    std::cout << "$\\varepsilon$ & $\\varepsilon$ & $\\varnothing$ &"\
        " $\\varnothing$ \\\\" << std::endl;

    const bool result = outer_dfs(g, S);

    std::cout << "\\end{tabular}" << std::endl;
    std::cout << "\\end{center}" << std::endl;
    std::cout << "\\end{table}" << std::endl;
    std::cout << std::endl;

    std::cout << "\\begin{center}" << std::endl;
    if (result)
        std::cout << "\\textbf{terminate}(true)" << std::endl;
    else
        std::cout << "\\textbf{terminate}(false)" << std::endl;
    std::cout << "\\end{center}" << std::endl;

    if (!result)
    {
        std::cout << std::endl;
        std::cout << "\\begin{center}" << std::endl;
        std::cout << "$word = " << word.str() << "$" << std::endl;
        std::cout << "\\end{center}" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "\\end{document}" << std::endl;

    return 0;
}
