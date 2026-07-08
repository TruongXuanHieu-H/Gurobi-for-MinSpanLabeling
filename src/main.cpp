#include <gurobi_c++.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "edge.h"
#include "data/config_data.h"
#include "data/graph_data.h"
#include "data/gurobi_data.h"
#include "encoder/encoder.h"
#include "encoder/all_diff_encoder.h"
#include "encoder/has_hole_encoder.h"
#include "encoder/no_hole_encoder.h"
#include "verifier/verifier.h"
#include "verifier/all_diff_verifier.h"
#include "verifier/has_hole_verifier.h"
#include "verifier/no_hole_verifier.h"

std::unique_ptr<Encoder> get_encoder(VerticesMode vertices_mode)
{
    switch (vertices_mode)
    {
    case VerticesMode::no_hole:
        return std::make_unique<NoHoleEncoder>();
    case VerticesMode::has_hole:
        return std::make_unique<HasHoleEncoder>();
    case VerticesMode::all_diff:
        return std::make_unique<AllDiffEncoder>();

    default:
        exit(1);
    }
}

std::unique_ptr<Verifier> get_verifier(VerticesMode vertices_mode)
{
    switch (vertices_mode)
    {
    case VerticesMode::no_hole:
        return std::make_unique<NoHoleVerifier>();
    case VerticesMode::has_hole:
        return std::make_unique<HasHoleVerifier>();
    case VerticesMode::all_diff:
        return std::make_unique<AllDiffVerifier>();

    default:
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    ConfigData config_data(argc, argv);
    GraphData graph_data(config_data);
    GurobiData gurobi_data(config_data, graph_data);

    std::unique_ptr<Encoder> encoder = get_encoder(config_data.vertices_mode);
    GRBModel model = encoder->encode_model(config_data, graph_data, gurobi_data);

    model.set(GRB_IntParam_OutputFlag, 1);
    model.set(GRB_DoubleParam_TimeLimit, config_data.time_limit);

    gurobi_data.model->optimize();

    int status = gurobi_data.model->get(GRB_IntAttr_Status);

    if (status == GRB_OPTIMAL || status == GRB_TIME_LIMIT || status == GRB_SUBOPTIMAL)
    {
        if (gurobi_data.model->get(GRB_IntAttr_SolCount) > 0)
        {
            std::vector<int> solution(graph_data.num_vertices);

            for (int v = 0; v < graph_data.num_vertices; v++)
                solution[v] = static_cast<int>(gurobi_data.label[v].get(GRB_DoubleAttr_X));

            int solution_span = static_cast<int>(gurobi_data.span.get(GRB_DoubleAttr_X));

            std::unique_ptr<Verifier> verifier = get_verifier(config_data.vertices_mode);

            if (verifier->verify(config_data, graph_data, solution, solution_span))
            {
                std::cout << "! --------------------------------------------------------\n";
                std::cout << "! Best span = " << solution_span << "\n";
                std::cout << "! Labeling: ";

                for (int v = 0; v < graph_data.num_vertices; v++)
                    std::cout << solution[v] << " ";

                std::cout << "\n";
                std::cout << "! --------------------------------------------------------\n";
            }
        }
        else
        {
            std::cout << "! --------------------------------------------------------\n";
            std::cout << "! No feasible solution\n";
            std::cout << "! --------------------------------------------------------\n";
        }
    }
    else
    {
        std::cout << "! --------------------------------------------------------\n";
        std::cout << "! Optimization failed\n";
        std::cout << "! Status = " << status << "\n";
        std::cout << "! --------------------------------------------------------\n";
    }

    return 0;
}