#pragma once
#include "Vector4.h"
#define LIGHT_ATTR_AMBIENT 0x0001//������Դ
#define LIGHT_ATTR_INFINITE 0x0002//ƽ�й�Դ
#define LIGHT_ATTR_POINT 0x0004//���Դ
#define LIGHT_ATTR_SPOTLIGHT1 0x0008//�򵥾۹��
#define LIGHT_ATTR_SPOTLIGHT2 0x0010//���Ӿ۹��
#define LIGHT_STATE_ON 1//��Դ��
#define LIGHT_STATE_OFF 0//��Դ�ر�
#define MAX_LIGHTS 8//���֧����
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
	float kc, kl, kq;//˥������
	float spotInner;//�۹����׶��
	float spotOuter;//�۹����׶��
	float pf;//�۹��ָ������
	Light(int id,int state,int attribute,const Vector4& light,
		const Vector4& position,const Vector4& direction,float kc=0,float kl=0,float kq=0,
		float spotInner=0,float spotOuter=0,float pf=0);
	~Light();
};

