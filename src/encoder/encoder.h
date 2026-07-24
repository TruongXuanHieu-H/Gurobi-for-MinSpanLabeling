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
        std::vector<int> degree(graph_data.num_vertices, 0);
        for (const auto &edge : graph_data.edges)
        {
            degree[edge.u]++;
            degree[edge.v]++;
        }

        int max_degree_vertex = 0; // First vertex
        for (int i = 1; i < graph_data.num_vertices; i++)
            if (degree[i] > degree[max_degree_vertex])
                max_degree_vertex = i;

        gurobi_data.model.addConstr(gurobi_data.label[max_degree_vertex] <= (config_data.upper_bound + 1) / 2);
    }

    void encode_target_value(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        for (const auto &edge : graph_data.edges)
        {
            GRBVar diff = gurobi_data.model.addVar(-config_data.upper_bound, config_data.upper_bound, 0.0, GRB_INTEGER);
            gurobi_data.model.addConstr(diff == gurobi_data.label[edge.u] - gurobi_data.label[edge.v]);

            GRBVar absDiff = gurobi_data.model.addVar(0, config_data.upper_bound, 0.0, GRB_INTEGER);
            gurobi_data.model.addGenConstrAbs(absDiff, diff);

            switch (config_data.target_value_mode)
            {
            case TargetValueMode::abp:
            {
                gurobi_data.model.addConstr(absDiff >= config_data.target_value);
                break;
            }
            case TargetValueMode::cabp:
            {
                gurobi_data.model.addConstr(absDiff >= config_data.target_value);
                gurobi_data.model.addConstr(gurobi_data.span - absDiff >= config_data.target_value);
                break;
            }
            default:
                break;
            }
        }
    }

    void encode_span(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        GRBLinExpr is_label_one_used = 0;
        for (int v = 0; v < graph_data.num_vertices; v++)
            is_label_one_used += gurobi_data.assignment[v][0];
        gurobi_data.model.addConstr(is_label_one_used >= 1);

        std::vector<GRBVar> is_max(graph_data.num_vertices);
        GRBLinExpr sum = 0;
        for (int v = 0; v < graph_data.num_vertices; v++)
        {
            is_max[v] = gurobi_data.model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
            sum += is_max[v];

            // is_max[v] = 1  => label[v] == span
            gurobi_data.model.addGenConstrIndicator(is_max[v], 1, gurobi_data.label[v] == gurobi_data.span);

            // is_max[v] = 0  => label[v] <= span - 1
            gurobi_data.model.addGenConstrIndicator(is_max[v], 0, gurobi_data.label[v] <= gurobi_data.span - 1);
        }
        gurobi_data.model.addConstr(sum >= 1);
    }

    void link_label_and_assignment(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
    {
        for (int i = 0; i < graph_data.num_vertices; i++)
        {
            GRBLinExpr label_expr = 0;
            GRBLinExpr assign_expr = 0;

            for (int j = 0; j < config_data.upper_bound; j++)
            {
                label_expr += (j + 1) * gurobi_data.assignment[i][j];
                assign_expr += gurobi_data.assignment[i][j];
            }

            gurobi_data.model.addConstr(label_expr == gurobi_data.label[i]);
            gurobi_data.model.addConstr(assign_expr == 1);
        }
    }
};

#endif