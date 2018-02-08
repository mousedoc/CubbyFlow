/*************************************************************************
> File Name: ParticleSystemData.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: ParticleSystemData functions for CubbyFlow Python API.
> Created Time: 2018/02/08
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PYTHON_PARTICLE_SYSTEM_DATA_H
#define CUBBYFLOW_PYTHON_PARTICLE_SYSTEM_DATA_H

#include <pybind11/pybind11.h>

void AddParticleSystemData2(pybind11::module& m);
void AddParticleSystemData3(pybind11::module& m);

#endif