#ifndef ENCODER_H
#define ENCODER_H

#include <gurobi_c++.h>

#include "../data/graph_data.h"
#include "../data/gurobi_data.h"

class Encoder
{
public:
    virtual ~Encoder() = default;

    virtual void encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data) = 0;

protected:
    void encode_symmetry_breaking(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        gurobi_data.init_label(config_data, graph_data);

        std::vector<int> degree(graph_data.num_vertices + 1, 0);
        for (const auto &edge : graph_data.edges)
        {
            degree[edge.u]++;
            degree[edge.v]++;
        }

        int max_degree_vertex = 1;
        for (int i = 2; i <= graph_data.num_vertices; i++)
            if (degree[i] > degree[max_degree_vertex])
                max_degree_vertex = i;

        gurobi_data.model->addConstr(gurobi_data.label[max_degree_vertex] <= (config_data.upper_bound + 1) / 2, "symmetry_highest_degree");
    }

    void encode_target_value(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        gurobi_data.init_label(config_data, graph_data);
        gurobi_data.init_span(config_data, graph_data);

        switch (config_data.target_value_type)
        {
        case TargetValueType::abp:
        {
            for (const auto &edge : graph_data.edges)
            {
                GRBVar diff = gurobi_data.model->addVar(-config_data.upper_bound, config_data.upper_bound, 0, GRB_INTEGER, "diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));

                gurobi_data.model->addConstr(diff == gurobi_data.label[edge.u] - gurobi_data.label[edge.v]);

                GRBVar absDiff = gurobi_data.model->addVar(0, config_data.upper_bound, 0, GRB_INTEGER, "abs_diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));

                gurobi_data.model->addGenConstrAbs(absDiff, diff);

                gurobi_data.model->addConstr(absDiff >= config_data.target_value);
            }
            break;
        }
        case TargetValueType::cabp:
        {
            for (const auto &edge : graph_data.edges)
            {
                GRBVar diff = gurobi_data.model->addVar(-config_data.upper_bound, config_data.upper_bound, 0, GRB_INTEGER, "diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));
                gurobi_data.model->addConstr(diff == gurobi_data.label[edge.u] - gurobi_data.label[edge.v]);

                GRBVar absDiff = gurobi_data.model->addVar(0, config_data.upper_bound, 0, GRB_INTEGER, "abs_diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));
                gurobi_data.model->addGenConstrAbs(absDiff, diff);

                GRBVar cyclicDiff = gurobi_data.model->addVar(0, config_data.upper_bound, 0, GRB_INTEGER, "cyclic_diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));
                gurobi_data.model->addConstr(cyclicDiff == gurobi_data.span - absDiff);

                GRBVar minDiff = gurobi_data.model->addVar(0, config_data.upper_bound, 0, GRB_INTEGER, "min_diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));
                GRBVar vars[] = {absDiff, cyclicDiff};
                gurobi_data.model->addGenConstrMin(minDiff, vars, 2, GRB_INTEGER, "min_diff_" + std::to_string(edge.u) + "_" + std::to_string(edge.v));

                gurobi_data.model->addConstr(minDiff >= config_data.target_value);
            }
            break;
        }
        }
    }

    void encode_span(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        gurobi_data.init_label(config_data, graph_data);
        gurobi_data.init_span(config_data, graph_data);

        for (int i = 1; i <= graph_data.num_vertices; i++)
            gurobi_data.model->addConstr(gurobi_data.span >= gurobi_data.label[i], "span_" + std::to_string(i));
    }

    void link_label_and_assignment(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        gurobi_data.init_label(config_data, graph_data);
        gurobi_data.init_assignment(config_data, graph_data);

        for (int i = 1; i <= graph_data.num_vertices; i++)
        {
            GRBLinExpr label_expr = 0;
            GRBLinExpr assign_expr = 0;

            for (int j = 1; j <= config_data.upper_bound; j++)
            {
                label_expr += (j)*gurobi_data.assignment[i][j];
                assign_expr += gurobi_data.assignment[i][j];
            }

            gurobi_data.model->addConstr(label_expr == gurobi_data.label[i], "label_link_" + std::to_string(i));

            gurobi_data.model->addConstr(assign_expr == 1, "one_label_" + std::to_string(i));
        }
    }
};

#endif