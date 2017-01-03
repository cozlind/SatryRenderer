#pragma once
#include "Vector4.h"
#define LIGHT_ATTR_AMBIENT 0x0001//环境光源
#define LIGHT_ATTR_INFINITE 0x0002//平行光源
#define LIGHT_ATTR_POINT 0x0004//点光源
#define LIGHT_ATTR_SPOTLIGHT1 0x0008//简单聚光灯
#define LIGHT_ATTR_SPOTLIGHT2 0x0010//复杂聚光灯
#define LIGHT_STATE_ON 1//光源打开
#define LIGHT_STATE_OFF 0//光源关闭
#define MAX_LIGHTS 8//最多支持数
class Light
{
public:
	int id;
	int state;
	int attribute;

	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	Vector4 pos;
	Vector4 dir;
	float kc, kl, kq;//衰减因子
	float spotInner;//聚光灯内锥角
	float spotOuter;//聚光灯外锥角
	float pf;//聚光灯指数因子
	Light(int id,int state,int attribute,const Vector4& light,
		const Vector4& position,const Vector4& direction,float kc=0,float kl=0,float kq=0,
		float spotInner=0,float spotOuter=0,float pf=0);
	~Light();
};

