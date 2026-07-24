#ifndef HAS_HOLE_ENCODER_H
#define HAS_HOLE_ENCODER_H

#include "encoder.h"

class HasHoleEncoder : public Encoder
{
public:
    GRBModel encode_model(ConfigData &config_data, GraphData &graph_data, GurobiData &gurobi_data) override;
};

#endif