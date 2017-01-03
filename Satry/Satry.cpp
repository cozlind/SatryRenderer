#include "stdafx.h"
#include "Satry.h"
#include <fstream>
using namespace std;
//���Ķ���
int keys[512];
bool isLockKey = false;
Transform transform;
Render render;
Camera camera(Vector4(0, 0, -10));
int rMode[] = { RENDER_MODE_COLOR, RENDER_MODE_WIREFRAME, RENDER_MODE_TEXTURE };
int renderMode = RENDER_MODE_COLOR;
int sMode[] = { SHADE_MODE_FLAT, SHADE_MODE_GOURAUD };
int shaderMode = SHADE_MODE_GOURAUD;
int index1 = 0, index2 = 0;
float theta = 0;
bool isPrint = true;
FileIO fileIO;
CubeMap cubemap;
int numObj = 0;
Obj *objList = new Obj[10];//�����б�
int numMat = 0;
Material matList[10];//���ʿ�
Material normMat;
Vector4 nullV4(0, 0, 0);
int mouseDownX, mouseDownY;
Vector4 mouseDownU;
Vector4 mouseDownV;
Vector4 mouseDownN;
Vector4 mouseDownAt;
Vector4 mouseDownPos;
int mouseWheel;
int numLight = 4;
bool showInstruction = true;

Light lights[] = { Light(0, LIGHT_STATE_ON, LIGHT_ATTR_AMBIENT, Vector4(255, 255, 255, 0), nullV4, nullV4),
					Light(1, LIGHT_STATE_ON, LIGHT_ATTR_INFINITE, Vector4(255, 10,10, 0), nullV4, Vector4(0, -1, 0, 0)),
					Light(2, LIGHT_STATE_ON, LIGHT_ATTR_POINT, Vector4(0, 255, 255, 0), Vector4(0, 10000, 0), nullV4, 0, 0.0001, 0),
					Light(3, LIGHT_STATE_ON, LIGHT_ATTR_SPOTLIGHT1, Vector4(255, 255, 150, 0), Vector4(100, 100, -100, 0),Vector4(-1, -1, 1, 0), 0.0003, 0.0006, 0.00001) };

HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	//DEBUG
	AllocConsole();   // ��������̨
	SetConsoleTitle(_T("Debug Output"));      // ���ÿ���̨���ڱ���
	// �ض��� STDIN
	{
		FILE*fp;
		errno_t eno = freopen_s(&fp, "CONIN$", "r+t", stdin);
		if (eno != 0)
		{//���ɹ�
			MessageBox(NULL, _T("�ض���stdinʧ��!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
			exit(0);
		}
	}
	// �ض���STDOUT
	{
		FILE*fp;
		errno_t eno = freopen_s(&fp, "CONOUT$", "w+t", stdout);
		if (eno != 0)
		{//���ɹ�
			MessageBox(NULL, _T("�ض���stdoutʧ��!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
			exit(0);
		}

	}
	//

	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SATRY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SATRY));

	// ����Ϣѭ��: 
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// main game processing goes here
		EngineMain();
	}

	EngineShutdown();
	//DEBUG
	FreeConsole();//���ٿ���̨
	//
	return (int)msg.wParam;
}
//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SATRY));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SATRY);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	initHWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!initHWnd)
	{
		return FALSE;
	}
	//��ȡ�û�����Ĵ�С
	RECT rect;
	GetClientRect(initHWnd, &rect);
	width = rect.right;
	height = rect.bottom;
	////DEBUG
	//cout << "----clientRect:" << endl;
	//cout << "rect.bottom:" << rect.bottom << endl;
	//cout << "rect.left:" << rect.left << endl;
	//cout << "rect.right:" << rect.right << endl;
	//cout << "rect.top:" << rect.top << endl;
	////

	EngineInit();

	ShowWindow(initHWnd, nCmdShow);
	UpdateWindow(initHWnd);

	return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
inline void OpenOBJFile(HINSTANCE hInstance, HWND hWnd,char* fileName,char* filePath)
{
	OPENFILENAME ofn;
	char* szFile = new char[512];
	char* szFileTitle = new char[512];
	memset(&ofn, 0, sizeof(ofn));
	memset(szFile, 0, sizeof(char) * 512);
	memset(szFileTitle, 0, sizeof(char) * 512);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = initHWnd;
	ofn.hInstance = hInstance;
	ofn.lpstrFilter = "OBJ File\0*.obj*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFileTitle;
	ofn.nMaxFile = sizeof(char) * 512;
	ofn.lpstrFileTitle = szFile;
	ofn.nMaxFileTitle = sizeof(char) * 512;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

	BOOL ok = GetOpenFileName(&ofn);
	strcpy_s(filePath,512, szFileTitle);
	strcpy_s(fileName, 512, szFile);
	filePath[strlen(filePath) - strlen(fileName)] = '\0';
	delete[] szFile;
	delete[] szFileTitle;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case ID_32772://����
			render.ResetFlag();
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(filePath, "ee.obj");
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
			break;
		case ID_32773://���������
			render.ResetFlag();
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(filePath, "cube.obj");
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
			break;
		case ID_32774://����������
			render.ResetFlag();
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(filePath, "ccube.obj");
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
			break;
		case ID_32775://���
		{
			char file[512],path[512];
			OpenOBJFile(hInst, hWnd, file, path);
			render.ResetFlag();
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(path, file);
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
			break;
		}
		case ID_32777://�㶨��ɫ
			render.ResetFlag();
			renderMode = RENDER_MODE_COLOR;
			shaderMode = SHADE_MODE_FLAT;
			break;
		case ID_32778://GOURAUD��ɫ
			render.ResetFlag();
			renderMode = RENDER_MODE_COLOR;
			shaderMode = SHADE_MODE_GOURAUD;
			break;
		case ID_32779://GOURAUD��ɫ
			render.ResetFlag();
			renderMode = RENDER_MODE_WIREFRAME;
			shaderMode = SHADE_MODE_FLAT;
			break;
		case ID_32786://ӡ����ͼ
			render.ResetFlag();
			renderMode = RENDER_MODE_TEXTURE;
			shaderMode = SHADE_MODE_GOURAUD;
			break;
		case ID_32787://������ͼ
			render.ResetFlag();
			renderMode = RENDER_MODE_TEXTURE;
			shaderMode = SHADE_MODE_GOURAUD;
			render.showCubeMap = !render.showCubeMap;
			break;
		case ID_32789://������ͼ
			render.ResetFlag();
			renderMode = RENDER_MODE_TEXTURE;
			shaderMode = SHADE_MODE_GOURAUD;
			render.showNormMap = !render.showNormMap;
			break;
		case ID_32794://���
			render.ResetFlag();
			renderMode = RENDER_MODE_TEXTURE;
			shaderMode = SHADE_MODE_GOURAUD;
			render.showNormMap = true;
			break;
		case ID_32781://������
			lights[0].state = lights[0].state == LIGHT_STATE_ON ? LIGHT_STATE_OFF : LIGHT_STATE_ON;
			break;
		case ID_32782://ƽ�й�
			lights[1].state = lights[1].state == LIGHT_STATE_ON ? LIGHT_STATE_OFF : LIGHT_STATE_ON;
			break;
		case ID_32783://���Դ
			lights[2].state = lights[2].state == LIGHT_STATE_ON ? LIGHT_STATE_OFF : LIGHT_STATE_ON;
			break;
		case ID_32784://�۹��
			lights[3].state = lights[3].state == LIGHT_STATE_ON ? LIGHT_STATE_OFF : LIGHT_STATE_ON;
			break;
		case ID_32798:
			for (int i = 0; i < 4; i++)
			{
				lights[i].state = LIGHT_STATE_ON;
			}
			break;
		case ID_32799:
			for (int i = 0; i < 4; i++)
			{
				lights[i].state = LIGHT_STATE_OFF;
			}
			break;
		case ID_32791:
			render.enableZBuffer = !render.enableZBuffer;
			break;
		case ID_32792:
			render.showBackFace = !render.showBackFace;
			break;
		case ID_32800:
			render.ResetFlag();
			break;
		case ID_32793:
			render.showBackFace = true;
			render.showAlpha = !render.showAlpha;
			break;
		case ID_32796:
			camera.pos = Vector4(0, 0, -10);
			camera.at = Vector4(0, 0, 0);
			camera.up = Vector4(0, 1, 0);
			break;
		case ID_32797:
			showInstruction = !showInstruction;
			break;

		}
		break;
	}
	case WM_KEYDOWN:
		keys[wParam & 511] = 1;
		break;
	case WM_KEYUP:
		keys[wParam & 511] = 0;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{
		POINT cursor;
		GetCursorPos(&cursor);
		mouseDownX = cursor.x;
		mouseDownY = cursor.y;
		mouseDownU = camera.u;
		mouseDownV = camera.v;
		mouseDownN = camera.n;
		mouseDownAt = camera.at;
		mouseDownPos = camera.pos;
		keys[wParam & 511] = 1;
		break;
	}
	case WM_LBUTTONUP:
		keys[VK_LBUTTON] = 0;
		break;
	case WM_RBUTTONDOWN:
	{
		POINT cursor;
		GetCursorPos(&cursor);
		mouseDownX = cursor.x;
		mouseDownY = cursor.y;
		mouseDownU = camera.u;
		mouseDownV = camera.v;
		mouseDownN = camera.n;
		mouseDownAt = camera.at;
		mouseDownPos = camera.pos;
		keys[VK_RBUTTON] = 1;
		if (wParam & MK_SHIFT)
		{
			keys[VK_SHIFT] = 1;
		}
		else 
		{
			keys[VK_SHIFT] = 0;
		}
		break;
	}
	case WM_RBUTTONUP:
		keys[VK_RBUTTON] = 0;
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_SHIFT)
		{
			keys[VK_SHIFT] = 1;
		}
		else
		{
			keys[VK_SHIFT] = 0;
		}
	case WM_MOUSEWHEEL:
		mouseWheel = HIWORD(wParam) > 30000 ? HIWORD(wParam)-65535  : HIWORD(wParam);
		keys[VK_MBUTTON] = 1;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//Every thing start from here
void EngineInit(void *params, int num_params)
{
	memset(keys, 0, sizeof(int) * 512);
	BITMAPINFO bmpInfo = { { sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, width * height * 4, 0, 0, 0, 0 } };
	hBDC = CreateCompatibleDC(NULL);//����һ�������ڵ�ǰ�豸���ڴ�DC
	HDC hDC = GetDC(initHWnd);//�ͻ������DC
	LPVOID p;
	hBmpMem = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &p, 0, 0);//����λͼ
	SelectObject(hBDC, hBmpMem);//��λͼװ���ڴ�DC //hPreBmp = 
	ReleaseDC(initHWnd, hDC);
	//framebufferһάָ��+zbufferһάָ��+framebuffer����+zbuffer����
	int spaceSize = sizeof(void*)*height * 2 + width*height * (sizeof(UINT32*) + sizeof(float*));
	char* ptr = (char*)malloc(spaceSize);
	frameBuffer = (UINT32**)ptr;
	zBuffer = (float**)(ptr + sizeof(void*)*height);
	ptr += 2 * height * sizeof(void*);
	//char����ʹ��ָ������ʱ����1�ֽڵĲ�����pΪ�ⲿ֡���棬����ǿգ���ʹ��p��Ϊ�վ�ʹ���Լ�������ڴ�ptr��
	char* memPtr = (p != NULL) ? (char*)p : ptr;
	ptr += width*height * sizeof(UINT32);
	for (int i = 0; i < height; i++)
	{
		//����������ÿһ�к�frameBuffer���һάָ�����������ӣ�ʹ��frameBuffer��Ϊһ�������Ķ�ά����
		frameBuffer[i] = (UINT32*)(memPtr + i*width * sizeof(UINT32));
		zBuffer[i] = (float*)(ptr + i*width * sizeof(float));
	}
	render.Init();
	camera.Init();
	cubemap.ReadCubeMap();
	fileIO.readBitmap(".\\model\\normal.bmp", normMat);
	fileIO.OpenObjFile(filePath, "ee.obj");
	numMat = fileIO.LoadAllMat(matList);
	while (fileIO.LoadObjModel(objList[numObj++], matList));
	fileIO.CloseFile();
}
inline void inputProc()
{
	if (keys[VK_ESCAPE])
		SendMessage(initHWnd, WM_CLOSE, 0, 0);
	if (keys[VK_LBUTTON]) {
		POINT cursor;
		GetCursorPos(&cursor);
		double l = sqrt((cursor.x - mouseDownX)*(cursor.x - mouseDownX) + (cursor.y - mouseDownY)*(cursor.y - mouseDownY));
		if (l < 90)
		{
			float ratio = 150;
			float rotateX = (cursor.x - mouseDownX) / ratio;
			float rotateY = (cursor.y - mouseDownY) / ratio;
			rotateX = rotateX >6.283 ? rotateX - (int)(rotateX / 6.283)*6.283 : rotateX;
			rotateY = rotateY > 6.283 ? rotateY - (int)(rotateY / 6.283)*6.283 : rotateY;
			rotateX = rotateX <0 ? rotateX - (int)(rotateX / 6.283)*6.283 + 6.283 : rotateX;
			rotateY = rotateY <0 ? rotateY - (int)(rotateY / 6.283)*6.283 + 6.283 : rotateY;
			camera.Rotate(mouseDownN, mouseDownU, mouseDownV, mouseDownPos, mouseDownAt, rotateX, rotateY);
		}
	}
	else if (keys[VK_RBUTTON])
	{
		POINT cursor;
		GetCursorPos(&cursor);
		double l = sqrt((cursor.x - mouseDownX)*(cursor.x - mouseDownX) + (cursor.y - mouseDownY)*(cursor.y - mouseDownY));
		float ratio = 100;
		float rotateX = (cursor.x - mouseDownX) / ratio;
		float rotateY = (cursor.y - mouseDownY) / ratio;
		rotateX = rotateX > 6.283 ? rotateX - (int)(rotateX / 6.283)*6.283 : rotateX;
		rotateY = rotateY > 6.283 ? rotateY - (int)(rotateY / 6.283)*6.283 : rotateY;
		rotateX = rotateX < 0 ? rotateX - (int)(rotateX / 6.283)*6.283 + 6.283 : rotateX;
		rotateY = rotateY < 0 ? rotateY - (int)(rotateY / 6.283)*6.283 + 6.283 : rotateY;
		if (keys[VK_SHIFT])
			camera.RotateFocusY(mouseDownN, mouseDownU, mouseDownV, mouseDownPos, mouseDownAt, rotateY);
		else
			camera.RotateFocus(mouseDownN, mouseDownU, mouseDownV, mouseDownPos, mouseDownAt, rotateX, 0);
	}
	else if (keys[VK_MBUTTON]) {
		camera.pos = camera.pos + camera.n*mouseWheel*0.01;
		camera.at = camera.at + camera.n*mouseWheel*0.01;
		keys[VK_MBUTTON] = 0;
	}
	if (keys[VK_DOWN])
	{
		camera.pos = camera.pos - camera.v*0.09;
		camera.at = camera.at - camera.v*0.09;
	}
	if (keys[VK_UP])
	{
		camera.pos = camera.pos + camera.v*0.09;
		camera.at = camera.at + camera.v*0.09;
	}
	if (keys[VK_LEFT])
	{
		camera.pos = camera.pos + camera.u*0.09;
		camera.at = camera.at + camera.u*0.09;
	}
	if (keys[VK_RIGHT])
	{
		camera.pos = camera.pos -camera.u*0.09;
		camera.at = camera.at - camera.u*0.09;
	}
	if (keys[VK_F1])
	{
		if (!isLockKey)
		{
			isLockKey = true;
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(filePath, "ee.obj");
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
		}
	}
	else if (keys[VK_F2])
	{
		if (!isLockKey)
		{
			isLockKey = true;
			fileIO.Reset();
			objList = new Obj[10];
			fileIO.OpenObjFile(filePath, "ccube.obj");
			numMat = fileIO.LoadAllMat(matList);
			while (fileIO.LoadObjModel(objList[numObj++], matList));
			fileIO.CloseFile();
		}
	}
	else if (keys[VK_SPACE])
	{
		if (!isLockKey)
		{
			isLockKey = true;
			camera.pos = Vector4(0, 0, -10);
			camera.at = Vector4(0, 0, 0);
			camera.up = Vector4(0, 1, 0);
		}
	}
	else if (keys[65])//a
	{
		if (!isLockKey)
		{
			isLockKey = true;
			shaderMode = sMode[index2++];
			index2 %= 2;
		}
	}
	else if (keys[66])//b
	{
		if (!isLockKey)
		{
			isLockKey = true;
			render.showBackFace = !render.showBackFace;
		}
	}
	else if (keys[71])//g
	{
		if (!isLockKey)
		{
			isLockKey = true;
			render.showCubeMap = !render.showCubeMap;
		}
	}
	else if (keys[72])//h
	{
		if (!isLockKey)
		{
			isLockKey = true;
			render.showNormMap = !render.showNormMap;
		}
	}
	else if (keys[82])//r
	{
		if (!isLockKey)
		{
			isLockKey = true;
			render.showAlpha = !render.showAlpha;
		}
	}
	else if (keys[90])//z
	{
		if (!isLockKey)
		{
			isLockKey = true;
			render.enableZBuffer = !render.enableZBuffer;
		}
	}
	else if (keys[83])//s
	{
		if (!isLockKey)
		{
			isLockKey = true;
			lights[0].state++;
			lights[0].state %= 2;
			printf("LIGHT_ATTR_AMBIENT\n");
		}
	}
	else if (keys[87])//w
	{
		if (!isLockKey)
		{
			isLockKey = true;
			lights[1].state++;
			lights[1].state %= 2;
			printf("LIGHT_ATTR_INFINITE\n");
		}
	}
	else if (keys[68])//d
	{
		if (!isLockKey)
		{
			isLockKey = true;
			lights[2].state++;
			lights[2].state %= 2;
			printf("LIGHT_ATTR_POINT\n");
		}
	}
	else if (keys[81])//q
	{
		if (!isLockKey)
		{
			isLockKey = true;
			lights[3].state++;
			lights[3].state %= 2;
			printf("LIGHT_ATTR_SPOTLIGHT1\n");
		}
	}
	else
	{
		isLockKey = false;
	}
}
//����ѭ����
void EngineMain(void *params, int num_params)
{
	DWORD startTime = GetTickCount64();
	render.Clear();

	//����
	inputProc();

	//��ͼ�任
	camera.SetViewTransform();
	camera.SetProjectionTransform();
	render.DrawObj(objList, theta, camera);

	char fps[10];
	if (GetTickCount64() > startTime)
		sprintf_s(fps, "FPS:%d", 1000 / (GetTickCount64() - startTime));
	else sprintf_s(fps, "FPS:INF");
	Sleep(1);
	//��Ⱦ
	HDC hDC = GetDC(initHWnd);
	TextOut(hBDC, 50, 30, fps, strlen(fps));
	if (showInstruction)
	{
		char instruction1[100] = "Press SPACE to reset the camera";
		char instruction2[100] = "Press A to switch the shader mode";
		char instruction3[100] = "Press B to switch the back face display";
		char instruction4[100] = "Press R to switch the alpha display";
		char instruction5[100] = "Press S/W/D/Q to control the ambient light, infinite light, point light and spotlight";
		char instruction6[100] = "Press G to switch the cubemap";
		char instruction7[100] = "Press H to switch the normalMap";
		TextOut(hBDC, 50, 50, instruction1, strlen(instruction1));
		TextOut(hBDC, 50, 70, instruction2, strlen(instruction2));
		TextOut(hBDC, 50, 90, instruction3, strlen(instruction3));
		TextOut(hBDC, 50, 110, instruction4, strlen(instruction4));
		TextOut(hBDC, 50, 130, instruction5, strlen(instruction5));
		TextOut(hBDC, 50, 150, instruction6, strlen(instruction6));
		TextOut(hBDC, 50, 170, instruction7, strlen(instruction7));
	}

	BitBlt(hDC, 0, 0, width, height, hBDC, 0, 0, SRCCOPY);//���ڴ�󻺳�����λͼ������ʾ������װ��ǰ������
	ReleaseDC(initHWnd, hDC);
}
void EngineShutdown(void *params, int num_params)
{
	free(frameBuffer);
	frameBuffer = NULL;
	zBuffer = NULL;

	DeleteDC(hBDC);
	DeleteObject(hBmpMem);
}