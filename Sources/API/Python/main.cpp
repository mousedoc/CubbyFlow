/*************************************************************************
> File Name: main.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Main entry of CubbyFlow Python API.
> Created Time: 2018/01/22
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/Array/ArrayAccessor.h>
#include <API/Python/Animation/Animation.h>
#include <API/Python/Animation/PhysicsAnimation.h>
#include <API/Python/Utils/Logging.h>
#include <API/Python/Vector/Vector.h>

#include <pybind11/pybind11.h>

PYBIND11_MODULE(pyCubbyFlow, m)
{
	m.doc() =
		R"pbdoc(
			Voxel-based fluid simulation engine for computer games
		)pbdoc";

	// Trivial basic types
	AddVector2D(m);
	AddVector2F(m);
	AddVector3D(m);
	AddVector3F(m);
	AddFrame(m);

	// Containers/helpers
	AddArrayAccessor1(m);
	AddArrayAccessor2(m);
	AddArrayAccessor3(m);

	// Trivial APIs
	AddLogging(m);
	
	// Animations
	AddAnimation(m);
	AddPhysicsAnimation(m);

#ifdef VERSION_INFO
	m.attr("__version__") = pybind11::str(VERSION_INFO);
#else
	m.attr("__version__") = pybind11::str("Dev");
#endif
}