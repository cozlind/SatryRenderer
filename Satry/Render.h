#pragma once
#include "Vertex.h"
#include "Obj.h"
#include "Triangle.h"
#include "Camera.h"
#include "CubeMap.h"

#define RENDER_MODE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_MODE_TEXTURE        2		// ��Ⱦ����
#define RENDER_MODE_COLOR          4		// ��Ⱦ��ɫ
#define RENDERLIST_MAX_POLYS		200
extern LONG width;										// ��ʼ���ڿ��
extern LONG height;	
extern UINT32 **frameBuffer;			// frame buffer ֡�����������ػ���������ָ��
extern float **zBuffer;					// z buffer ��Ȼ�������ָ��

class Render
{
public:
	int minClipX;
	int maxClipX;
	int minClipY;
	int maxClipY;
	//��Ⱦ�б�
	int state;
	int attribute;
	int numPolys;

	bool showBackFace;
	bool showAlpha;
	bool showCubeMap;
	bool showNormMap;
	bool enableZBuffer;

	Triangle triDataList[RENDERLIST_MAX_POLYS];
	Triangle* triPtrList[RENDERLIST_MAX_POLYS];

	Render();
	~Render();
	void ResetFlag();
	void Init();
	void Clear();
	void DrawPixel(int x, int y, UINT32 color);
	void DrawLine(int x1, int y1, int x2, int y2, UINT32 color=0xffffff);
	void DrawTriangle(Poly&);
	void DrawObj(Obj*,float,Camera&);
	void CirclePlot(int xc, int yc, int x, int y);
	void DrawCircle(int xc, int yc, int r);
	int CVVCheck(const Vector4& v)const;
private:
	void DrawTopTri(Vector4 v1, Vector4 c1, Vector2 uv1, float rhw1, Vector4 r1,
		Vector4 v2, Vector4 c2, Vector2 uv2, float rhw2, Vector4 r2,
		Vector4 v3, Vector4 c3, Vector2 uv3, float rhw3, Vector4 r3, Poly& poly);
	void DrawBottomTri(Vector4 v1, Vector4 c1, Vector2 uv1, float rhw1, Vector4 r1,
		Vector4 v2, Vector4 c2, Vector2 uv2, float rhw2, Vector4 r2,
		Vector4 v3, Vector4 c3, Vector2 uv3, float rhw3, Vector4 r3, Poly& poly);
};

