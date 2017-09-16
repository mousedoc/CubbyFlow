/*************************************************************************
> File Name: LevelSetLiquidSim.h
> Project Name: CubbyFlow
> Author: Dongmin Kim
> Purpose: Level set based 3-D liquid Simulator
> Created Time: 2017/09/14
> Copyright (c) 2017, Dongmin Kim
*************************************************************************/
#ifndef CUBBYFLOW_LEVELSET_LIQUID_SOLVERSIM3_H
#define CUBBYFLOW_LEVELSET_LIQUID_SOLVERSIM3_H

#include <../../../DirectX12/NormalMapApp.h>
#include <Geometry/TriangleMesh3.h>
namespace CubbyFlow
{
	class LevelSetLiquidSim// : public NormalMapApp
	{
	public:
		LevelSetLiquidSim(double timeIntervalInSeconds);
		bool Initialize();

	private:
		void AnimateMaterials(const GameTimer& gt);
		void Draw(const GameTimer& gt);
		/*
		GeometryGenerator::MeshData SaveTriangleMesh(const ScalarGrid3Ptr& sdf, unsigned int frameCnt);

		double mTimeIntervalInSeconds;

		std::unique_ptr<MeshGeometry> mLiquidGeo = nullptr;
		std::unique_ptr<UploadBuffer<Vertex>> mLiquidSDF = nullptr;
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
		std::vector<std::unique_ptr<RenderItem>> mAllRitems;
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];
		*/
	};
}

#endif