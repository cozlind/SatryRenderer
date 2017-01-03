#pragma once
#include "Vector2.h"
#include "Vector4.h"
///





//该类未使用 This class isn't used






///
class Vertex
{
public:
	Vector4 pos;
	Vector4 color;
	Vector4 norm;
	Vector2 texCoord;
	float rhw;

	Vertex();
	Vertex(Vector4 position, Vector4 color, Vector4 normal, Vector2 textureCoordinate, float rhw = 0);


	void RhwInit(); 
	void setTexCoord(float u,float v);
	UINT32 Vertex::GetColor()const;
	static Vertex Division(const Vertex&,const Vertex&,float );
	static Vertex Interpolate(const Vertex&, const Vertex&, float);
	static void Swap(Vertex&, Vertex&);

	Vertex operator + (const Vertex&) const;
	Vertex operator * (const float) const;
	Vertex& operator += (const Vertex&);
	Vertex& operator = (const Vertex&) ;
};

