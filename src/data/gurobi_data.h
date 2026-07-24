#ifndef GUROBI_DATA_H
#define GUROBI_DATA_H

#include <gurobi_c++.h>
#include <memory>

#include "config_data.h"
#include "graph_data.h"

class GurobiData
{
public:
    GRBEnv env;
    GRBModel model;

    /* label[i] = j + 1 is equivalent to assignment[i][j] = true */
    std::vector<GRBVar> label;

    /* assignment[i][j] = true is equivalent to label[i] = j + 1 */
    std::vector<std::vector<GRBVar>> assignment;

    GRBVar span;

    GurobiData(const ConfigData &config_data, const GraphData &graph_data) : env(), model(env)
    {
        init_label(config_data, graph_data);
        init_assignment(config_data, graph_data);
        init_span(config_data, graph_data);
    }

    ~GurobiData() = default;

private:
    void init_label(const ConfigData &config_data, const GraphData &graph_data)
    {
        label.resize(graph_data.num_vertices);
        for (int i = 0; i < graph_data.num_vertices; i++)
        {
            label[i] = model.addVar(1, config_data.upper_bound, 0, GRB_INTEGER);
        }
    }

    void init_assignment(const ConfigData &config_data, const GraphData &graph_data)
    {
        assignment.resize(graph_data.num_vertices);
        for (int i = 0; i < graph_data.num_vertices; i++)
        {
            assignment[i].resize(config_data.upper_bound);
            for (int j = 0; j < config_data.upper_bound; j++)
                assignment[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
        }
    }

    void init_span(const ConfigData &config_data, const GraphData &graph_data)
    {
        span = model.addVar(config_data.lower_bound, config_data.upper_bound, 0, GRB_INTEGER);
    }
};

#endif