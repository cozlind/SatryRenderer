#include "stdafx.h"
#include "Poly.h"
Poly::Poly() : state(POLY_STATE_ACTIVE), attribute(0), matIndex(0)
{
	color = 0;
}

Poly::Poly(Vector4* vl, int va, int vb, int vc, Vector4* nl, int na, int nb, int nc, Vector2* uvl, int uva, int uvb, int uvc) :
state(POLY_STATE_ACTIVE), attribute(0), matIndex(0), vList(vl), normList(nl), uvList(uvl)
{
	vert[0] = va;
	vert[1] = vb;
	vert[2] = vc;
	norm[0] = na;
	norm[1] = nb;
	norm[2] = nc;
	uv[0] = uva;
	uv[1] = uvb;
	uv[2] = uvc;
	color = 0;
}


Poly::~Poly()
{
}
