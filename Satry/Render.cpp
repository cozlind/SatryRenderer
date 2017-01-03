#include "stdafx.h"
#include "Render.h"
#include "Vertex.h"
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#define FCMP(a,b) ((fabs(a-b)<0.0001f)?1:0)
#define INTERP(x1, x2,t) (x1 + (x2 - x1) * t)
#define PI 3.1415926f

extern Camera camera;
extern Transform transform;
extern int renderMode;
extern int shaderMode;
extern bool isPrint;
extern HDC hBDC;
extern int numObj;
extern Material matList[];
extern Material normMat;
extern Light lights[];
extern int numLight;
extern CubeMap cubemap;
Matrix inverseGeneral;

inline void Swap(float& a, float& b)
{
	float t = a;
	a = b;
	b = t;
}
Render::Render() :state(0), attribute(0), numPolys(0), showBackFace(false), showAlpha(false), showCubeMap(false), showNormMap(false), enableZBuffer(true){}
Render::~Render() {}
void Render::ResetFlag() {
	showBackFace = false;
	showAlpha = false;
	showCubeMap = false;
	showNormMap = false;
	enableZBuffer = true;
}
void Render::Init()
{
	memset(&frameBuffer[0][0], 0, sizeof(UINT32)*width*height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			zBuffer[j][i] = 1;
		}
	}
	minClipX = 0;
	maxClipX = width;
	minClipY = 0;
	maxClipY = height;
}
void Render::Clear()
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			zBuffer[j][i] = 1;
		}
	}
	float currentY = camera.n.y*2;//获取到当前摄像机的仰角（这是一个代替值，不是真正的仰角，根据角度和对应行的映射实现伪天空盒）
	if (currentY < 0 && currentY + 0.25*PI <=- 0.25*PI) 
	{
		memset(&frameBuffer[0][0], 0, sizeof(UINT32)*width*height);
	}
	else if (currentY > 0 && currentY - 0.25*PI >= 0.25*PI)
	{
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				frameBuffer[j][i] = 0xffffff;
			}
		}
	}
	else
	{
		int ymin = currentY > 0 ? (int)(365 * tan(currentY - 0.25*PI)) : -height / 2;
		int ymax = currentY > 0 ? height / 2 : (int)(365 * tan(currentY + 0.25*PI));
		ymin = ymin > height / 2 ? height : height / 2 + ymin;
		ymax = ymax < -height / 2 ? 0 : height / 2 + ymax;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int digitColor=0;
				if (currentY > 0) 
				{
					if (j < ymax - ymin) 
					{
						digitColor = (j + ymin) * 255 / height;
					}
					else digitColor = 255;
				}
				else 
				{
					if (j > height - (ymax-ymin)-1)
					{
						digitColor = (j - height + ymax+1) * 255 / height;
					}
					else digitColor = 0;
				}
				frameBuffer[j][i] = digitColor << 16 | digitColor << 8 | digitColor;
			}
		}
	}
}
void Render::DrawPixel(int x, int y, UINT32 color)
{
	frameBuffer[y][x] = color;
}
void Render::DrawLine(int x1, int y1, int x2, int y2, UINT32 color)
{// Liang - Barsky裁剪算法
	int p[4] = { x1 - x2, x2 - x1, y1 - y2, y2 - y1 };
	int q[4] = { x1 - minClipX, maxClipX - x1, y1 - minClipY, maxClipY - y1 };
	float u1 = 0, u2 = 1;
	for (int i = 0; i < 4; i++)
	{
		float r = (float)q[i] / (float)p[i];
		if (p[i] < 0)//由外向内穿过边界延长线
		{
			if (r > u2) return;
			u1 = u1 >= r ? u1 : r;
		}
		else if (p[i] > 0)//由内向外穿过边界延长线
		{
			if (r < u1) return;
			u2 = u2 <= r ? u2 : r;
		}
		//在边界外且水平或竖直
		else if (p[i] == 0 && q[i] < 0) return;
	}
	int a1 = x1, b1 = y1, a2 = x2, b2 = y2;
	x1 = (int)(a1 - u1*(a1 - a2) + 0.5f);
	y1 = (int)(b1 - u1*(b1 - b2) + 0.5f);
	x2 = (int)(a1 - u2*(a1 - a2) + 0.5f);
	y2 = (int)(b1 - u2*(b1 - b2) + 0.5f);

	if (x1 == x2)
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (int y = y1; y != y2; y += inc)
			DrawPixel(x1, y, color);
	}
	else if (y1 == y2)
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (int x = x1; x != x2; x += inc)
			DrawPixel(x, y1, color);
	}
	else
	{
		int x, y, error = 0;
		int dx = x1 > x2 ? x1 - x2 : x2 - x1;
		int dy = y1 > y2 ? y1 - y2 : y2 - y1;
		if (dx >= dy)
		{
			if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;//交换
			for (x = x1, y = y1; x <= x2; x++)
			{
				DrawPixel(x, y, color);
				error += dy;
				if (error >= dx)
				{
					error -= dx;
					y += (y2 >= y1) ? 1 : -1;
				}
			}
			DrawPixel(x2, y2, color);
		}
		else
		{
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;//交换
			for (x = x1, y = y1; y <= y2; y++)
			{
				DrawPixel(x, y, color);
				error += dx;
				if (error >= dy)
				{
					error -= dy;
					x += (x2 >= x1) ? 1 : -1;
				}
			}
			DrawPixel(x2, y2, color);
		}
	}
}
void Render::DrawTopTri(Vector4 v1, Vector4 c1, Vector2 uv1, float rhw1, Vector4 r1,
	Vector4 v2, Vector4 c2, Vector2 uv2, float rhw2, Vector4 r2,
	Vector4 v3, Vector4 c3, Vector2 uv3, float rhw3, Vector4 r3, Poly& poly)
{
	int bottom = (int)(v2.y + 0.5f);
	int top = (int)(v3.y + 0.5f);
	//y裁剪
	if (bottom < minClipY) bottom = minClipY;
	if (top > maxClipY) top = maxClipY;
	//绘制
	for (int y = bottom; y < top; y++)
	{
		//纵向插值
		Vector4 vs = Vector4::Interpolate(v1, v3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		Vector4 ve = Vector4::Interpolate(v2, v3, (y + 0.5f - v2.y) / (v3.y - v2.y));
		Vector4 cs = Vector4::Interpolate(c1, c3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		Vector4 ce = Vector4::Interpolate(c2, c3, (y + 0.5f - v2.y) / (v3.y - v2.y));
		Vector2 uvs = Vector2::Interpolate(uv1, uv3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		Vector2 uve = Vector2::Interpolate(uv2, uv3, (y + 0.5f - v2.y) / (v3.y - v2.y));
		float rhws = INTERP(rhw1, rhw3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		float rhwe = INTERP(rhw2, rhw3, (y + 0.5f - v2.y) / (v3.y - v2.y));
		//反射向量旋转插值
		Matrix rotate1, rotate2;
		float thetas = Vector4::Angle(r1, r3);
		float dthetas = thetas* (y + 0.5f - v1.y) / (v3.y - v1.y);
		Vector4 axis1 = Vector4::Cross(r1, r3);
		rotate1.SetRotate(axis1.x, axis1.y, axis1.z, dthetas);
		float thetae = Vector4::Angle(r2, r3);
		float dthetae = thetae* (y + 0.5f - v2.y) / (v3.y - v2.y);
		Vector4 axis2 = Vector4::Cross(r2, r3);
		rotate2.SetRotate(axis2.x, axis2.y, axis2.z, dthetae);

		Vector4 rs = r1*rotate1;
		Vector4 re = r2*rotate2;
		//非平底的部分特殊钝角三角形
		if (vs.x > ve.x)
		{
			Vector4::Swap(vs, ve);
			Vector4::Swap(v1, v2);
			Vector4::Swap(cs, ce);
			Vector4::Swap(c1, c2);
			Vector2::Swap(uvs, uve);
			Vector2::Swap(uv1, uv2);
			Vector4::Swap(rs, re);
			Vector4::Swap(r1, r2);
			Swap(rhws, rhwe);
			Swap(rhw1, rhw2);
		}
		//横向插值
		float factor = 1 / (ve.x - vs.x);
		Vector4 dv((ve.x - vs.x) * factor, (ve.y - vs.y) * factor, (ve.z - vs.z) * factor, (ve.w - vs.w) * factor);
		Vector4 dc((ce.x - cs.x) * factor, (ce.y - cs.y) * factor, (ce.z - cs.z) * factor, (ce.w - cs.w) * factor);
		Vector2 duv((uve.x - uvs.x) * factor, (uve.y - uvs.y) * factor);
		float drhw = (rhwe - rhws) * factor;
		//反射向量旋转插值
		float theta = Vector4::Angle(rs, re);
		float dTheta = theta*factor;
		Vector4 axis = Vector4::Cross(rs, re);
		Matrix rotateMatrix;
		rotateMatrix.SetRotate(axis.x, axis.y, axis.z, dTheta);

		int x = (int)(vs.x + 0.5f);
		int right = (int)(ve.x + 0.5f);
		//x裁剪
		if (x < minClipX)
		{
			vs += (dv*(minClipX - x));
			cs += (dc*(minClipX - x));
			uvs += (duv*(minClipX - x));
			rs = Vector4::Rotate(rs, re, dTheta*(minClipX - x));
			rhws += (drhw*(minClipX - x));
			x = minClipX;
			if (right < minClipX) continue;
		}
		if (right > maxClipX)
		{
			right = maxClipX;
			if (x > maxClipX) continue;
		}
		while (x < right)
		{
			if (!enableZBuffer||zBuffer[y][x] >= vs.z)
			{
				if (enableZBuffer) 
				{
					zBuffer[y][x] = vs.z;
				}

				switch (shaderMode)
				{
				case SHADE_MODE_FLAT:
					if (showAlpha)
					{
						Vector4 polyColor;
						Vector4::GetColorV4(poly.color, polyColor);
						Vector4 newColor;
						Vector4::GetColorV4(frameBuffer[y][x], newColor);
						newColor.x = 0.7*polyColor.x + 0.3*newColor.x;
						newColor.y = 0.7*polyColor.y + 0.3*newColor.y;
						newColor.z = 0.7*polyColor.z + 0.3*newColor.z;
						DrawPixel(x, y, newColor.GetColor255());
					}
					else
					{
						DrawPixel(x, y, poly.color);
					}
					break;
				case SHADE_MODE_GOURAUD:
				{
					//纹理映射
					Vector4 texColor(1, 1, 1);
					if (showCubeMap) {
						texColor = cubemap.GetCubeMapColor255(rs.x, rs.y, rs.z);
					}
					else
					{
						Vector4 c = cs;
						if (renderMode == RENDER_MODE_TEXTURE&&matList[poly.matIndex].texture.data != NULL)
						{
							float u = uvs.x / rhws;
							float v = uvs.y / rhws;
							//获取decal texture的像素
							texColor = matList[poly.matIndex].GetTexColor255(u, v)*(1.0f / 255.0f);
							if (showNormMap)
							{
								//texColor = Vector4(1, 1, 1);
								c = Vector4(0, 0, 0);
								//获取法线
								Vector4 norm = normMat.GetTexColor255(u, v)*(1.0f / 255.0f);
								norm.x = (norm.x - 0.5f) * 2;
								norm.y = (norm.y - 0.5f) * 2;
								norm.z = (norm.z - 0.5f) * 2;
								//获取当前像素的世界坐标
								Vector4 v = vs;
								transform.ViewportInverseTransform(v);
								v *= inverseGeneral;
								v *= transform.model;

								for (int k = 0; k < numLight; k++)
								{
									Light& light = lights[k];
									if (light.state == LIGHT_STATE_ON)
									{
										float dp;//点积
										float d;//表面和光源的距离
										float i;//光强
										float atten;//衰减因子
										float factor;//临时因子
										switch (light.attribute)
										{
										case LIGHT_ATTR_AMBIENT:
											c.x += (matList[poly.matIndex].ka.x*light.ambient.x);//材质的分量在0-1，光在0-255，结果在0-255
											c.y += (matList[poly.matIndex].ka.y*light.ambient.y);
											c.z += (matList[poly.matIndex].ka.z*light.ambient.z);
											break;
										case LIGHT_ATTR_INFINITE:
											dp = Vector4::Dot(norm, light.dir);
											if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
											{
												c.x += (matList[poly.matIndex].kd.x * light.diffuse.x*dp);
												c.y += (matList[poly.matIndex].kd.y * light.diffuse.y*dp);
												c.z += (matList[poly.matIndex].kd.z * light.diffuse.z*dp);
											}
											break;
										case LIGHT_ATTR_POINT:
										{
											Vector4 l = light.pos - v;
											d = l.Magnitude();
											dp = Vector4::Dot(norm, l.Normalize());
											if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
											{
												atten = light.kc + light.kl*d + light.kq*d*d;
												factor = dp / atten;
												c.x += (matList[poly.matIndex].kd.x * light.diffuse.x*factor);
												c.y += (matList[poly.matIndex].kd.y * light.diffuse.y*factor);
												c.z += (matList[poly.matIndex].kd.z * light.diffuse.z*factor);
											}
											break;
										}
										case LIGHT_ATTR_SPOTLIGHT1:
										{
												Vector4 l = light.pos - v;
												d = l.Magnitude();
												dp = Vector4::Dot(norm, light.dir);
												if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
												{
													atten = light.kc + light.kl*d + light.kq*d*d;
													factor = dp / atten/5;
													c.x += (matList[poly.matIndex].kd.x*light.diffuse.x*factor);
													c.y += (matList[poly.matIndex].kd.y*light.diffuse.y*factor);
													c.z += (matList[poly.matIndex].kd.z*light.diffuse.z*factor);
												}
											break;
										}
										}
									}
								}
								c.x = c.x > 255 ? 255 : c.x;
								c.y = c.y > 255 ? 255 : c.y;
								c.z = c.z > 255 ? 255 : c.z;
							}
						}
						texColor *= c;
					}

					if (showAlpha)
					{
						Vector4 newColor = texColor;
						Vector4::GetColorV4(frameBuffer[y][x], newColor);
						newColor.x = 0.7*texColor.x + 0.3*newColor.x;
						newColor.y = 0.7*texColor.y + 0.3*newColor.y;
						newColor.z = 0.7*texColor.z + 0.3*newColor.z;
						DrawPixel(x, y, newColor.GetColor255());
					}
					else
					{
						DrawPixel(x, y, texColor.GetColor255());
					}

					vs += dv;
					cs += dc;
					rs *= rotateMatrix;
					uvs += duv;
					rhws += drhw;
					break;
				}
				}
			}
			x++;
		}

	}
}
//平底下三角 2,3是底角 1是顶角
void Render::DrawBottomTri(Vector4 v1, Vector4 c1, Vector2 uv1, float rhw1, Vector4 r1,
	Vector4 v2, Vector4 c2, Vector2 uv2, float rhw2, Vector4 r2,
	Vector4 v3, Vector4 c3, Vector2 uv3, float rhw3, Vector4 r3, Poly& poly)
{
	int bottom = (int)(v1.y + 0.5f);
	int top = (int)(v3.y + 0.5f);
	//y裁剪
	if (bottom < minClipY) bottom = minClipY;
	if (top > maxClipY) top = maxClipY;

	//绘制
	for (int y = bottom; y < top; y++)
	{
		//纵向插值
		Vector4 vs = Vector4::Interpolate(v1, v2, (y + 0.5f - v1.y) / (v2.y - v1.y));
		Vector4 ve = Vector4::Interpolate(v1, v3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		Vector4 cs = Vector4::Interpolate(c1, c2, (y + 0.5f - v1.y) / (v2.y - v1.y));
		Vector4 ce = Vector4::Interpolate(c1, c3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		Vector2 uvs = Vector2::Interpolate(uv1, uv2, (y + 0.5f - v1.y) / (v2.y - v1.y));
		Vector2 uve = Vector2::Interpolate(uv1, uv3, (y + 0.5f - v1.y) / (v3.y - v1.y));

		Matrix rotate1, rotate2;
		float thetas = Vector4::Angle(r1, r2);
		float dthetas = thetas* (y + 0.5f - v1.y) / (v2.y - v1.y);
		Vector4 axis1 = Vector4::Cross(r1, r2);
		rotate1.SetRotate(axis1.x, axis1.y, axis1.z, dthetas);
		float thetae = Vector4::Angle(r1, r3);
		float dthetae = thetae* (y + 0.5f - v1.y) / (v3.y - v1.y);
		Vector4 axis2 = Vector4::Cross(r1, r3);
		rotate2.SetRotate(axis2.x, axis2.y, axis2.z, dthetae);

		Vector4 rs = r1*rotate1;
		Vector4 re = r1*rotate2;

		float rhws = INTERP(rhw1, rhw2, (y + 0.5f - v1.y) / (v2.y - v1.y));
		float rhwe = INTERP(rhw1, rhw3, (y + 0.5f - v1.y) / (v3.y - v1.y));
		//非平底的部分特殊钝角三角形
		if (vs.x > ve.x)
		{
			Vector4::Swap(vs, ve);
			Vector4::Swap(v2, v3);
			Vector4::Swap(cs, ce);
			Vector4::Swap(c2, c3);
			Vector2::Swap(uvs, uve);
			Vector2::Swap(uv2, uv3);
			Vector4::Swap(rs, re);
			Vector4::Swap(r2, r3);
			Swap(rhws, rhwe);
			Swap(rhw2, rhw3);
		}
		//横向插值
		float factor = 1 / (ve.x - vs.x);
		Vector4 dv((ve.x - vs.x) * factor, (ve.y - vs.y) * factor, (ve.z - vs.z) * factor, (ve.w - vs.w) * factor);
		Vector4 dc((ce.x - cs.x) * factor, (ce.y - cs.y) * factor, (ce.z - cs.z) * factor, (ce.w - cs.w) * factor);
		Vector2 duv((uve.x - uvs.x) * factor, (uve.y - uvs.y) * factor);

		float theta = Vector4::Angle(rs, re);
		float dTheta = theta*factor;
		Vector4 axis = Vector4::Cross(rs, re);
		Matrix rotateMatrix;
		rotateMatrix.SetRotate(axis.x, axis.y, axis.z, dTheta);

		float drhw = (rhwe - rhws) * factor;
		int x = (int)(vs.x + 0.5f);
		int right = (int)(ve.x + 0.5f);
		//x裁剪
		if (x < minClipX)
		{
			vs += (dv*(minClipX - x));
			cs += (dc*(minClipX - x));
			uvs += (duv*(minClipX - x));
			rs = Vector4::Rotate(rs, re, dTheta*(minClipX - x));
			rhws += (drhw*(minClipX - x));
			x = minClipX;
			if (right < minClipX) continue;
		}
		if (right > maxClipX)
		{
			right = maxClipX;
			if (x > maxClipX) continue;
		}
		while (x < right)
		{
			if (!enableZBuffer || zBuffer[y][x] >= vs.z)
			{
				if (enableZBuffer)
				{
					zBuffer[y][x] = vs.z;
				}
				switch (shaderMode)
				{
				case SHADE_MODE_FLAT:
					if (showAlpha)
					{
						Vector4 polyColor;
						Vector4::GetColorV4(poly.color, polyColor);
						Vector4 newColor;
						Vector4::GetColorV4(frameBuffer[y][x], newColor);
						newColor.x = 0.7*polyColor.x + 0.3*newColor.x;
						newColor.y = 0.7*polyColor.y + 0.3*newColor.y;
						newColor.z = 0.7*polyColor.z + 0.3*newColor.z;
						DrawPixel(x, y, newColor.GetColor255());
					}
					else
					{
						DrawPixel(x, y, poly.color);
					}
					break;
				case SHADE_MODE_GOURAUD:
				{
					//纹理映射
					Vector4 texColor(1, 1, 1);

					if (showCubeMap) {
						texColor = cubemap.GetCubeMapColor255(rs.x, rs.y, rs.z);
					}
					else
					{
						Vector4 c = cs;
						if (renderMode == RENDER_MODE_TEXTURE&&matList[poly.matIndex].texture.data != NULL)
						{
							float u = uvs.x / rhws;
							float v = uvs.y / rhws;
							//获取decal texture的像素
							texColor = matList[poly.matIndex].GetTexColor255(u, v)*(1.0f / 255.0f);
							if (showNormMap)
							{
								//texColor = Vector4(1, 1, 1);
								c = Vector4(0, 0, 0);
								//获取法线
								Vector4 norm = normMat.GetTexColor255(u, v)*(1.0f / 255.0f);
								norm.x = (norm.x - 0.5f) * 2;
								norm.y = (norm.y - 0.5f) * 2;
								norm.z = (norm.z - 0.5f) * 2;
								//获取当前像素的世界坐标
								Vector4 v = vs;
								transform.ViewportInverseTransform(v);
								v *= inverseGeneral;
								v *= transform.model;

								for (int k = 0; k < numLight; k++)
								{
									Light& light = lights[k];
									if (light.state == LIGHT_STATE_ON)
									{
										float dp;//点积
										float d;//表面和光源的距离
										float i;//光强
										float atten;//衰减因子
										float factor;//临时因子
										switch (light.attribute)
										{
										case LIGHT_ATTR_AMBIENT:
											c.x += (matList[poly.matIndex].ka.x*light.ambient.x);//材质的分量在0-1，光在0-255，结果在0-255
											c.y += (matList[poly.matIndex].ka.y*light.ambient.y);
											c.z += (matList[poly.matIndex].ka.z*light.ambient.z);
											break;
										case LIGHT_ATTR_INFINITE:
											dp = Vector4::Dot(norm, light.dir);
											if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
											{
												c.x += (matList[poly.matIndex].kd.x * light.diffuse.x*dp);
												c.y += (matList[poly.matIndex].kd.y * light.diffuse.y*dp);
												c.z += (matList[poly.matIndex].kd.z * light.diffuse.z*dp);
											}
											break;
										case LIGHT_ATTR_POINT:
										{
											Vector4 l = light.pos - v;
											d = l.Magnitude();
											dp = Vector4::Dot(norm, l.Normalize());
											if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
											{
												atten = light.kc + light.kl*d + light.kq*d*d;
												factor = dp / atten;
												c.x += (matList[poly.matIndex].kd.x * light.diffuse.x*factor);
												c.y += (matList[poly.matIndex].kd.y * light.diffuse.y*factor);
												c.z += (matList[poly.matIndex].kd.z * light.diffuse.z*factor);
											}
											break;
										}
										case LIGHT_ATTR_SPOTLIGHT1:
										{
											Vector4 l = light.pos - v;
											d = l.Magnitude();
											dp = Vector4::Dot(norm, light.dir);
											if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
											{
												atten = light.kc + light.kl*d + light.kq*d*d;
												factor = dp / atten / 5;
												c.x += (matList[poly.matIndex].kd.x*light.diffuse.x*factor);
												c.y += (matList[poly.matIndex].kd.y*light.diffuse.y*factor);
												c.z += (matList[poly.matIndex].kd.z*light.diffuse.z*factor);
											}
											break;
										}
										}
									}
								}
								c.x = c.x > 255 ? 255 : c.x;
								c.y = c.y > 255 ? 255 : c.y;
								c.z = c.z > 255 ? 255 : c.z;
							}
						}
						texColor *= c;
					}

					if (showAlpha)
					{
						Vector4 newColor = texColor;
						Vector4::GetColorV4(frameBuffer[y][x], newColor);
						newColor.x = 0.7*texColor.x + 0.3*newColor.x;
						newColor.y = 0.7*texColor.y + 0.3*newColor.y;
						newColor.z = 0.7*texColor.z + 0.3*newColor.z;
						DrawPixel(x, y, newColor.GetColor255());
					}
					else
					{
						DrawPixel(x, y, texColor.GetColor255());
					}

					vs += dv;
					cs += dc;
					rs *= rotateMatrix;
					uvs += duv;
					rhws += drhw;
					break;
				}
				}
			}
			x++;
		}
	}
}
void Render::DrawTriangle(Poly& poly)
{
	Vector4 v1 = poly.vList[poly.vert[0]];
	Vector4 v2 = poly.vList[poly.vert[1]];
	Vector4 v3 = poly.vList[poly.vert[2]];
	Vector4 c1 = poly.lightColor[0];
	Vector4 c2 = poly.lightColor[1];
	Vector4 c3 = poly.lightColor[2];
	Vector2 uv1 = poly.uvList[poly.uv[0]];
	Vector2 uv2 = poly.uvList[poly.uv[1]];
	Vector2 uv3 = poly.uvList[poly.uv[2]];
	Vector4 r1 = poly.reflectList[poly.norm[0]];
	Vector4 r2 = poly.reflectList[poly.norm[1]];
	Vector4 r3 = poly.reflectList[poly.norm[2]];
	//DisplayPosition(v1, uv1, v2, uv2, v3, uv3);
	float rhw1 = 1 / v1.w;
	uv1.x *= rhw1;
	uv1.y *= rhw1;
	float rhw2 = 1 / v2.w;
	uv2.x *= rhw2;
	uv2.y *= rhw2;
	float rhw3 = 1 / v3.w;
	uv3.x *= rhw3;
	uv3.y *= rhw3;
	if (renderMode&RENDER_MODE_WIREFRAME)
	{
		DrawLine(v1.x, v1.y, v2.x, v2.y);
		DrawLine(v1.x, v1.y, v3.x, v3.y);
		DrawLine(v2.x, v2.y, v3.x, v3.y);
	}
	else if (renderMode&(RENDER_MODE_COLOR | RENDER_MODE_TEXTURE))
	{
		if ((FCMP(v1.x, v2.x) && FCMP(v2.x, v3.x)) || FCMP(v1.y, v2.y) && FCMP(v2.y, v3.y)) return;
		//按照y的增序排列
		if (v1.y > v2.y) { Vector4::Swap(v1, v2); Vector4::Swap(c1, c2); Vector2::Swap(uv1, uv2); Swap(rhw1, rhw2); Vector4::Swap(r1, r2); }
		if (v1.y > v3.y) { Vector4::Swap(v1, v3); Vector4::Swap(c1, c3); Vector2::Swap(uv1, uv3); Swap(rhw1, rhw3); Vector4::Swap(r1, r3); }
		if (v2.y > v3.y) { Vector4::Swap(v2, v3); Vector4::Swap(c2, c3); Vector2::Swap(uv2, uv3); Swap(rhw2, rhw3); Vector4::Swap(r2, r3); }

		if (FCMP(v1.y, v2.y)) DrawTopTri(v1, c1, uv1, rhw1, r1, v2, c2, uv2, rhw2, r2, v3, c3, uv3, rhw3, r3, poly);
		else if (FCMP(v2.y, v3.y)) DrawBottomTri(v1, c1, uv1, rhw1, r1, v2, c2, uv2, rhw2, r2, v3, c3, uv3, rhw3, r3, poly);
		else
		{
			DrawTopTri(v1, c1, uv1, rhw1, r1, v2, c2, uv2, rhw2, r2, v3, c3, uv3, rhw3, r3, poly);//第一个点是假点，实际上应当是截断点
			DrawBottomTri(v1, c1, uv1, rhw1, r1, v3, c3, uv3, rhw3, r3, v2, c2, uv2, rhw2, r2, poly);//第二个点是假点，实际上应当是截断点
		}
	}
}
void Render::DrawObj(Obj* objList, float theta, Camera& camera)
{
	Matrix m1, m2;
	m1.SetRotate(1, 0.5, -0.1, theta);
	for (int i = 0; i < numObj; i++)
	{
		//使用模型变换
		m2.SetTranslate(objList[i].worldPos.x, objList[i].worldPos.y, objList[i].worldPos.z);
		transform.model = m1*m2;
		Matrix normalModel = transform.model.InverseTranspose();//用于法线变换
																//对包围球进行视见变换，球体本身就是世界空间坐标
		Vector4 sphere = objList[i].worldPos*transform.view;
		//物体剔除
		float xBorder = sphere.z * tan(camera.fovx / 2);
		float yBorder = sphere.z * tan(camera.fovy / 2);
		if ((sphere.x - objList[i].maxRadius > xBorder) || (sphere.x + objList[i].maxRadius < -xBorder)
			|| (sphere.y - objList[i].maxRadius > yBorder) || (sphere.y + objList[i].maxRadius < -yBorder)
			|| (sphere.z - objList[i].maxRadius > camera.zFar) || (sphere.z + objList[i].maxRadius < camera.zNear))
		{
			objList[i].state = OBJ_STATE_CULLED;
			continue;
		}
		//先转到世界坐标系
		for (int j = 0; j < objList[i].numVertices; j++)
		{
			objList[i].vList[j] = objList[i].bakVList[j] * transform.model;
		}
		for (int j = 0; j < objList[i].numNorm; j++)
		{
			objList[i].normList[j] = objList[i].bakNormList[j] * normalModel;
			objList[i].normList[j].Normalize();
		}


		//背面判定
		for (int j = 0; j < objList[i].numPolys; j++)
		{
			Poly& poly = objList[i].pList[j];
			int index0 = poly.vert[0];
			int index1 = poly.vert[1];
			int index2 = poly.vert[2];
			Vector4 u = poly.vList[index1] - poly.vList[index0];
			Vector4 v = poly.vList[index2] - poly.vList[index0];
			Vector4 n = Vector4::Cross(u, v);//面法向量
			Vector4 view = camera.pos - poly.vList[index0];//视见向量
			if (Vector4::Dot(n, view) <= 0)
			{
				poly.state = POLY_STATE_BACKFACE;
				n.x = -n.x;
				n.y = -n.y;
				n.z = -n.z;
			}
			else
			{
				poly.state = POLY_STATE_ACTIVE;
			}
			//不使用法线贴图时进行顶点光照计算	
			switch (shaderMode)
			{
			case SHADE_MODE_PURE:
				poly.color = matList[poly.matIndex].kd.GetColor01();
				break;
			case SHADE_MODE_FLAT:
			{
				int r = 0, g = 0, b = 0;
				for (int k = 0; k < numLight; k++)
				{
					Light& light = lights[k];
					if (light.state == LIGHT_STATE_ON)
					{
						float dp;//点积
						float d;//表面和光源的距离
						float i;//光强
						float atten;//衰减因子
						float factor;//临时因子
						switch (light.attribute)
						{
						case LIGHT_ATTR_AMBIENT:
							r += (matList[poly.matIndex].ka.x*light.ambient.x);//材质的分量在0-1，光在0-255，结果在0-255
							g += (matList[poly.matIndex].ka.y*light.ambient.y);
							b += (matList[poly.matIndex].ka.z*light.ambient.z);
							break;
						case LIGHT_ATTR_INFINITE:
						{
							dp = Vector4::Dot(n.Normalize(), light.dir);
							if (dp > 0)
							{
								r += (matList[poly.matIndex].kd.x * light.diffuse.x*dp);
								g += (matList[poly.matIndex].kd.y  * light.diffuse.y*dp);
								b += (matList[poly.matIndex].kd.z  * light.diffuse.z*dp);
							}
							break;
						}
						case LIGHT_ATTR_POINT:
						{
							Vector4 l = light.pos - poly.vList[index0];
							d = l.Magnitude();
							dp = Vector4::Dot(n.Normalize(), l.Normalize());
							if (dp > 0)
							{
								atten = light.kc + light.kl*d + light.kq*d*d;
								factor = dp / atten;
								r += (matList[poly.matIndex].kd.x  * light.diffuse.x*factor);
								g += (matList[poly.matIndex].kd.y  * light.diffuse.y*factor);
								b += (matList[poly.matIndex].kd.z  * light.diffuse.z*factor);
							}
							break;
						}
						case LIGHT_ATTR_SPOTLIGHT1:
						{
							Vector4 l = light.pos - poly.vList[index0];
							d = l.Magnitude();
							dp = Vector4::Dot(n.Normalize(), light.dir);
							if (dp > 0)
							{
								atten = light.kc + light.kl*d + light.kq*d*d;
								factor = dp / atten;
								r += (matList[poly.matIndex].kd.x*light.diffuse.x*factor);
								g += (matList[poly.matIndex].kd.y*light.diffuse.y*factor);
								b += (matList[poly.matIndex].kd.z*light.diffuse.z*factor);
							}
							break;
						}
						}
					}
				}
				r = r > 255 ? 255 : r;
				g = g > 255 ? 255 : g;
				b = b > 255 ? 255 : b;
				poly.color = (r << 16) | (g << 8) | b;
				break;
			}
			case SHADE_MODE_GOURAUD:
			{
				//计算反射向量
				if (showCubeMap)
				{
					for (int ii = 0; ii < 3; ii++)
					{
						Vector4 l = (camera.pos - poly.vList[poly.vert[ii]]).Normalize();
						poly.reflectList[poly.norm[ii]] = Vector4::Reflect(l, poly.normList[poly.norm[ii]]).Normalize();
					}
				}
				else if (!showNormMap)
				{
					for (int ii = 0; ii < 3; ii++)
					{
						poly.lightColor[ii].x = 0;
						poly.lightColor[ii].y = 0;
						poly.lightColor[ii].z = 0;
					}
					for (int k = 0; k < numLight; k++)
					{
						Light& light = lights[k];
						if (light.state == LIGHT_STATE_ON)
						{
							float dp;//点积
							float d;//表面和光源的距离
							float i;//光强
							float atten;//衰减因子
							float factor;//临时因子
							switch (light.attribute)
							{
							case LIGHT_ATTR_AMBIENT:
								poly.lightColor[0].x += (matList[poly.matIndex].ka.x*light.ambient.x);//材质的分量在0-1，光在0-255，结果在0-255
								poly.lightColor[0].y += (matList[poly.matIndex].ka.y*light.ambient.y);
								poly.lightColor[0].z += (matList[poly.matIndex].ka.z*light.ambient.z);
								poly.lightColor[1].x += (matList[poly.matIndex].ka.x*light.ambient.x);
								poly.lightColor[1].y += (matList[poly.matIndex].ka.y*light.ambient.y);
								poly.lightColor[1].z += (matList[poly.matIndex].ka.z*light.ambient.z);
								poly.lightColor[2].x += (matList[poly.matIndex].ka.x*light.ambient.x);
								poly.lightColor[2].y += (matList[poly.matIndex].ka.y*light.ambient.y);
								poly.lightColor[2].z += (matList[poly.matIndex].ka.z*light.ambient.z);
								break;
							case LIGHT_ATTR_INFINITE:
								for (int ii = 0; ii < 3; ii++)
								{
									dp = Vector4::Dot(poly.normList[poly.norm[ii]], light.dir);
									if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
									{
										poly.lightColor[ii].x += (matList[poly.matIndex].kd.x * light.diffuse.x*dp);
										poly.lightColor[ii].y += (matList[poly.matIndex].kd.y * light.diffuse.y*dp);
										poly.lightColor[ii].z += (matList[poly.matIndex].kd.z * light.diffuse.z*dp);
									}
								}
								break;
							case LIGHT_ATTR_POINT:
							{
								for (int ii = 0; ii < 3; ii++)
								{
									Vector4 l = light.pos - poly.vList[poly.vert[ii]];
									d = l.Magnitude();
									dp = Vector4::Dot(poly.normList[poly.norm[ii]], l.Normalize());
									if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
									{
										atten = light.kc + light.kl*d + light.kq*d*d;
										factor = dp / atten;
										poly.lightColor[ii].x += (matList[poly.matIndex].kd.x * light.diffuse.x*factor);
										poly.lightColor[ii].y += (matList[poly.matIndex].kd.y * light.diffuse.y*factor);
										poly.lightColor[ii].z += (matList[poly.matIndex].kd.z * light.diffuse.z*factor);
									}
								}
								break;
							}
							case LIGHT_ATTR_SPOTLIGHT1:
							{
								for (int ii = 0; ii < 3; ii++)
								{
									Vector4 l = light.pos - poly.vList[poly.vert[ii]];
									d = l.Magnitude();
									dp = Vector4::Dot(poly.normList[poly.norm[ii]], light.dir);
									if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
									{
										atten = light.kc + light.kl*d + light.kq*d*d;
										factor = dp / atten;
										poly.lightColor[ii].x += (matList[poly.matIndex].kd.x*light.diffuse.x*factor);
										poly.lightColor[ii].y += (matList[poly.matIndex].kd.y*light.diffuse.y*factor);
										poly.lightColor[ii].z += (matList[poly.matIndex].kd.z*light.diffuse.z*factor);
									}
								}
								break;
							}
							case LIGHT_ATTR_SPOTLIGHT2:
							{
								for (int ii = 0; ii < 3; ii++)
								{
									Vector4 l = poly.vList[poly.vert[ii]] - light.pos;//从光源到表面
									d = l.Magnitude();
									dp = Vector4::Dot(poly.normList[poly.norm[ii]], light.dir);
									if ((dp > 0 && poly.state == POLY_STATE_ACTIVE) || (dp = -dp, dp > 0 && poly.state == POLY_STATE_BACKFACE))
									{
										float dpsl = Vector4::Dot(l, light.dir) / d;
										float dpslExp = dpsl;
										for (int index = 1; index < (int)light.pf; index++)
											dpslExp *= dpsl;
										atten = light.kc + light.kl*d + light.kq*d*d;
										factor = dp *dpslExp / atten;
										poly.lightColor[ii].x += (matList[poly.matIndex].kd.x*light.diffuse.x*factor);
										poly.lightColor[ii].y += (matList[poly.matIndex].kd.y*light.diffuse.y*factor);
										poly.lightColor[ii].z += (matList[poly.matIndex].kd.z*light.diffuse.z*factor);
									}
								}
								break;
							}
							}
						}
					}
					for (int ii = 0; ii < 3; ii++)
					{
						poly.lightColor[ii].x = poly.lightColor[ii].x > 255 ? 255 : poly.lightColor[ii].x;
						poly.lightColor[ii].y = poly.lightColor[ii].y > 255 ? 255 : poly.lightColor[ii].y;
						poly.lightColor[ii].z = poly.lightColor[ii].z > 255 ? 255 : poly.lightColor[ii].z;
					}
				}
				break;
			}
			}
		}
		transform.Update();
		for (int j = 0; j < objList[i].numVertices; j++)
		{
			//应用通用矩阵变换
			objList[i].vList[j] = objList[i].bakVList[j] * transform.general;
			inverseGeneral = transform.general.Inverse();
			//if (CVVCheck(obj.vList[i])) return;
			Transform::ViewportTransform(objList[i].vList[j]);
		}
		if (showAlpha || showBackFace)
		{
			for (int j = 0; j < objList[i].numPolys; j++)
			{
				if (objList[i].pList[j].state == POLY_STATE_BACKFACE)
					DrawTriangle(objList[i].pList[j]);
			}
		}
		if (showAlpha || !showBackFace)
		{
			for (int j = 0; j < objList[i].numPolys; j++)
			{
				if (objList[i].pList[j].state == POLY_STATE_ACTIVE)
					DrawTriangle(objList[i].pList[j]);
			}
		}
	}
}

void Render::CirclePlot(int xc, int yc, int x, int y)
{
	DrawPixel(x, y, 0xffffff);
	DrawPixel(2 * xc - x, y, 0xffffff);
	DrawPixel(2 * xc - x, 2 * yc - y, 0xffffff);
	DrawPixel(x, 2 * yc - y, 0xffffff);
	DrawPixel(y + xc - yc, x - xc + yc, 0xffffff);
	DrawPixel(-y + xc + yc, x - xc + yc, 0xffffff);
	DrawPixel(-y + xc + yc, -x + xc + yc, 0xffffff);
	DrawPixel(y + xc - yc, -x + xc + yc, 0xffffff);
}
void Render::DrawCircle(int xc, int yc, int r)
{
	int x, y;
	double d;

	x = 0;
	y = r;
	d = 1.25 - r;
	CirclePlot(xc, yc, x, y);
	while (x < y)
	{
		if (d < 0)
		{
			d = d + 2 * x + 3;
		}
		else
		{
			d = d + 2 * (x - y) + 5;
			y--;
		}
		x++;
		CirclePlot(xc, yc, x, y);
	}
}
int Render::CVVCheck(const Vector4& v)const
{
	Vector4 pos = v;
	int check = 0;
	if (pos.z / pos.w < 0) check |= 1;
	if (pos.z / pos.w > 1)check |= 2;
	if (pos.x / pos.w < -1)check |= 4;
	if (pos.x / pos.w > 1)check |= 8;
	if (pos.y / pos.w < -1)check |= 16;
	if (pos.y / pos.w > 1)check |= 32;
	return check;
}