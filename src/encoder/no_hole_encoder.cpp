#include "no_hole_encoder.h"

GRBModel NoHoleEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    encode_symmetry_breaking(config_data, graph_data, gurobi_data);

    encode_target_value(config_data, graph_data, gurobi_data);

    encode_span(config_data, graph_data, gurobi_data);

    link_label_and_assignment(config_data, graph_data, gurobi_data);
    encode_no_hole(config_data, graph_data, gurobi_data);

    gurobi_data.model->setObjective(GRBLinExpr(gurobi_data.span), GRB_MINIMIZE);

    return *gurobi_data.model;
}

void NoHoleEncoder::encode_no_hole(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    for (int j = 1; j <= config_data.upper_bound; j++)
    {
        GRBLinExpr assign_expr = 0;

        for (int i = 1; i <= graph_data.num_vertices; i++)
            assign_expr += gurobi_data.assignment[i][j];

        gurobi_data.model->addConstr(gurobi_data.span - j + 1 <= config_data.upper_bound * assign_expr, "no_hole_" + std::to_string(j));
    }
}