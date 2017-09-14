/*************************************************************************
> File Name: LevelSetLiquidSim.h
> Project Name: CubbyFlow
> Author: Dongmin Kim
> Purpose: Level set based 3-D liquid Simulator
> Created Time: 2017/09/14
> Copyright (c) 2017, Dongmin Kim
*************************************************************************/
#ifndef CUBBYFLOW_LEVELSET_LIQUID_SOLVER3_H
#define CUBBYFLOW_LEVELSET_LIQUID_SOLVER3_H

#include <../../../DirectX12/NormalMapApp.h>
#include <Geometry/TriangleMesh3.h>

namespace CubbyFlow
{
	class LevelSetLiquidSim : public NormalMapApp
	{
	public:
		bool Initialize();

	private:
		void AnimateMaterials(const GameTimer& gt);
		void RunSimulation();
//		void WriteMesh(const TriangleMesh3& mesh);
				
	};
}

#endif