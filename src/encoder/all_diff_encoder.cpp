#include "all_diff_encoder.h"

void AllDiffEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    encode_symmetry_breaking(config_data, graph_data, gurobi_data);

    encode_target_value(config_data, graph_data, gurobi_data);

    encode_span(config_data, graph_data, gurobi_data);

    link_label_and_assignment(config_data, graph_data, gurobi_data);
    encode_all_diff(config_data, graph_data, gurobi_data);

    gurobi_data.model->setObjective(GRBLinExpr(gurobi_data.span), GRB_MINIMIZE);
}

void AllDiffEncoder::encode_all_diff(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    for (int j = 1; j <= config_data.upper_bound; j++)
    {
        GRBLinExpr expr = 0;

        for (int i = 1; i <= graph_data.num_vertices; i++)
            expr += gurobi_data.assignment[i][j];

        gurobi_data.model->addConstr(expr <= 1, "all_diff_" + std::to_string(j));
    }
}