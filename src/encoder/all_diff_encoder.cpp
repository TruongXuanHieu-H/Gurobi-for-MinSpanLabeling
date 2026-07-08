#include "all_diff_encoder.h"

GRBModel AllDiffEncoder::encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data)
{
    return *gurobi_data.model;
}