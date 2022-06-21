#pragma once
#include "StructType.h"
#include "vector"
using namespace std;

void Swap(Point& orign, Point& change)
{
	Point temp = orign;
	orign = change;
	change = temp;
}

struct ConvexHull
{
public:
	vector<Point> hull;
	vector<Point> vertices;

	ConvexHull(){}
	ConvexHull(vector<Point>& v):vertices(v){}

	void QuickSort(int left, int right);
	static bool isCCW(const Point& a, const Point& b,const Point& c);
	bool CheckInclude(const int& index);

	void SetConvexHull();
	int GetHullSize() { return hull.size(); }

};

inline void ConvexHull::QuickSort(int left, int right)
{
	int pivot = left;
	int j = pivot;

	if (left < right) {
		for (int i = left + 1; i <= right; i++)
		{
			if (vertices[i].degree < vertices[pivot].degree)
			{
				j++;
				Swap(vertices[j], vertices[i]);
			}
			if (vertices[i].degree == vertices[pivot].degree)
			{
				if (vertices[i]._v._x < vertices[pivot]._v._x)
				{
					j++;
					Swap(vertices[j], vertices[i]);
				}
			}
		}
		Swap(vertices[left], vertices[j]);
		pivot = j;

		// ��, �� ������ ��� ��Ʈ
		QuickSort(left, pivot - 1);
		QuickSort(pivot + 1, right);
	}
}

inline bool ConvexHull::isCCW(const Point & a, const Point & b, const Point& c)
{
	float orignX = b._v._x - a._v._x;
	float orignY = b._v._y - a._v._y;

	float nextX = c._v._x - a._v._x;
	float nextY = c._v._y - a._v._y;

	D3DXVECTOR3 orign(orignX, orignY, 0);
	D3DXVECTOR3 next(nextX, nextY, 0);
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &orign, &next);

	D3DXVECTOR3 up(0, 0, 1);

	if (D3DXVec3Dot(&normal, &up) <= 0) return false;
	return true;
}

inline bool ConvexHull::CheckInclude(const int & index)
{
	bool isInclude = false;

	for (int i = 0; i < GetHullSize(); i++)
	{
		if (hull[i]._index == index) 
		{
			isInclude = true;
			break;
		}
	}

	return isInclude;
}

inline void ConvexHull::SetConvexHull()
{
	int min;
	int minY = 100;

	// y ���� ���� ���� �� ã��
	for (int i = 0; i < vertices.size(); i++)
	{
		if (minY > vertices[i]._v._y)
		{
			min = i;
			minY = vertices[i]._v._y;
		}
	}
	// ã�� ���� �������� ��� ������ ���� ���ϱ� 
	for (int j = 0; j < vertices.size(); j++)
	{
		if (j == min) continue;
		float x = vertices[j]._v._x - vertices[min]._v._x;
		float y = vertices[j]._v._y - vertices[min]._v._y;
		float d = atan2(y, x);

		vertices[j].degree = d;
	}

	// ������ ���� ������ ����
	QuickSort(0, vertices.size() - 1);

	vertices[0]._v._color = D3DCOLOR_XRGB(255, 0, 0);
	vertices[1]._v._color = D3DCOLOR_XRGB(255, 0, 0);
	//�� �ð� ������ ���� �ܰ� ������ 
	hull.push_back(vertices[0]);
	hull.push_back(vertices[1]);
	for (int k = 2; k < vertices.size(); k++)
	{
		int num = hull.size();
		Point b = hull.at(num - 1);
		Point a = hull.at(num - 2);

		// ���ο� ���� �ð�����϶�
		if (!isCCW(a, b, vertices[k]))
		{
			//���� �� ����
			hull.pop_back();

			// �ݽð������ �ɶ����� ���ο� ���� �� ���� ���� ��
			for (int i = 3; i <= num; i++)
			{
				int index = num - i;
				if (!isCCW(hull.at(index), hull.at(index + 1), vertices[k]))
					hull.pop_back();
				else break;
			}
		}

		vertices[k]._v._color = D3DCOLOR_XRGB(255, 0, 0);
		hull.push_back(vertices[k]);
	}
}
