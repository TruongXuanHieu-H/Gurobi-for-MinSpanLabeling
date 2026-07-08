#ifndef ENCODER_H
#define ENCODER_H

#include <gurobi_c++.h>

#include "../data/graph_data.h"
#include "../data/gurobi_data.h"

class Encoder
{
public:
    virtual ~Encoder() = default;

    virtual GRBModel encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data) = 0;
};

#endif