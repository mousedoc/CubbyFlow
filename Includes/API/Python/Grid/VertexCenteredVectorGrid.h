/*************************************************************************
> File Name: VertexCenteredVectorGrid.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: VertexCenteredVectorGrid functions for CubbyFlow Python API.
> Created Time: 2018/02/07
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PYTHON_VERTEX_CENTERED_VECTOR_GRID_H
#define CUBBYFLOW_PYTHON_VERTEX_CENTERED_VECTOR_GRID_H

#include <pybind11/pybind11.h>

void AddVertexCenteredVectorGrid2(pybind11::module& m);
void AddVertexCenteredVectorGrid3(pybind11::module& m);

#endif