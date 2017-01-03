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

//ģ���ļ�
const char filePath[100] = ".\\model\\";
const char fileName[50] = "ee.obj";

// �������: 
HWND initHWnd = NULL;							// ��ʼ���ھ��
LONG width;										// ��ʼ���ڿ��
LONG height;									// ��ʼ���ڸ߶�

//�ڴ����
HDC hBDC = NULL;								// �󻺳��������ڴ���
HBITMAP hBmpMem;
HBITMAP hPreBmp;
UINT32 **frameBuffer = NULL;			// frame buffer ֡�����������ػ���������ָ��
float **zBuffer = NULL;					// z buffer ��Ȼ�������ָ��


void EngineInit(void *params = NULL, int num_params = 0);
void EngineMain(void *params = NULL, int num_params = 0);
void EngineShutdown(void *params = NULL, int num_params = 0);