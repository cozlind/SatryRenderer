#include "stdafx.h"
#include "Vertex.h"
#define INTERP(x1, x2,t) (x1 + (x2 - x1) * t)

Vertex::Vertex()
{
	rhw = pos.w == 0 ? 0 : 1 / pos.w;
}

Vertex::Vertex(Vector4 v1, Vector4 v2, Vector4 v3, Vector2 v4, float r) :pos(v1), color(v2), norm(v3), texCoord(v4), rhw(r)
{
	if (rhw == 0)
	{
		rhw = pos.w == 0 ? 0 : 1 / pos.w;
	}
}
void Vertex::setTexCoord(float u, float v)
{
	texCoord.x = u;
	texCoord.y = v;
}
void Vertex::RhwInit()
{
	rhw = 1 / pos.w;
	/*texCoord.u *= rhw;
	texCoord.v *= rhw;*/
	color.x *= rhw;
	color.y *= rhw;
	color.z *= rhw;
}
Vertex Vertex::Division(const Vertex& v1, const Vertex& v2, float div)
{
	float factor = 1 / div;
	return Vertex(Vector4((v2.pos.x - v1.pos.x) * factor, (v2.pos.y - v1.pos.y) * factor, (v2.pos.z - v1.pos.z) * factor, (v2.pos.w - v1.pos.w) * factor),
		Vector4((v2.color.x - v1.color.x) * factor, (v2.color.y - v1.color.y) * factor, (v2.color.z - v1.color.z) * factor, (v2.color.w - v1.color.w) * factor),
		Vector4((v2.norm.x - v1.norm.x) * factor, (v2.norm.y - v1.norm.y) * factor, (v2.norm.z - v1.norm.z) * factor, (v2.norm.w - v1.norm.w) * factor),
		Vector2((v2.texCoord.x - v1.texCoord.x) * factor, (v2.texCoord.y - v1.texCoord.y) * factor),
		(v2.rhw - v1.rhw) * factor);
}
Vertex Vertex::Interpolate(const Vertex& v1, const Vertex& v2, float param)
{
	return Vertex(Vector4(INTERP(v1.pos.x, v2.pos.x, param), INTERP(v1.pos.y, v2.pos.y, param), INTERP(v1.pos.z, v2.pos.z, param), 1),
		Vector4(INTERP(v1.color.x, v2.color.x, param), INTERP(v1.color.y, v2.color.y, param), INTERP(v1.color.z, v2.color.z, param), 1),
		Vector4(INTERP(v1.norm.x, v2.norm.x, param), INTERP(v1.norm.y, v2.norm.y, param), INTERP(v1.norm.z, v2.norm.z, param), 1),
		Vector2(INTERP(v1.texCoord.x, v2.texCoord.x, param), INTERP(v1.texCoord.y, v2.texCoord.y, param)),
		INTERP(v1.rhw, v2.rhw, param));
}
inline void swap(float& a, float& b)
{
	float t = a;
	a = b;
	b = t;
}
void Vertex::Swap(Vertex& v1, Vertex& v2)
{
	swap(v1.pos.x, v2.pos.x);
	swap(v1.pos.y, v2.pos.y);
	swap(v1.pos.z, v2.pos.z);
	swap(v1.pos.w, v2.pos.w);
	swap(v1.color.x, v2.color.x);
	swap(v1.color.y, v2.color.y);
	swap(v1.color.z, v2.color.z);
	swap(v1.color.w, v2.color.w);
	swap(v1.norm.x, v2.norm.x);
	swap(v1.norm.y, v2.norm.y);
	swap(v1.norm.z, v2.norm.z);
	swap(v1.norm.w, v2.norm.w);
	swap(v1.texCoord.x, v2.texCoord.x);
	swap(v1.texCoord.y, v2.texCoord.y);
	swap(v1.rhw, v2.rhw);
}
UINT32 Vertex::GetColor()const
{
	float factor = 1 / rhw;
	int r = (int)(color.x * factor*255.0f);
	int g = (int)(color.y * factor* 255.0f);
	int b = (int)(color.z * factor*255.0f);
	return (r << 16) | (g << 8) | b;
}
Vertex Vertex::operator + (const Vertex& right) const
{
	return Vertex(pos + right.pos, color + right.color, norm + right.norm, texCoord + right.texCoord, rhw + right.rhw);
}
Vertex Vertex::operator * (const float right) const
{
	return Vertex(Vector4(pos.x *right, pos.y*right, pos.z*right, pos.w*right),
		Vector4(color.x*right, color.y*right, color.z*right, color.w *right),
		Vector4(norm.x*right, norm.y*right, norm.z*right, norm.w*right),
		Vector2(texCoord.x*right, texCoord.y*right),
		rhw*right);
}
Vertex& Vertex::operator = (const Vertex& right)
{
	pos = right.pos;
	color = right.color;
	norm = right.norm;
	texCoord = right.texCoord;
	rhw = right.rhw;
	return *this;
}
Vertex& Vertex::operator += (const Vertex& right)
{
	pos.x += right.pos.x;
	pos.y += right.pos.y;
	pos.z += right.pos.z;
	pos.w += right.pos.w;
	color.x += right.color.x;
	color.y += right.color.y;
	color.z += right.color.z;
	color.w += right.color.w;
	norm.x += right.norm.x;
	norm.y += right.norm.y;
	norm.z += right.norm.z;
	norm.w += right.norm.w;
	texCoord.x += right.texCoord.x;
	texCoord.y += right.texCoord.y;
	rhw += right.rhw;
	return *this;
}