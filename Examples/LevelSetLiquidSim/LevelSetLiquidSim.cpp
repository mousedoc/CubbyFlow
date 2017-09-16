/*************************************************************************
> File Name: LevelSetLiquidSim.h
> Project Name: CubbyFlow
> Author: Dongmin Kim
> Purpose: Level set based 3-D liquid Simulator
> Created Time: 2017/09/14
> Copyright (c) 2017, Dongmin Kim
*************************************************************************/
#include <../Examples/LevelSetLiquidSim/LevelSetLiquidSim.h>

namespace CubbyFlow
{
	struct Vertex
	{
		//XMFLOAT3 Pos;
		//XMFLOAT4 Color;
	};

	저거 다 주석처리해도 에러 

	LevelSetLiquidSim::LevelSetLiquidSim(double timeIntervalInSeconds) : mtimeIntervalInSeconds(timeIntervalInSeconds)
	{
		// Do nothing
	}

	bool LevelSetLiquidSim::Initialize()
	{
		if (!NormalMapApp::Initialize())
		{
			return false;
		}

		ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

		// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
		// so we have to query this information.
		mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		mCamera.SetPosition(0.0f, 2.0f, -15.0f);

		NormalMapApp::LoadTextures();
		NormalMapApp::BuildRootSignature();
		NormalMapApp::BuildDescriptorHeaps();
		NormalMapApp::BuildShadersAndInputLayout();
		NormalMapApp::BuildShapeGeometry();
		NormalMapApp::BuildMaterials();
		NormalMapApp::BuildRenderItems();
		NormalMapApp::BuildFrameResources();
		NormalMapApp::BuildPSOs();

		// Execute the initialization commands.
		ThrowIfFailed(mCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		FlushCommandQueue();

		return true;
	}

	void RunExample1(const size_t resX, unsigned int numberOfFrames, double fps)
	{
		// Build solver
		auto solver = LevelSetLiquidSolver3::Builder()
			.WithResolution({ resX, 2 * resX, resX })
			.WithDomainSizeX(1.0)
			.MakeShared();

		auto grids = solver->GetGridSystemData();
		BoundingBox3D domain = grids->GetBoundingBox();

		// Build emitter
		auto plane = Plane3::Builder()
			.WithNormal({ 0, 1, 0 })
			.WithPoint({ 0, 0.25 * domain.Height(), 0 })
			.MakeShared();

		auto sphere = Sphere3::Builder()
			.WithCenter(domain.MidPoint())
			.WithRadius(0.15 * domain.Width())
			.MakeShared();

		auto surfaceSet = ImplicitSurfaceSet3::Builder()
			.WithExplicitSurfaces({ plane, sphere })
			.MakeShared();

		auto emitter = VolumeGridEmitter3::Builder()
			.WithSourceRegion(surfaceSet)
			.MakeShared();

		solver->SetEmitter(emitter);
		emitter->AddSignedDistanceTarget(solver->GetSignedDistanceField());


		// Print simulation info
		printf("Running example 1 (water-drop)\n");

		// Initialize Simulation and Upload to Constant Buffer
		auto sdf = solver->getSignedDistanceField();
		auto meshData = SaveTriangleMesh(sdf, 0);

		auto vertices = meshData.vertices;
		auto indices = meshData.indices;
		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		mLiquidGeo = std::make_unique<MeshGeometry>();
		mLiquidGeo->Name = "LiquidGeo";

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &mLiquidGeo->VertexBufferCPU));
		CopyMemory(mLiquidGeo->VertexBufferCPU->GetBufferPoint(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &mLiquidGeo->IndexbufferCPU));
		CopyMemory(mLiquidGeo->IndexBufferCPU->GetBufferPoint(), indices.data(), ibByteSize);

		mLiquidGeo->VertexBufferCPU = d3dUtil::CreateDefaultBuffer(
			md3dDevice.Get(),
			mCommandList.Get(),
			vertices.data(),
			vbByteSize,
			mLiquidGeo->VertexBufferUploader);

		mLiquidGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
			md3dDevice.Get(),
			mCommandList.Get(),
			indices.data(),
			ibByteSize,
			mLiquidGeo->IndexBufferUploader);

		mLiquidGeo->VertexByteStride = sizeof(Vertex);
		mLiquidGeo->vertexBufferByteSize = vbByteSize;
		mLiquidGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
		mLiquidGeo->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.Indexcount = (UINT)indices.size();
		submesh.StartIndexLocation = 0;
		sumesh.BaseVertexLocation = 0;

		mLiquidGeo->DrawArgs["LiquidGeo"] = std::move(submesh);
	}

	GeometryGenerator::MeshData SaveTriangleMesh(const ScalarGrid3Ptr& sdf, unsigned int frameCnt)
	{
		GeometryGenerator::MeshData meshData;
		meshData.Vertices.resize(sdf);
		meshData.Indices32.resize(1);

		auto vertexAt = sdf->GetDataAccessor();

		sdf->ParallelForEachDataPointIndex([&](size_t i, size_t j, size_t k)
		{
			meshData.Vertices.push_back(vertexAt(i, j, k));
		});

		return meshData;
	}

	void AnimateMaterials(const GameTimer& gt)
	{
		auto sdf = solver->GetSignedDistanceField();

		Frame frame(0, 1.0 / fps);

		solver->Update(frame);

		auto meshData = SaveTriangleMesh(sdf, frame.index);

		auto CurrSDF = mCurrFrameResource->LiquidVB.get();

		for (size_t i = 0; i < meshData.Vertices.size(); i++)
		{
			Vertex v;
			v.Pos = meshData.Vertices[i];
			v.Color = XMFLOAT4(DirectX::Colors::Blue);

			CurrSDF->CopyData(i, v);
		}

		mLiquidRitem->Geo->VertexBufferGPU = CurrSDF->Resource();
	}

	void Draw(const GameTimer& gt)
	{
		//// Reuse the memory associated with command recording.
		//// We can only reset when the associated command lists have finished execution on the GPU.
		//ThrowIfFailed(mDirectCmdListAlloc->Reset());

		//// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		//// Reusing the command list reuses memory.
		//ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

		//mCommandList->RSSetViewports(1, &mScreenViewport);
		//mCommandList->RSSetScissorRects(1, &mScissorRect);

		//// Indicate a state transition on the resource usage.
		//mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		//	D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		//// Clear the back buffer and depth buffer.
		//mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
		//mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		//// Specify the buffers we are going to render to.
		//mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		//ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
		//mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		//mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

		//mCommandList->IASetVertexBuffers(0, 1, &LiquidGeo->VertexBufferView());
		//mCommandList->IASetIndexBuffer(&mLiquidGeo->IndexBufferView());
		//mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

		//mCommandList->DrawIndexedInstanced(
		//	mLiquidGeo->DrawArgs["liquidGeo"].IndexCount,
		//	1, 0, 0, 0);

		//// Indicate a state transition on the resource usage.
		//mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		//// Done recording commands.
		//ThrowIfFailed(mCommandList->Close());

		//// Add the command list to the queue for execution.
		//ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		//mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		//// swap the back and front buffers
		//ThrowIfFailed(mSwapChain->Present(0, 0));
		//mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

		//// Wait until frame commands are complete.  This waiting is inefficient and is
		//// done for simplicity.  Later we will show how to organize our rendering code
		//// so we do not have to wait per frame.
		//FlushCommandQueue();
	}

	void BuildRenderItems()
	{
		auto liquidRitem = std::make_unique<RenderItem>();
		XMStoreFloat4X4(&liquidRitem->World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		XMStoreFloat4x4(&liquidRitem->TexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
		
		liquidRitem->ObjCBIndex = 0;
		liquidRitem->Geo = mGeometries["liquidGeo"].get();
		liquidRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		liquidRitem->IndexCount = liquidRitem->Geo->DrawArgs["liquidGeo"].IndexCount;
		liquidRitem->StartIndexLocation = liquidRitem->Geo->DrawArgs["liquidGeo"].StartIndexLocation;
		liquidRitem->BaseVertexLocation = liquidRitem->Geo->DrawArgs["liquidGeo"].BaseVertexLocation;

		mRitemLayer[(int)RenderLayer::Opaque].push_back(liquidRitem.get());
		mAllRitems.push_back(std::move(liquidRitem));
	}
}