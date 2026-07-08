#include "has_hole_encoder.h"

GRBModel HasHoleEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    encode_symmetry_breaking(config_data, graph_data, gurobi_data);

    encode_target_value(config_data, graph_data, gurobi_data);

    encode_span(config_data, graph_data, gurobi_data);

    gurobi_data.model->setObjective(GRBLinExpr(gurobi_data.span), GRB_MINIMIZE);

    return *gurobi_data.model;
}