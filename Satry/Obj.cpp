#include "stdafx.h"
#include "Obj.h"

static int idCount = 0;
Obj::Obj() :numNorm(0), numUV(0), numPolys(0), numVertices(0), id(idCount++), 
state(0), attribute(SHADE_MODE_PURE), avgRadius(0), maxRadius(0)
{
	strcpy_s(name, "Untitle");
	ux = Vector4(1, 0, 0);
	uy = Vector4(0, 1, 0);
	uz = Vector4(0, 0, 1);
}

Obj::Obj(char n[], float ar, float mr, Vector4 d, int nv, Vertex vs[], int np, int polyIndex[]) :id(idCount++), state(0), attribute(SHADE_MODE_PURE), avgRadius(ar), maxRadius(mr), numVertices(nv), numPolys(np)
{
	strcpy_s(name,n);
	//for (int i = 0; i < nv; i++) vList[i]=localVList[i] = vs[i];
	//for (int i = 0; i < np; i++) pList[i] = Poly(vList, polyIndex[i * 3 + 0], polyIndex[i * 3 + 1], polyIndex[i * 3 + 2]);
}

Obj::~Obj()
{
}
