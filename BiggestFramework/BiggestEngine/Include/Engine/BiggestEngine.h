#pragma once

#include <Windows.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dCompiler.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <d3dx11effect.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>
#include <crtdbg.h>
#include <conio.h>

#include <algorithm>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <unordered_map>

// Include Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API
#endif



#pragma comment(lib, "assimp-vc120-mtd.lib")


#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )


// Effect Framework를 사용하기 위한 lib
#ifdef BIGGEST_IMPORT
#pragma comment( lib, "Directx11Common/Effects11d.lib")
#else
#pragma comment( lib, "Effects11d.lib")
#endif

//#pragma comment(lib, "libfbxsdk.lib")
//#pragma comment(lib, "libfbxsdk-md.lib")
//libfbxsdk-md.lib 로 해주어야 FbxMaterial을 사용할 수 있다.

using namespace std;



#define BIGGEST_BEGIN namespace BiggestEngine {
#define BIGGEST_END }
#define USING_BIGGEST using namespace BiggestEngine;



#ifdef BIGGEST_IMPORT
#define BIGGEST_DLL __declspec(dllimport)
#else
#define BIGGEST_DLL __declspec(dllexport)
#endif


#include "BiggestValue.h"
