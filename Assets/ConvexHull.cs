using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public struct Point
{
    public Vector3 _v;
    public int _index;
    public float degreeXY;
    public float degreeXZ;

    public Point(Vector3 v, int index){ _v = v; _index = index; degreeXY = 0.0f; degreeXZ = 0.0f; }

    bool isEqual(Vector3 vt)
    {
        if (vt == _v)
            return true;
        else
            return false;
    }
};

public class ConvexHull : MonoBehaviour {

	public List<Point> hull;
   public List<Point> vertices;

    List<Point> high1;
    List<Point> high2;
    List<Point> high3;
    List<Point> high4;
    List<Point> high5;

    public List<Point> hull1;
    public List<Point> hull2;
    public List<Point> hull3;
    public List<Point> hull4;
    public List<Point> hull5;


    ConvexHull() { }
public ConvexHull(List<Point> v)  {
        vertices = v;  hull = new List<Point>();
        high1 = new List<Point>();
        high2= new List<Point>();
        high3 = new List<Point>();
        high4 = new List<Point>();
        high5 = new List<Point>();
        hull1 = new List<Point>();
        hull2 = new List<Point>();
        hull3 = new List<Point>();
        hull4 = new List<Point>();
        hull5 = new List<Point>();
    }

    void Swap(Point orign, Point change)
    {
        Point temp = orign;
        orign = change;
        change = temp;
    }

    void QuickSort(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left < right)
        {
            for (int i = left + 1; i <= right; i++)
            {
                if (vertices[i].degreeXZ <= vertices[pivot].degreeXZ)
                {
                    if (vertices[i].degreeXY < vertices[pivot].degreeXY)
                    {
                        j++;
                        //   Swap(vertices[j], vertices[i]);
                        Point temp = vertices[j];
                        vertices[j] = vertices[i];
                        vertices[i] = temp;
                    }
                    else if(vertices[i].degreeXY == vertices[pivot].degreeXY)
                    {
                        if (vertices[i]._v.x < vertices[pivot]._v.x)
                        {
                            j++;
                            //   Swap(vertices[j], vertices[i]);
                            Point t = vertices[j];
                            vertices[j] = vertices[i];
                            vertices[i] = t;
                        }
                    }
                }
                //if (vertices[i].degreeXY == vertices[pivot].degreeXY)
                //{
                //    if (vertices[i]._v.x < vertices[pivot]._v.x)
                //    {
                //        j++;
                //        //   Swap(vertices[j], vertices[i]);
                //        Point t = vertices[j];
                //        vertices[j] = vertices[i];
                //        vertices[i] = t;
                //    }
                //}
            }
            //  Swap(vertices[left], vertices[j]);
            Point tem = vertices[j];
            vertices[j] = vertices[left];
            vertices[left] = tem;

            pivot = j;

            // 좌, 우 영역에 재귀 소트
            QuickSort(left, pivot - 1);
            QuickSort(pivot + 1, right);
        }
    }
    void QuickSort1(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left < right)
        {
            for (int i = left + 1; i <= right; i++)
            {
                if (high1[i].degreeXZ <= high1[pivot].degreeXZ)
                {
                    if (high1[i].degreeXY < high1[pivot].degreeXY)
                    {
                        j++;

                        Point temp = high1[j];
                        high1[j] = high1[i];
                        high1[i] = temp;
                    }
                    else if (high1[i].degreeXY == high1[pivot].degreeXY)
                    {
                        if (high1[i]._v.x < high1[pivot]._v.x)
                        {
                            j++;

                            Point t = high1[j];
                            high1[j] = high1[i];
                            high1[i] = t;
                        }
                    }
                }
            }
            Point tem = high1[j];
            high1[j] = high1[left];
            high1[left] = tem;

            pivot = j;

            // 좌, 우 영역에 재귀 소트
            QuickSort1(left, pivot - 1);
            QuickSort1(pivot + 1, right);
        }
    }
    void QuickSort2(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left < right)
        {
            for (int i = left + 1; i <= right; i++)
            {
                if (high2[i].degreeXZ <= high2[pivot].degreeXZ)
                {
                    if (high2[i].degreeXY < high2[pivot].degreeXY)
                    {
                        j++;

                        Point temp = high2[j];
                        high2[j] = high2[i];
                        high2[i] = temp;
                    }
                    else if (high2[i].degreeXY == high2[pivot].degreeXY)
                    {
                        if (high2[i]._v.x < high2[pivot]._v.x)
                        {
                            j++;

                            Point t = high2[j];
                            high2[j] = high2[i];
                            high2[i] = t;
                        }
                    }
                }
            }
            Point tem = high2[j];
            high2[j] = high2[left];
            high2[left] = tem;

            pivot = j;

            // 좌, 우 영역에 재귀 소트
            QuickSort2(left, pivot - 1);
            QuickSort2(pivot + 1, right);
        }
    }
    void QuickSort3(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left < right)
        {
            for (int i = left + 1; i <= right; i++)
            {
                if (high3[i].degreeXZ <= high3[pivot].degreeXZ)
                {
                    if (high3[i].degreeXY < high3[pivot].degreeXY)
                    {
                        j++;

                        Point temp = high3[j];
                        high3[j] = high3[i];
                        high3[i] = temp;
                    }
                    else if (high3[i].degreeXY == high3[pivot].degreeXY)
                    {
                        if (high3[i]._v.x < high3[pivot]._v.x)
                        {
                            j++;

                            Point t = high3[j];
                            high3[j] = high3[i];
                            high3[i] = t;
                        }
                    }
                }
            }
            Point tem = high3[j];
            high3[j] = high3[left];
            high3[left] = tem;

            pivot = j;

            // 좌, 우 영역에 재귀 소트
            QuickSort3(left, pivot - 1);
            QuickSort3(pivot + 1, right);
        }
    }
    void QuickSort4(int left, int right)
    {
        int pivot = left;
        int j = pivot;

        if (left < right)
        {
            for (int i = left + 1; i <= right; i++)
            {
                if (high4[i].degreeXZ <= high4[pivot].degreeXZ)
                {
                    if (high4[i].degreeXY < high4[pivot].degreeXY)
                    {
                        j++;

                        Point temp = high4[j];
                        high4[j] = high4[i];
                        high4[i] = temp;
                    }
                    else if (high4[i].degreeXY == high4[pivot].degreeXY)
                    {
                        if (high4[i]._v.x < high4[pivot]._v.x)
                        {
                            j++;

                            Point t = high4[j];
                            high4[j] = high4[i];
                            high4[i] = t;
                        }
                    }
                }
            }
            Point tem = high4[j];
            high4[j] = high4[left];
            high4[left] = tem;

            pivot = j;

            // 좌, 우 영역에 재귀 소트
            QuickSort4(left, pivot - 1);
            QuickSort4(pivot + 1, right);
        }
    }

    public bool isCCW(Point a, Point b, Point c)
    {
        float orignX = b._v.x - a._v.x;
        // float orignY = b._v.y - a._v.y;
        float orignY = b._v.z - a._v.z;

        float nextX = c._v.x - a._v.x;
        //  float nextY = c._v.y - a._v.y;
        float nextY = c._v.z - a._v.z;

        Vector3 orign = new Vector3(orignX, orignY, 0);
        Vector3 next = new Vector3(nextX, nextY, 0);

        Vector3 normal = Vector3.Cross(orign, next);

        Vector3 up = new Vector3(0, 0, 1);

        if (Vector3.Dot(normal, up) <= 0) return false;
        return true;
    }

   public bool CheckInclude(int index)
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

	public void SetConvexHull()
    {
          int[] min = { -1,-1,-1,-1,-1};
          float[] minZ = { 255, 255, 255, 255, 255 };

        // int min = -1;
        // float minY = 100;

        // y 값이 가장 작은 점 찾기
        for (int i = 0; i < vertices.Count; i++)
        {
            float Y = vertices[i]._v.y;
            if (Y>=95 && Y<135)
            {
                if (minZ [0]> vertices[i]._v.z)
                {
                    min[0] = high1.Count;
                    minZ[0] = vertices[i]._v.z;
                }
                high1.Add(vertices[i]);
            }
            else if (Y >= 135 && Y < 175)
            { 
                if (minZ[1] > vertices[i]._v.z)
                {
                    min[1] = high2.Count;
                    minZ[1] = vertices[i]._v.z;
                }
                high2.Add(vertices[i]);
            }
            else if (Y >= 175 && Y < 215)
            {
                if (minZ[2] > vertices[i]._v.z)
                {
                    min[2] = high3.Count;
                    minZ[2] = vertices[i]._v.z;
                }
                high3.Add(vertices[i]);
            }
            else if (Y >= 215 && Y <256)
            {
              
                if (minZ[3] > vertices[i]._v.z)
                {
                    min[3] = high4.Count;
                    minZ[3] = vertices[i]._v.z;
                }
                high4.Add(vertices[i]);
            }
            //else
            //{
            //    if (minZ[4] > vertices[i]._v.z)
            //    {
            //        min[4] = high5.Count;
            //        minZ[4] = vertices[i]._v.z;
            //    }
            //    high5.Add(vertices[i]);
            //}
        }
        //for (int i = 0; i < vertices.Count; i++)
        //{
        //    if (minY > vertices[i]._v.z)
        //    {
        //        min = i;
        //        minY = vertices[i]._v.z;
        //    }
        //}
        // 찾은 점을 기준으로 모든 점과의 각도 구하기 
        //for (int j = 0; j < vertices.Count ; j++)
        //{
        //    if (j == min) continue;
        //    float x = vertices[j]._v.x - vertices[min]._v.x;
        //    float y = vertices[j]._v.y - vertices[min]._v.y;
        //    float xy = (float)Math.Atan2(y, x);

        //    float z = vertices[j]._v.z - vertices[min]._v.z;
        //    float xz = (float)Math.Atan2(z, x);

        //    Point v = vertices[j];
        //    v.degreeXY = xy;
        //    v.degreeXZ = xz;
        //    vertices[j] = v;
        //}

        for(int i =0; i<high1.Count; i++)
        {
            if (i == min[0]) continue;
            float x = high1[i]._v.x - high1[min[0]]._v.x;
            float y = high1[i]._v.y - high1[min[0]]._v.y;
            float xy = (float)Math.Atan2(y, x);

            float z = high1[i]._v.z - high1[min[0]]._v.z;
            float xz = (float)Math.Atan2(z, x);

            Point v = high1[i];
            v.degreeXY = xy;
            v.degreeXZ = xz;
            high1[i] = v;
        }
        for (int i = 0; i < high2.Count; i++)
        {
            if (i == min[1]) continue;
            float x = high2[i]._v.x - high2[min[1]]._v.x;
            float y = high2[i]._v.y - high2[min[1]]._v.y;
            float xy = (float)Math.Atan2(y, x);

            float z = high2[i]._v.z - high2[min[1]]._v.z;
            float xz = (float)Math.Atan2(z, x);

            Point v = high2[i];
            v.degreeXY = xy;
            v.degreeXZ = xz;
            high2[i] = v;
        }
        for (int i = 0; i < high3.Count; i++)
        {
            if (i == min[2]) continue;
            float x = high3[i]._v.x - high3[min[2]]._v.x;
            float y = high3[i]._v.y - high3[min[2]]._v.y;
            float xy = (float)Math.Atan2(y, x);

            float z = high3[i]._v.z - high3[min[2]]._v.z;
            float xz = (float)Math.Atan2(z, x);

            Point v = high3[i];
            v.degreeXY = xy;
            v.degreeXZ = xz;
            high3[i] = v;
        }
        for (int i = 0; i < high4.Count; i++)
        {
            if (i == min[3]) continue;
            float x = high4[i]._v.x - high4[min[3]]._v.x;
            float y = high4[i]._v.y - high4[min[3]]._v.y;
            float xy = (float)Math.Atan2(y, x);

            float z = high4[i]._v.z - high4[min[3]]._v.z;
            float xz = (float)Math.Atan2(z, x);

            Point v = high4[i];
            v.degreeXY = xy;
            v.degreeXZ = xz;
            high4[i] = v;
        }
        //for (int i = 0; i < high5.Count; i++)
        //{
        //    if (i == min[4]) continue;
        //    float x = high5[i]._v.x - high5[min[4]]._v.x;
        //    float y = high5[i]._v.y - high5[min[4]]._v.y;
        //    float xy = (float)Math.Atan2(y, x);

        //    float z = high5[i]._v.z - high5[min[4]]._v.z;
        //    float xz = (float)Math.Atan2(z, x);

        //    Point v = high5[i];
        //    v.degreeXY = xy;
        //    v.degreeXZ = xz;
        //    high5[i] = v;
        //}

        // 각도가 좁은 순으로 정렬
        QuickSort1(0, high1.Count - 1);
        QuickSort2(0, high2.Count - 1);
        QuickSort3(0, high3.Count - 1);
        QuickSort4(0, high4.Count - 1);

        //반 시계 방향의 점만 외곽 점으로 
        //  hull.Add(vertices[0]);
        // hull.Add(vertices[1]);
        hull1.Add(high1[0]);
        hull1.Add(high1[1]);

        hull2.Add(high2[0]);
        hull2.Add(high2[1]);

        hull3.Add(high3[0]);
        hull3.Add(high3[1]);

        hull4.Add(high4[0]);
        hull4.Add(high4[1]);

        //hull5.Add(high5[0]);
        //hull5.Add(high5[1]);

      
        for (int k = 2; k < high1.Count; k++)
        {
            //int num = hull.Count;
            //Point b = hull[num - 1];
            //Point a = hull[num - 2];

            int num =  hull1.Count;
            Point b =  hull1[num - 1];
            Point a =  hull1[num-2];

            // 새로운 점이 시계방향일때
            if (!isCCW(a, b, high1[k]))
            {
                hull1.RemoveAt(num - 1);

                for (int i = 3; i <= num; i++)
                {
                    int index = num - i;
                    if (!isCCW(hull1[index], hull1[index + 1], high1[k]))
                    {
                        int n = hull1.Count - 1;
                        if (n > -1)
                            hull1.RemoveAt(n);
                    }
                    else break;
                }
            }
            hull1.Add(high1[k]);

            //if (!isCCW(a[1], b[1], high2[k]))
            //{
            //    hull2.RemoveAt(num[1] - 1);

            //    for (int i = 0; i <= num[1]; i++)
            //    {
            //        int index = num[1] - i;
            //        if (!isCCW(hull2[index], hull2[index + 1], high2[k]))
            //        {
            //            int n = hull2.Count - 1;
            //            if (n > -1)
            //                hull2.RemoveAt(n);
            //        }
            //        else break;
            //    }
            //}
            //hull2.Add(high2[k]);

            //if (!isCCW(a[2], b[2], high3[k]))
            //{
            //    hull3.RemoveAt(num[2] - 1);

            //    for (int i = 0; i <= num[2]; i++)
            //    {
            //        int index = num[2] - i;
            //        if (!isCCW(hull3[index], hull3[index + 1], high3[k]))
            //        {
            //            int n = hull3.Count - 1;
            //            if (n > -1)
            //                hull3.RemoveAt(n);
            //        }
            //        else break;
            //    }
            //}
            //hull3.Add(high3[k]);

            //if (!isCCW(a[3], b[3], high4[k]))
            //{
            //    hull4.RemoveAt(num[3] - 1);

            //    for (int i = 0; i <= num[3]; i++)
            //    {
            //        int index = num[3] - i;
            //        if (!isCCW(hull4[index], hull4[index + 1], high4[k]))
            //        {
            //            int n = hull4.Count - 1;
            //            if (n > -1)
            //                hull4.RemoveAt(n);
            //        }
            //        else break;
            //    }
            //}
            //hull4.Add(high4[k]);

            //if (!isCCW(a[4], b[4], high5[k]))
            //{
            //    hull5.RemoveAt(num[4] - 1);

            //    for (int i = 0; i <= num[4]; i++)
            //    {
            //        int index = num[4] - i;
            //        if (!isCCW(hull5[index], hull5[index + 1], high5[k]))
            //        {
            //            int n = hull5.Count - 1;
            //            if (n > -1)
            //                hull5.RemoveAt(n);
            //        }
            //        else break;
            //    }
            //}
            //hull5.Add(high5[k]);
        }

        for (int k = 2; k < high2.Count; k++)
        {
            int num = hull2.Count;
            Point b = hull2[num - 1];
            Point a =  hull2[num - 2];

            // 새로운 점이 시계방향일때

            if (!isCCW(a, b, high2[k]))
            {
                hull2.RemoveAt(num - 1);

                for (int i = 3; i <= num; i++)
                {
                    int index = num - i;
                    if (!isCCW(hull2[index], hull2[index + 1], high2[k]))
                    {
                        int n = hull2.Count - 1;
                        if (n > -1)
                            hull2.RemoveAt(n);
                    }
                    else break;
                }
            }
            hull2.Add(high2[k]);

        }


        //if (!isCCW(a, b, vertices[k]))
        //{
        //    //이전 점 제거
        //    hull.RemoveAt(num - 1);

        //    // 반시계방향이 될때까지 새로운 점과 그 이전 점들 비교
        //    for (int i = 3; i <= num; i++)
        //    {
        //        int index = num - i;
        //        if (!isCCW(hull[index], hull[index + 1], vertices[k]))
        //        {
        //            int n = hull.Count - 1;
        //            if( n >-1)
        //               hull.RemoveAt(n);
        //        }
        //        else break;
        //    }
        //}
        //hull.Add(vertices[k]);
        //    }
    }

    int GetHullSize() { return hull.Count; }
}