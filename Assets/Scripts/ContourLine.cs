using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ContourLine  {

    List<Point> vertices;
   public List<Point> convex;

	public ContourLine()
    {
        vertices = new List<Point>();
        convex = new List<Point>();
	}

    public void AddVertex(Point InV)
    {
        vertices.Add(InV);
    }

    public void GetConveHull() { SetConvexHull();  }

    void SetConvexHull()
    {
        // Z 값이 가장 낮은 점  = 기준점
        int minIndex = -1;
        float minZ = 256;

        for(int i=0; i< vertices.Count; i++)
        {
            if(minZ > vertices[i]._v.z)
            {
                minZ = vertices[i]._v.z;
                minIndex = i;
            }
        }

        // 기준점과의 각도 구하기
        Vector3 standard = vertices[minIndex]._v;

        for (int i=0; i<vertices.Count; i++)
        {
            if (i == minIndex) continue;

            Vector3 v = vertices[i]._v;
            float x = v.x - standard.x;
            float z = v.z - standard.z;

            float degree = (float)Math.Atan2(z, x);

            Point pt = vertices[i];
            pt.degreeXZ = degree;
            vertices[i] = pt;
        }

        //각도가 좁은 순으로 정렬
        QuickSort(0, vertices.Count - 1);

        //convex hull 구하기
        convex.Add(vertices[0]);
        convex.Add(vertices[1]);

        for(int i=2; i < vertices.Count; i ++ )
        {
            int num = convex.Count;
            Point b = convex[num - 1];
            Point a = convex[num - 2];

            if(!isCCW (a._v, b._v, vertices[i]._v))
            {
                convex.Remove(b);

                for(int j = 3; j<= num; j++)
                {
                    int index = num - j;
                    if (!isCCW(convex[index]._v, convex[index + 1]._v, vertices[i]._v))
                    {
                        int n = convex.Count - 1;
                        if (n > -1) convex.RemoveAt(n);
                    }
                    else break;
                }
            }
            convex.Add(vertices[i]);
        }
    }

    void Swap(Point a, Point b)
    {
        Point temp = a;
        a = b;
        b = temp;
    }

    void QuickSort(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left >= right) return;

        for (int i = left + 1; i <= right; i++)
        {
            if (vertices[i].degreeXZ < vertices[pivot].degreeXZ)
            {
                j++;

                Swap(vertices[j], vertices[i]);
                //Point temp = vertices[j];
                //vertices[j] = vertices[i];
                //vertices[i] = temp;
            }
            else if (vertices[i].degreeXZ == vertices[pivot].degreeXZ)
            {
                if (vertices[i]._v.x < vertices[pivot]._v.x)
                {
                    j++;

                    Swap(vertices[j], vertices[i]);
                    //Point t = high1[j];
                    //high1[j] = high1[i];
                    //high1[i] = t;
                }
            }
        }

        Swap(vertices[j], vertices[left]);
        //Point tem = high1[j];
        //high1[j] = high1[left];
        //high1[left] = tem;

        pivot = j;

        // 좌, 우 영역에 재귀 소트
        QuickSort(left, pivot - 1);
        QuickSort(pivot + 1, right);
    }

    bool isCCW(Vector3 a, Vector3 b, Vector3 c)
    {
        float orignX = b.x - a.x;
        float orignZ = b.z - a.z;

        float nextX = c.x - a.x;
        float nextZ = c.z - a.z;

        Vector3 orign = new Vector3(orignX, orignZ, 0);
        Vector3 next = new Vector3(nextX, nextZ, 0);

        Vector3 normal = Vector3.Cross(orign, next);

        Vector3 up = Vector3.forward;

        if (Vector3.Dot(normal, up) <= 0) return false;
        return true;
    }

}
