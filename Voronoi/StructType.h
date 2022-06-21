#pragma once
#include "d3dUtility.h"

struct Vertex
{
	float _x, _y, _z;
	float _size;
	D3DCOLOR _color;
	static const DWORD FVF;

	Vertex() :_x(0), _y(0), _z(0.0f) {}
	Vertex(float x, float y) :_x(x), _y(y)
	{
		_z = 0.0f;
		_size = 1.0f;
		_color = D3DCOLOR_XRGB(0, 0, 0);
	}
	Vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {
		_size = 1.0f;
		_color = D3DCOLOR_XRGB(0, 0, 0);
	}
	Vertex(float x, float y, float z, float size, D3DCOLOR color) : _x(x), _y(y), _z(z), _size(size), _color(color) {}

	bool operator== (const Vertex& v)const 
	{
		if (_x != v._x) return false;
		if (_y != v._y) return false;
		return true;
	}
	
	Vertex operator^(const Vertex&v)const;
	Vertex operator^(const D3DXVECTOR3&v)const;
	Vertex operator+(const Vertex&v)const;
	Vertex operator-(const Vertex&v)const;
	Vertex operator*(const D3DXVECTOR3&v)const;
	Vertex operator/(const D3DXVECTOR3&v)const;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE;

inline Vertex Vertex::operator^(const Vertex & v) const
{
	return Vertex
	(
		_y*v._z - _z * v._y,
		_z*v._x - _x * v._z,
		_x*v._y - _y * v._x
	);
}

inline Vertex Vertex::operator^(const D3DXVECTOR3 & v) const
{
	return Vertex
	(
		_y*v.z - _z * v.y,
		_z*v.x - _x * v.z,
		_x*v.y - _y * v.x
	);
}

inline Vertex Vertex::operator+(const Vertex&v)const
{
	Vertex result;
	result._x = _x + v._x;
	result._y = _y + v._y;
	
	return result;
}

inline Vertex Vertex::operator-(const Vertex & v) const
{
	Vertex result;
	result._x = _x - v._x;
	result._y = _y - v._y;

	return result;
}

inline Vertex Vertex::operator*(const D3DXVECTOR3 & v) const
{
	return Vertex(_x * v.x, _y * v.y, _z * v.z);
}

inline Vertex Vertex::operator/(const D3DXVECTOR3 & v) const
{
	return Vertex(_x/v.x,_y/v.y,_z/v.z);
}

struct Point
{
	Point() {}
	Point(Vertex v, int index) : _v(v), _index(index) { degree = 0.0f; }

	bool isEqual(Vertex vt)
	{
		if (vt == _v)
			return true;
		else
			return false;
	}

	Vertex _v;
	int _index;
	float degree;
};
