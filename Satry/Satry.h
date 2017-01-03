#pragma once

#include "Resource.h"

#include "Transform.h"
#include "Render.h"
#include "Matrix.h"
#include "Vertex.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Camera.h"
#include "FileIO.h"
#include "Material.h"
#include "Light.h"
#include "CubeMap.h"

#define MAX_LOADSTRING 100
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

//模型文件
const char filePath[100] = ".\\model\\";
const char fileName[50] = "ee.obj";

// 窗口相关: 
HWND initHWnd = NULL;							// 初始窗口句柄
LONG width;										// 初始窗口宽度
LONG height;									// 初始窗口高度

//内存相关
HDC hBDC = NULL;								// 后缓冲区，在内存中
HBITMAP hBmpMem;
HBITMAP hPreBmp;
UINT32 **frameBuffer = NULL;			// frame buffer 帧缓冲区（像素缓冲区）的指针
float **zBuffer = NULL;					// z buffer 深度缓冲区的指针


void EngineInit(void *params = NULL, int num_params = 0);
void EngineMain(void *params = NULL, int num_params = 0);
void EngineShutdown(void *params = NULL, int num_params = 0);