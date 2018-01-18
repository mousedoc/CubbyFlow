//--------------------------------------------------------------------------------------
// File: Tutorial04.cpp
//
// This application displays a 3D cube using Direct3D 11
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#define NOMINMAX

#include <limits>
#include <fstream>
#include <string>
#include <vector>

#include <Array/Array2.h>
#include <Collider/RigidBodyCollider3.h>
#include <Emitter/VolumeGridEmitter3.h>
#include <Geometry/Box3.h>
#include <Geometry/Cylinder3.h>
#include <Geometry/ImplicitTriangleMesh3.h>
#include <Geometry/Plane3.h>
#include <Geometry/Sphere3.h>
#include <Geometry/TriangleMesh3.h>
#include <Grid/ScalarGrid3.h>
#include <Math/MathUtils.h>
#include <MarchingCubes/MarchingCubes.h>
#include <Solver/LevelSet/LevelSetLiquidSolver3.h>
#include <Surface/Implicit/ImplicitSurfaceSet3.h>
#include <Utils/Logger.h>
#include <Utils/Timer.h>

#include <pystring/pystring.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "Resource.h"

#ifdef CUBBYFLOW_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <getopt.h>

#define APP_NAME L"DirectX"

using namespace CubbyFlow;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pConstantBuffer = NULL;
XMMATRIX                g_World;
XMMATRIX                g_View;
XMMATRIX                g_Projection;
XMVECTOR				g_Eye =	XMVectorSet(0.f, 3.0f, -4.8f, 0.0f);


ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
ID3D11RasterizerState*	g_pRasterizerState = NULL;

TriangleMesh3			g_mesh;
int						g_frameCnt = 0;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"LevelSetLiquidSim", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{

    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial04.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        return hr;
	}

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();
	if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial04.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
	pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Create vertex buffer

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(SimpleVertex) * 10000;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	SimpleVertex* liquidVertices = new SimpleVertex[10000];
	WORD* indices = new WORD[50000];

	D3D11_SUBRESOURCE_DATA vBufferData;
	vBufferData.pSysMem = liquidVertices;
	vBufferData.SysMemPitch = 0;
	vBufferData.SysMemSlicePitch = 0;

	hr = g_pd3dDevice->CreateBuffer(&bufDesc, &vBufferData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	D3D11_BUFFER_DESC ibufDesc;
	
	ibufDesc.Usage = D3D11_USAGE_DYNAMIC;
	ibufDesc.ByteWidth = sizeof(WORD) * 50000;
	ibufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibufDesc.MiscFlags = 0;
	ibufDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA iBufferData;
	
	iBufferData.pSysMem = indices;
	iBufferData.SysMemPitch = 0;
	iBufferData.SysMemSlicePitch = 0;

    hr = g_pd3dDevice->CreateBuffer( &ibufDesc, &iBufferData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the constant buffer
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(ConstantBuffer);
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bufDesc, NULL, &g_pConstantBuffer );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrix
	g_World = XMMatrixIdentity();

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -5.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f );

	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_NONE; 
	RSDesc.DepthBias = 0;                      
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = FALSE;            
	RSDesc.ScissorEnable = FALSE;             
	RSDesc.MultisampleEnable = FALSE;         
	RSDesc.AntialiasedLineEnable = FALSE;        

	g_pd3dDevice->CreateRasterizerState(&RSDesc, &g_pRasterizerState);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
		case WM_KEYDOWN:
			switch (wParam) {
			case 'W':
				g_Eye += XMVectorSet(1, 0, 0, 0);
				break;
			case 'S':
				g_Eye += XMVectorSet(-1, 0, 0, 0);
				break;
			}
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

void UpdateBuffer() {
	D3D11_MAPPED_SUBRESOURCE mappedVertexResource, mappedIndexResource;
	ZeroMemory(&mappedVertexResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&mappedIndexResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	int numberOfPoints = g_mesh.NumberOfPoints();

	SimpleVertex *vertices = new SimpleVertex[numberOfPoints];

	for (int i = 0; i < numberOfPoints; i++) {
		vertices[i].Pos.x = g_mesh.Point(i).x;
		vertices[i].Pos.y = g_mesh.Point(i).y;
		vertices[i].Pos.z = g_mesh.Point(i).z;
		vertices[i].Color = { 0.52f, 0.80f, 0.92f, 1.0f };
	}

	int numberOfTriangles = g_mesh.NumberOfTriangles();
	WORD *indices = new WORD[3 * numberOfTriangles];

	for (int i = 0; i < numberOfTriangles; i++) {
		indices[i * 3] = g_mesh.PointIndex(i).x;
		indices[i * 3 + 1] = g_mesh.PointIndex(i).y;
		indices[i * 3 + 2] = g_mesh.PointIndex(i).z;
	}

	g_pImmediateContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	g_pImmediateContext->Map(g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexResource);

	D3D11_BUFFER_DESC desc;
	g_pIndexBuffer->GetDesc(&desc);
	
	memcpy(mappedVertexResource.pData, vertices, numberOfPoints * sizeof(SimpleVertex));
	memcpy(mappedIndexResource.pData, indices, sizeof(WORD) * numberOfTriangles * 3);

	g_pImmediateContext->Unmap(g_pVertexBuffer, 0);
	g_pImmediateContext->Unmap(g_pIndexBuffer, 0);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	int numberOfFrames = 30;
	char baseName[256];
	
    // Update our time
    static float t = 0.0f;
	static int frameCnt = 0;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if( dwTimeStart == 0 )
		dwTimeStart = dwTimeCur;
	t = ( dwTimeCur - dwTimeStart ) / 1000.0f;

	float timeIntervalOfFrame = (float)1 / numberOfFrames;
	
	if (t > timeIntervalOfFrame) {
		t -= timeIntervalOfFrame;
		frameCnt++;

		char baseName[256];
		snprintf(baseName, sizeof(baseName), "frame_%06d.obj", frameCnt);
		std::ifstream file(baseName);
		g_mesh = TriangleMesh3();
		g_mesh.ReadObj(&file);

		UpdateBuffer();
	}

	// 
	// My Camera
	//

	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(g_Eye, At, Up);

    //
    // Animate the cube
    //
	g_World = XMMatrixRotationY( 0 );

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // red,green,blue,alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose( g_World );
	cb.mView = XMMatrixTranspose( g_View );
	cb.mProjection = XMMatrixTranspose( g_Projection );
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cb, 0, 0 );

    //
    // Renders a triangle
    //
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
	g_pImmediateContext->DrawIndexed( g_mesh.NumberOfPoints(), 0, 0 );        // 36 vertices needed for 12 triangles in a triangle list
	
    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}

