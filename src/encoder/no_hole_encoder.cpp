#include "no_hole_encoder.h"

GRBModel NoHoleEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    return *gurobi_data.model;
}