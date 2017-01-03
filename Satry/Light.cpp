#include "stdafx.h"
#include "Light.h"

Light::Light(int _id, int _state, int _attr, const Vector4& l,
	const Vector4& _pos, const Vector4& _dir, float _kc, float _kl, float _kq,
	float _inner, float _outer, float _pf) :id(_id), state(_state), attribute(_attr),
	kc(_kc), kl(_kl), kq(_kq), spotInner(_inner), spotOuter(_outer), pf(_pf)
{
	switch (attribute)
	{
	case LIGHT_ATTR_AMBIENT: ambient = l; break;
	case LIGHT_ATTR_INFINITE:
	case LIGHT_ATTR_POINT: 
	case LIGHT_ATTR_SPOTLIGHT1:
	case LIGHT_ATTR_SPOTLIGHT2:diffuse = l; break;
	}
	pos = _pos;
	dir = _dir;
	dir.Normalize();
}


Light::~Light()
{
}
