#ifndef GUROBI_DATA_H
#define GUROBI_DATA_H

#include <gurobi_c++.h>
#include <memory>

#include "config_data.h"
#include "graph_data.h"

class GurobiData
{
public:
    std::unique_ptr<GRBEnv> env;
    std::unique_ptr<GRBModel> model;

    std::vector<GRBVar> label; // label[i] represents the label assigned to vertex i
    bool is_label_initialized = false;

    std::vector<std::vector<GRBVar>> assignment; // assignment[i][j] = true iff vertex i is assigned label j
    bool is_assignment_initialized = false;

    GRBVar span;
    bool is_span_initialized = false;

    GurobiData(ConfigData &config_data, GraphData &graph_data)
    {
        env = std::make_unique<GRBEnv>(true);
        env->start();

        model = std::make_unique<GRBModel>(env.get());
    }

    void init_label(ConfigData &config_data, GraphData &graph_data)
    {
        if (is_label_initialized)
            return; // Already initialized

        label.resize(graph_data.num_vertices + 1);
        for (int i = 1; i <= graph_data.num_vertices; i++)
        {
            label[i] = model->addVar(1, config_data.upper_bound, 0, GRB_INTEGER, "vertex_" + std::to_string(i));
        }

        is_label_initialized = true;
    }

    void init_assignment(ConfigData &config_data, GraphData &graph_data)
    {
        if (is_assignment_initialized)
            return; // Already initialized

        assignment.resize(graph_data.num_vertices + 1);
        for (int i = 1; i <= graph_data.num_vertices; i++)
        {
            assignment[i].resize(config_data.upper_bound + 1);
            for (int j = 1; j <= config_data.upper_bound; j++)
                assignment[i][j] = model->addVar(0, 1, 0, GRB_BINARY, "vertex_" + std::to_string(i) + "_label_" + std::to_string(j));
        }

        is_assignment_initialized = true;
    }

    void init_span(ConfigData &config_data, GraphData &graph_data)
    {
        if (is_span_initialized)
            return; // Already initialized

        span = model->addVar(config_data.lower_bound, config_data.upper_bound, 0, GRB_INTEGER, "span");

        is_span_initialized = true;
    }

    ~GurobiData() = default;
};

#endif