#include "has_hole_encoder.h"

GRBModel HasHoleEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    return *gurobi_data.model;
}