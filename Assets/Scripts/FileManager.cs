using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

struct Triangle
{
    public int[] v;
    public Triangle(int _v1, int _v2, int _v3)
    {
        v = new int[3];
        v[0] = _v1; v[1] = _v2; v[2] = _v3;
    }

    public int isOverlap(int[] index)
    {
        int count = 0;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (index[i] == v[j]) count++;
            }
        }
        if (count == 3) return 1;
        else if (count == 2) return 0;
        return -1;
    }

};


public class FileManager : MonoBehaviour
{

    StreamReader reader;

    int stackSize = 1024 * 1024;

    public Material[] color;
    public GameObject dummy;
    List<Vector3> vertices;
    List<GameObject> objs;

    List<Point> floor;
   // ConvexHull convex;
    List<Triangle> triangles;

    Point[] p1;
    int prev1;
    Point[] p2;
    int prev2;
    bool con1 = true;
    bool con2 = true;

    ContourLine[] contourLine;

    // Use this for initialization
    void Start()
    {
        p1 = new Point[2];
        prev1 = -1;
        p2 = new Point[2];
        prev2 = -1;

        vertices = new List<Vector3>();
        objs = new List<GameObject>();

        floor = new List<Point>();

       triangles = new List<Triangle>();

        contourLine = new ContourLine[4];
        for(int i=0; i<4; i++)
        {
            contourLine[i] = new ContourLine();
        }

        GetVertices();
    }

    void GetVertices()
    {
        // csv 읽기
        FileInfo source = new FileInfo(Application.dataPath + "/Resources/vertex3.csv");
        TextReader reader = source.OpenText();

        string s = reader.ReadLine();

        int num = 0;
        while (s != null)
        {
            string[] temp = s.Split(',');

            float[] f = new float[temp.Length];
            for (int i = 0; i < temp.Length; i++)
            {
                f[i] = float.Parse(temp[i]);
            }

            Vector3 v = new Vector3(f[0], f[2], f[1]);

            float y = 0.0f;

            //점 위치 표시 
            GameObject ob = Instantiate(dummy, v, Quaternion.identity);

            if (f[2] < 95)
                ob.GetComponent<MeshRenderer>().material = color[0];
            else
            {
                floor.Add(new Point(v, floor.Count));

                if (f[2] >= 95 && f[2] < 135)
                {
                    y = 95.0f;
                    contourLine[0].AddVertex(new Point(v, num));
                    ob.GetComponent<MeshRenderer>().material = color[1];
                }
                else if (f[2] >= 135 && f[2] < 175)
                {
                    y = 135.0f;
                    contourLine[1].AddVertex(new Point(v, num));
                    ob.GetComponent<MeshRenderer>().material = color[2];
                }
                else if (f[2] >= 175 && f[2] < 215)
                {
                    y = 175.0f;
                    contourLine[2].AddVertex(new Point(v, num));
                    ob.GetComponent<MeshRenderer>().material = color[3];
                }
                else if (f[2] >= 215 && f[2] < 256)
                {
                    y = 215.0f;
                    contourLine[3].AddVertex(new Point(v, num));
                    ob.GetComponent<MeshRenderer>().material = color[4];
                }
            }
            vertices.Add(new Vector3(f[0],y,f[1]));

            objs.Add(ob);
            num++;
            s = reader.ReadLine();
        }

        source = null;
        reader.Dispose();
        reader = null;

        int count = 0;
        //while(true)
        //{

        //}

        for (int i = 0; i < 4; i++)
        {
            contourLine[i].GetConveHull();
        }

        float ox = (contourLine[1].convex[1]._v.x - contourLine[1].convex[0]._v.x) / 2;
        float oz = (contourLine[1].convex[1]._v.z - contourLine[1].convex[0]._v.z) / 2;

        Vector3 orignDown = contourLine[1].convex[0]._v + new Vector3(ox,0, oz);
       // Debug.Log(orignDown);
     //   GameObject o = Instantiate(dummy, orignDown, Quaternion.identity);

        List<Vector3> vs = new List<Vector3>();
        float min = 1000;
        int minIndex = -1;
        int index = -1;

        for (int i = 0; i < contourLine[0].convex.Count; i++)
        {
            int oi = contourLine[0].convex[i]._index;
            float oy = vertices[oi].y;

            orignDown.y = oy;

            Vector3 vA = contourLine[0].convex[i]._v;
            vA.y = oy;

            Vector3 vB;
            if (i + 1 == contourLine[0].convex.Count)
                vB = contourLine[0].convex[0]._v;
            else
                vB = contourLine[0].convex[i + 1]._v;

            vB.y = oy;

            Vector3 AB = vB - vA;
            Vector3 AP = orignDown - vA;

            float sAB = (AB.x * AB.x) + (AB.y * AB.y) + (AB.z * AB.z);
            sAB = Mathf.Sqrt(sAB);
            float sAP = (AP.x * AP.x) + (AP.y * AP.y) + (AP.z * AP.z);
            sAP = Mathf.Sqrt(sAP);

            float dot = (AB.x * AP.x) + (AB.y * AP.y) + (AB.z * AP.z);
            float under = sAB * sAP;
            Vector3 dir = (AB * dot) / under;
            Vector3 V = vA + dir;
            vs.Add(V);

          //  Debug.Log("vA = " + vA + "/ vB = " + vB + "/ Dir = " +dir + "/ V = " + V);

            float dx = V.x - orignDown.x;
            float dz = V.z - orignDown.z;

            /*
            float a = vB.z - vA.z;
            float b = vB.x - vA.x;
            float c = -(a * vA.x) + vA.z;

            float k = (a * orignDown.x) + (b * orignDown.z) + c;
            float under = (a * a) + (b * b);
            k /= -under;

            float x = a * k + orignDown.x;
            float z = b * k + orignDown.z;

            Vector3 n = new Vector3(x, orignDown.y, z);
            vs.Add(n);

            Debug.Log("vA = " + vA + "/ vB = " + vB + "/ n = " + n);

            float dx = n.x - orignDown.x;
            float dz = n.z - orignDown.z;
             */

            float d = Mathf.Sqrt((dx * dx) + (dz * dz));
            if (min > d)
            {
                minIndex = vs.Count;
                min = d;
                index = i;
            }
        }
        vertices.Add(vs[minIndex]);


        for(int i=0; i<= index; i++)
        {
            if( i == index)
            {
                triangles.Add(new Triangle(contourLine[0].convex[i]._index, contourLine[1].convex[0]._index, vertices.Count - 1));
            }
            else
                triangles.Add(new Triangle(contourLine[0].convex[i]._index, contourLine[1].convex[0]._index, contourLine[0].convex[i+1]._index));
        }
    //    Debug.Log(vs[minIndex]);
        triangles.Add(new Triangle(vertices.Count-1, contourLine[1].convex[0]._index, contourLine[1].convex[1]._index));

      

        //Convex hull 생성 
       // convex = new ConvexHull(floor);
        //convex.SetConvexHull();
        
        /*
        for(int i=0; i<4; i++)
        {
            contourLine[i].GetConveHull();
        }

        int count1 = contourLine[0].convex.Count;
        int count2 = contourLine[1].convex.Count;

        for (int i = 0; i < count1; i++)
        {
            int j = i + 1;
            if (j < count2)
            {
                triangles.Add(new Triangle(contourLine[0].convex[i]._index, contourLine[1].convex[i]._index, contourLine[1].convex[j]._index));
                if (j < count1)
                    triangles.Add(new Triangle(contourLine[0].convex[i]._index, contourLine[1].convex[j]._index, contourLine[0].convex[j]._index));
            }
            else if (j < count1)
            {
                triangles.Add(new Triangle(contourLine[0].convex[i]._index, contourLine[1].convex[count2 - 1]._index, contourLine[0].convex[j]._index));
            }
        }
        triangles.Add(new Triangle(contourLine[0].convex[count1 - 1]._index, contourLine[1].convex[count2 - 1]._index, contourLine[1].convex[0]._index));
        triangles.Add(new Triangle(contourLine[1].convex[count2 - 1]._index, contourLine[1].convex[0]._index, contourLine[0].convex[0]._index));

        int cha = count2 - count1;
        if (cha > 0)
        {
            for (int n = count1; n < count2; n++)
            {
                int j = n + 1;
                if (j < count2)
                {
                    triangles.Add(new Triangle(contourLine[0].convex[count1 - 1]._index, contourLine[1].convex[n]._index, contourLine[1].convex[j]._index));
                }
                else
                    triangles.Add(new Triangle(contourLine[0].convex[count1 - 1]._index, contourLine[1].convex[n]._index, contourLine[1].convex[n - 1]._index));
            }
        }

    //    -----------
        int count3 = contourLine[2].convex.Count;

        for (int i = 0; i < count2; i++)
        {
            int j = i + 1;
            if (j < count3)
            {
                triangles.Add(new Triangle(contourLine[1].convex[i]._index, contourLine[2].convex[i]._index, contourLine[2].convex[j]._index));
                if (j < count2)
                    triangles.Add(new Triangle(contourLine[1].convex[i]._index, contourLine[2].convex[j]._index, contourLine[1].convex[j]._index));
            }
            else if (j < count2)
            {
                triangles.Add(new Triangle(contourLine[1].convex[i]._index, contourLine[2].convex[count3 - 1]._index, contourLine[1].convex[j]._index));
            }
        }
        triangles.Add(new Triangle(contourLine[1].convex[count2 - 1]._index, contourLine[2].convex[count3 - 1]._index, contourLine[2].convex[0]._index));
        triangles.Add(new Triangle(contourLine[2].convex[count3 - 1]._index, contourLine[2].convex[0]._index, contourLine[1].convex[0]._index));

         cha = count3 - count2;
        if (cha > 0)
        {
            for (int n = count2; n < count3; n++)
            {
                int j = n + 1;
                if (j < count3)
                {
                    triangles.Add(new Triangle(contourLine[1].convex[count2 - 1]._index, contourLine[2].convex[n]._index, contourLine[2].convex[j]._index));
                }
                else
                    triangles.Add(new Triangle(contourLine[1].convex[count2 - 1]._index, contourLine[2].convex[n]._index, contourLine[2].convex[n - 1]._index));
            }
        }

        //-----------
        int count4 = contourLine[3].convex.Count;

        for (int i = 0; i < count3; i++)
        {
            int j = i + 1;
            if (j < count4)
            {
                triangles.Add(new Triangle(contourLine[2].convex[i]._index, contourLine[3].convex[i]._index, contourLine[3].convex[j]._index));
                if (j < count3)
                    triangles.Add(new Triangle(contourLine[2].convex[i]._index, contourLine[3].convex[j]._index, contourLine[2].convex[j]._index));
            }
            else if (j < count3)
            {
                triangles.Add(new Triangle(contourLine[2].convex[i]._index, contourLine[3].convex[count4 - 1]._index, contourLine[2].convex[j]._index));
            }
        }
        triangles.Add(new Triangle(contourLine[2].convex[count3 - 1]._index, contourLine[3].convex[count4 - 1]._index, contourLine[3].convex[0]._index));
        triangles.Add(new Triangle(contourLine[3].convex[count4 - 1]._index, contourLine[3].convex[0]._index, contourLine[2].convex[0]._index));

        cha = count4 - count3;
        if (cha > 0)
        {
            for (int n = count3; n < count4; n++)
            {
                int j = n + 1;
                if (j < count4)
                {
                    triangles.Add(new Triangle(contourLine[2].convex[count2 - 1]._index, contourLine[3].convex[n]._index, contourLine[3].convex[j]._index));
                }
                else
                    triangles.Add(new Triangle(contourLine[2].convex[count2 - 1]._index, contourLine[3].convex[n]._index, contourLine[3].convex[n - 1]._index));
            }
        }
        */

        GetMesh();
    }

    Vector3 GetIntersectPoint(Vector3 a1, Vector3 a2, Vector3 b)
    {
        Vector3 b2 = b;
        b2.y = 0.0f;

        float under = (b2.y - b.y) * (a2.x - a1.x) - (b2.x - b.x) * (a2.y - a1.y);
        float _t = (b2.x - b.x) * (a1.y - b.y) - (b2.y - b.y) * (a1.x - b.x);

        float underZ = (b2.y - b.y) * (a2.x - a1.x) - (b2.x - b.x) * (a2.y - a1.y);
        float _s = (a2.x - a1.x) * (a1.z - b.z) - (a2.z - a1.z) * (a1.x - b.x);

        float t = _t / under;
        float s = _s / underZ;

        float x = a1.x + t * (a2.x - a1 .x);
        float y = a1.y + t * (a2.y - a1.y);
        float z = a1.z + s * (a2.z - a1.z);
        return new Vector3(x,y,z);
    }
    void GetMesh()
    {
        LineRenderer line = GetComponent<LineRenderer>();
        line.positionCount = contourLine[0].convex.Count;
        for (int i = 0; i < contourLine[0].convex.Count; i++)
        {
            int index = contourLine[0].convex[i]._index;
            line.SetPosition(i, vertices[index]);
        }

        int num = contourLine[0].convex.Count;
        line.positionCount += contourLine[1].convex.Count;
        for (int i = 0; i < contourLine[1].convex.Count; i++)
        {
            int index = contourLine[1].convex[i]._index;
            line.SetPosition(num + i, vertices[index]);
        }

        //line.positionCount = triangles.Count * 3;
        //for (int i = 0; i < triangles.Count; i++)
        //{
        //    int[] index = { triangles[i].v[0], triangles[i].v[1], triangles[i].v[2] };
        //    int num = i * 3;

        //    line.SetPosition(num, vertices[index[0]]);
        //    line.SetPosition(num + 1, vertices[index[1]]);
        //    line.SetPosition(num + 2, vertices[index[2]]);
        //}


        MeshFilter mf = GetComponent<MeshFilter>();
        Mesh m = new Mesh();

        //List<Vector3> v = new List<Vector3>();
        //for(int i =0; i < floor.Count; i++)
        //{
        //    v.Add(floor[i]._v);
        //}

        List<int> indices = new List<int>();
        for(int i=0; i< triangles.Count; i ++)
        {
            indices.AddRange(triangles[i].v);
        }

        m.vertices = vertices.ToArray(); 
        m.triangles = indices.ToArray();
        mf.mesh = m;
    }

    float distance(Vector3 center, Vector3 v)
    {
        float x = v.x - center.x;
        float y = v.y - center.y;
        //  float z = v.z - center.z;
        //  float dist = Mathf.Sqrt((x * x) + (y * y) + (z*z));
        float dist = Mathf.Sqrt((x * x) + (y * y));
        return dist;
    }

    bool InCircle(float r, Vector3 center, int a, int b, int c)
    {
        for (int i = 0; i < floor.Count; i++)
        {
            if (floor[i]._index == a || floor[i]._index == b || floor[i]._index == c)
                continue;
            if (distance(center, floor[i]._v) <= r)
                return false;
        }
        return true;
    }

    int CCW(Vector3 a, Vector3 b, Vector3 c)
    {
        float orignX = b.x - a.x;
        float orignY = b.y - a.y;

        float nextX = c.x - a.x;
        float nextY = c.y - a.y;

        Vector3 orign = new Vector3(orignX, orignY, 0);
        Vector3 next = new Vector3(nextX, nextY, 0);
        Vector3 normal = Vector3.Cross(orign, next);

        Vector3 up = new Vector3(0, 0, 1);

        float dot = Vector3.Dot(normal, up);
        if (dot > 0) return 1;
        else if (dot < -0.1f) return -1;
        else return 0;
    }

    bool isIntersection(Vector3 a1, Vector3 a2, Vector3 b1, Vector3 b2)
    {
        int a = CCW(a1, a2, b1) * CCW(a1, a2, b2);
        int b = CCW(b1, b2, a1) * CCW(b1, b2, a2);

        if (a < 0 && b < 0) return true;
        else return false;
    }

    
    /*
    void FindTriangle(Point A, Point B, int prevIndex)
    {
        Point C = new Point();

        int aIndex = A._index;
        int bIndex = B._index;
        int cIndex = -1;

        Vector3 vAB;
        vAB.x = (B._v.x - A._v.x);
        vAB.y = (B._v.y - A._v.y);
        vAB.z = 1.0f;

        Vector3 midAB;
        midAB.x = A._v.x + (vAB.x * 0.5f);
        midAB.y = A._v.y + (vAB.y * 0.5f);

        bool tri = false;
        for (int j = 0; j < floor.Count; j++)
        {
            if (j == A._index || j == B._index) continue;
            C = floor[j];
            cIndex = C._index;

            Vector3 vAC;
            vAC.x = (C._v.x - A._v.x);
            vAC.y = (C._v.y - A._v.y);
            vAC.z = 1.0f;

            Vector3 midAC;
            midAC.x = A._v.x + (vAC.x * 0.5f);
            midAC.y = A._v.y + (vAC.y * 0.5f);

            Vector3 N = Vector3.Cross(vAB, vAC);
            //       Vector3(&N, &vAB, &vAC);

            Vector3 perpAB = Vector3.Cross(vAB, N);
            //   D3DXVec3Cross(&perpAB, &vAB, &N);

            Vector3 perpAC = Vector3.Cross(vAC, N);
            // D3DXVec3Cross(&perpAC, &vAC, &N);

            // 교점 (외심) 찾기
            float t;
            float s;
            float under = perpAC.y * perpAB.x - perpAC.x * perpAB.y;
            if (under == 0) continue;

            float _t = perpAC.x * (midAB.y - midAC.y) - perpAC.y * (midAB.x - midAC.x);
            float _s = perpAB.x * (midAB.y - midAC.y) - perpAB.y * (midAB.x - midAC.x);

            t = _t / under;
            s = _s / under;
            if (t < -0.05f || t > 1.0 || s < -0.05f || s > 1.0) continue;
            if (_t == 0 && _s == 0) continue;

            Vector3 center = new Vector3();
            center.x = midAB.x + t * perpAB.x;
            center.y = midAB.y + t * perpAB.y;

            //반지름
            float r = distance(center, A._v);

            // 원안에 다른점 
            if (InCircle(r, center, aIndex, bIndex, cIndex))
            {

                int[] indices = { aIndex, bIndex, cIndex };
                bool equal = false;
                
                for (int i = 0; i < triangles.Count; i++)
                {
                    //겹치는 삼각형 확인
                    int overlapNum = triangles[i].isOverlap(indices);
                    // 같은 삼각형이 있음
                    if (overlapNum == 1)
                    {
                        //if (prevIndex != -1 && i != prevIndex)
                        //{
                        //    circumCenters[prevIndex].AddNeighbor(i);
                        //    circumCenters[i].AddNeighbor(prevIndex);
                        //}

                        equal = true;
                        break;
                    }
                    //점 2개(선 1개)가 같은 삼각형이 있음
                    else if (overlapNum == 0)
                    {
                        Vector3[] v = new Vector3[4];
                        int num = 0;
                        bool[] find = { false, false, false };
                        bool[] chk = { false, false, false };
                        for (int n = 0; n < 3; n++)
                        {
                            for (int k = 0; k < 3; k++)
                            {
                                if (indices[n] == triangles[i].v[k])
                                {
                                    v[num++] = floor[indices[n]]._v;
                                    find[n] = true;
                                    chk[k] = true;
                                    break;
                                }
                            }
                            if (!find[n]) v[2] = floor[indices[n]]._v;
                        }

                        for (int z = 0; z < 3; z++)
                        {
                            if (!chk[z])
                            {
                                v[3] = floor[triangles[i].v[z]]._v;
                                break;
                            }
                        }

                        bool t2 = isIntersection(v[0], v[2], v[1], v[3]);
                        bool s2 = isIntersection(v[0], v[3], v[1], v[2]);
                        if (t2)
                        {
                            float ax = distance(v[1], v[0]);
                            float ay = distance(v[3], v[0]);

                            float dgreeA = Mathf.Atan2(ay, ax) * 180 / 3.1415f;

                            float bx = distance(v[1], v[2]);
                            float by = distance(v[3], v[2]);

                            float dgreeB = Mathf.Atan2(by, bx) * 180 / 3.1415f;

                            if (dgreeA + dgreeB > 180)
                            {
                                // 삼각형 제거
                                triangles.RemoveAt(i);

                                //// 삼각형의 외심 제거
                                //for (int j = 0; j < circumCenters[i].neighborIndex.size(); j++)
                                //{
                                //    int num = circumCenters[i].neighborIndex[j];
                                //    circumCenters[num].RemoveNeighbor(i);
                                //}
                                //vector<Circmcenter> ::iterator iter = circumCenters.begin();
                                //iter += i;
                                //circumCenters.erase(iter);

                                break;
                            }
                            else
                            {
                                equal = true;

                                // circumCenters[prevIndex].AddNeighbor(i);
                                // circumCenters[i].AddNeighbor(prevIndex);
                                break;
                            }
                        }
                        else if (s2)
                        {

                            float ax = distance(v[0], v[1]);
                            float ay = distance(v[3], v[1]);

                            float dgreeA = Mathf.Atan2(ay, ax) * 180 / 3.1415f;

                            float bx = distance(v[0], v[2]);
                            float by = distance(v[3], v[2]);

                            float dgreeB = Mathf.Atan2(by, bx) * 180 / 3.1415f;

                            if (dgreeA + dgreeB > 180)
                            {
                                triangles.RemoveAt(i);

                                //// 삼각형의 외심 제거
                                //for (int j = 0; j < circumCenters[i].neighborIndex.size(); j++)
                                //{
                                //    int num = circumCenters[i].neighborIndex[j];
                                //    circumCenters[num].RemoveNeighbor(i);
                                //}
                                //vector<Circmcenter> ::iterator iter = circumCenters.begin();
                                //iter += i;
                                //circumCenters.erase(iter);
                                break;
                            }
                            else
                            {
                                equal = true;
                                //circumCenters[prevIndex].AddNeighbor(i);
                                //circumCenters[i].AddNeighbor(prevIndex);
                                break;
                            }
                        }
                    }
                }
                if (!equal)
                {

                    // if (prevIndex != -1)
                    //   circumCenters[prevIndex].AddNeighbor(triangles.size());

                    //center._color = D3DCOLOR_XRGB(255, 0, 0);
                    // Circmcenter cc(center, circumCenters.size());

                    //반시계 방향순서로
                    if (convex.isCCW(A, B, C))
                    {
                        //   if (prevIndex != -1)
                        //   {
                        triangles.Add(new Triangle(aIndex, cIndex, bIndex));
                        //  triangles.push_back(Triangle(aIndex, cIndex, bIndex));
                        //   cc.AddNeighbor(prevIndex);
                    }
                    //     else
                    //         triangles.push_back(Triangle(aIndex, cIndex, bIndex));
                }
                else
                {
                    //  if (prevIndex != -1)
                    // {
                    //   triangles.push_back(Triangle(aIndex, bIndex, cIndex));
                    //    cc.AddNeighbor(prevIndex);
                    // }
                    //else
                    //  triangles.push_back(Triangle(aIndex, bIndex, cIndex));
                    triangles.Add(new Triangle(aIndex, bIndex, cIndex));
                }
                //    circumCenters.push_back(cc);

                tri = true;
                break;
            }

        }

        con1 = false;
        con2 = false;
        if (tri)
        {
            int index = triangles.Count - 1;
            bool chkC = convex.CheckInclude(cIndex);


            if (!chkC)
            {
                if (!convex.CheckInclude(bIndex))
                {
                    p1[0] = B;
                    p1[1] = C;
                    prev1 = index;
                    con1 = true;
                    //   Thread thread1 = new Thread(() => { FindTriangle(B, C, index); }, stackSize);
                    // thread1.Start();
                }
                 if (!convex.CheckInclude(aIndex))
                {
                    p2[0] = A;
                    p2[1] = C;
                    prev2= index;
                    con2 = true;
                    //    Thread thread2 = new Thread(() => { FindTriangle(A, C, index); }, stackSize);
                    //  thread2.Start();
                }
            }
        }
    }

    void SetDelaunayTriangle()
    {
        Point A = convex.hull[0];
        Point B = convex.hull[1];
        FindTriangle(A, B, -1);
    }
    */
}