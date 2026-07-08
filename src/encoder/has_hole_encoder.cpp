#include "has_hole_encoder.h"

GRBModel HasHoleEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    for (const auto &edge : graph_data.edges)
    {
        GRBVar diff = gurobi_data.model->addVar(-config_data.upper_bound, config_data.upper_bound, 0, GRB_INTEGER, "diff_" + std::to_string(edge.u + 1) + "_" + std::to_string(edge.v + 1));

        gurobi_data.model->addConstr(diff == gurobi_data.label[edge.u] - gurobi_data.label[edge.v]);

        GRBVar absDiff = gurobi_data.model->addVar(0, config_data.upper_bound, 0, GRB_INTEGER, "abs_diff_" + std::to_string(edge.u + 1) + "_" + std::to_string(edge.v + 1));

        gurobi_data.model->addGenConstrAbs(absDiff, diff);

        gurobi_data.model->addConstr(absDiff >= config_data.target_value);
    }

    for (int i = 0; i < graph_data.num_vertices; i++)
        gurobi_data.model->addConstr(gurobi_data.span >= gurobi_data.label[i], "span_" + std::to_string(i + 1));

    gurobi_data.model->setObjective(GRBLinExpr(gurobi_data.span), GRB_MINIMIZE);

    return *gurobi_data.model;
}