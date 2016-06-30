#pragma once

#include <windows.h>
#include <string>
#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "Exception.h"
#include "GameClock.h"
#include "GameTime.h"
#include "DDSTextureLoader.h"

//Rendering related systems
#include <d3d11_1.h>
//Stay in PCH
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <WICTextureLoader.h>

//Yea...No.
using namespace std;
using namespace DirectX;
using namespace PackedVector;

//Shared Macros - Might Remain in PCH.
#define DeleteObject(object)   if((object)  != nullptr)  { delete object; object = nullptr; }
#define DeleteObjects(objects) if((objects) != nullptr)  { delete[] objects; objects = nullptr; }
#define ReleaseObject(object)  if((object)  != nullptr)  { object->Release(); object = nullptr; }
#define ZeroMem(object)  ZeroMemory(&object, sizeof(object));

//Shared TypeDefs
typedef unsigned char byte;