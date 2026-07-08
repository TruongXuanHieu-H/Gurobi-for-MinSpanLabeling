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
    std::vector<GRBVar> label;
    GRBVar span;

    GurobiData(ConfigData &config_data, GraphData &graph_data) : label(graph_data.num_vertices)
    {
        env = std::make_unique<GRBEnv>(true);
        env->start();

        model = std::make_unique<GRBModel>(env);

        for (int i = 0; i < graph_data.num_vertices; i++)
        {
            label[i] = model->addVar(1, config_data.upper_bound, 0, GRB_INTEGER, "vertex_" + std::to_string(i + 1));
        }

        span = model->addVar(config_data.lower_bound, config_data.upper_bound, 0, GRB_INTEGER, "span");
    }

    ~GurobiData() = default;
};

#endif