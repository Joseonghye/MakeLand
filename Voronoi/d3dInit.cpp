#include <cstdlib>
#include <ctime>
#include <math.h>
#include <vector>
#include <queue>
#include <noise/noise.h>
#include "noiseutils.h"
#include "StructType.h"
#include "ConvexHull.h"
using namespace noise;
using namespace std;

IDirect3DDevice9* Device = 0;
const int Width = 640;
const int Height = 480;

int nodes = 160;

vector<Point> vertices;
ConvexHull _convexHull;

IDirect3DVertexBuffer9* vb = 0;
IDirect3DVertexBuffer9* vbuffer = 0;
//IDirect3DVertexBuffer9* lineBuffer = 0;
IDirect3DIndexBuffer9* ib = 0;
IDirect3DIndexBuffer9* voronoi = 0;

DWORD FtoDW(float f) { return *((DWORD*)&f); }

struct Node
{
	vector<int> coners;

	Node(){}
};
struct Data
{
	int parent;
	int data;

	Data(){}
	Data(int d):data(d) {}
	Data(int p, int d) :parent(p), data(d) {}
};


struct Edge
{
	int index[2];
	float dist;

	Edge(int* i) { index[0] = i[0]; index[1] = i[1]; }
	Edge(int* i, float d):dist(d){ index[0] = i[0]; index[1] = i[1]; }

	bool isEqual(int* num)
	{
		if (index[0] == num[0] && index[1] == num[1]) return true;
		else if (index[0] == num[1] && index[1] == num[0]) return true;

		return false;
	}
};

struct Triangle 
{
	int v[3];

	Triangle() {}
	Triangle(int _v1, int _v2, int _v3) { v[0] = _v1; v[1] = _v2; v[2] = _v3; }

	int isOverlap(int* index )
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
vector<Triangle> triangles;

struct Circmcenter
{
	Vertex V;
	int index;
	vector<int>neighborIndex; //이웃하는 점 인덱스

	Circmcenter(Vertex _v,int _index) :V(_v), index(_index) { }

	void AddNeighbor(int n)
	{
		bool result = false;
		for (int i = 0; i < neighborIndex.size(); i++)
		{
			if (neighborIndex[i] == n)
			{
				result = true;
				break;
			}
		}
		if (!result) neighborIndex.push_back(n); 
	}

	void RemoveNeighbor(int n)
	{
		int num = 5;
		for (int i = 0; i < neighborIndex.size(); i++)
		{
			if (neighborIndex[i] == n)
			{
				num = i;
			}
			else if (i > num)
			{
				neighborIndex[i - 1] = neighborIndex[i];
				neighborIndex[i] = NULL;
			}
		}
	
	}

};
vector<Circmcenter> circumCenters;

float distance(Vertex center, Vertex v)
{
	float x = v._x - center._x;
	float y = v._y - center._y;
	float dist = sqrt((x*x) + (y*y));

	return dist;
}

bool InCircle(float r, Vertex center, int a, int b, int c)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i]._index == a || vertices[i]._index == b || vertices[i]._index == c)
			continue;
		if (distance(center, vertices[i]._v) <= r)
			return false;
	}
	return true;
}

//반시계 방향 여부 확인
int CCW(const Vertex & a, const Vertex & b, const Vertex& c)
{
	float orignX = b._x - a._x;
	float orignY = b._y - a._y;

	float nextX = c._x - a._x;
	float nextY = c._y - a._y;

	D3DXVECTOR3 orign(orignX, orignY, 0);
	D3DXVECTOR3 next(nextX, nextY, 0);
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &orign, &next);

	D3DXVECTOR3 up(0, 0, 1);

	float dot = D3DXVec3Dot(&normal, &up);
	if (dot > 0) return 1;
	else if (dot < -0.1f) return -1;
	else  return 0;
}

// 겹치는지 확인
bool isIntersection(Vertex a1, Vertex a2, Vertex b1, Vertex b2)
{
	int a = CCW(a1, a2, b1) * CCW(a1, a2, b2);
	int b = CCW(b1, b2, a1) * CCW(b1, b2, a2);

	if (a < 0 && b < 0)return true;
	else return false;
}

void FindTriangle(const Point& A, const Point& B, int prevIndex)
{
	Point C;

	int aIndex = A._index;
	int bIndex = B._index;
	int cIndex = -1;

	D3DXVECTOR3 vAB;
	vAB.x = (B._v._x - A._v._x);
	vAB.y = (B._v._y - A._v._y);
	vAB.z = 1.0f;

	Vertex midAB;
	midAB._x = A._v._x + (vAB.x *0.5f);
	midAB._y = A._v._y + (vAB.y *0.5f);

	bool tri = false;
	for (int j = 0; j < vertices.size(); j++)
	{
		if (j == A._index || j == B._index) continue;
		C = vertices[j];
		cIndex = C._index;

		D3DXVECTOR3 vAC;
		vAC.x = (C._v._x - A._v._x);
		vAC.y = (C._v._y - A._v._y);
		vAC.z = 1.0f;

		Vertex midAC;
		midAC._x = A._v._x + (vAC.x *0.5f);
		midAC._y = A._v._y + (vAC.y *0.5f);

		D3DXVECTOR3 N;
		D3DXVec3Cross(&N, &vAB, &vAC);

		D3DXVECTOR3 perpAB;
		D3DXVec3Cross(&perpAB, &vAB, &N);

		D3DXVECTOR3 perpAC;
		D3DXVec3Cross(&perpAC, &vAC, &N);

		// 교점 (외심) 찾기
		float t;
		float s;
		float under = perpAC.y*perpAB.x - perpAC.x*perpAB.y;
		if (under == 0) continue;

		float _t = perpAC.x*(midAB._y - midAC._y) - perpAC.y*(midAB._x - midAC._x);
		float _s = perpAB.x*(midAB._y - midAC._y) - perpAB.y*(midAB._x - midAC._x);

		t = _t / under;
		s = _s / under;
		if (t< -0.05f || t>1.0 || s< -0.05f || s>1.0) continue;
		if (_t == 0 && _s == 0) continue;

		Vertex center;
		center._x = midAB._x + t * perpAB.x;
		center._y = midAB._y + t * perpAB.y;

		//반지름
		float r = distance(center, A._v);

		// 원안에 다른점 
		if (InCircle(r, center, aIndex, bIndex, cIndex)) {

			int indices[3] = { aIndex, bIndex ,cIndex };
			bool equal = false;
			for (int i = 0; i < triangles.size(); i++)
			{
				//겹치는 삼각형 확인
				int overlapNum = triangles[i].isOverlap(indices);
				// 같은 삼각형이 있음
				if (overlapNum == 1)
				{
					if (prevIndex != -1 && i != prevIndex)
					{
						circumCenters[prevIndex].AddNeighbor(i);
						circumCenters[i].AddNeighbor(prevIndex);
					}

					equal = true;
					break;
				}
				//점 2개(선 1개)가 같은 삼각형이 있음
				else if (overlapNum == 0)
				{
					Vertex v[4];
					int num = 0;
					bool find[3] = { false,false,false };
					bool chk[3] = { false,false,false };
					for (int j = 0; j < 3; j++)
					{
						for (int k = 0; k < 3; k++)
						{
							if (indices[j] == triangles[i].v[k])
							{
								v[num++] = vertices[indices[j]]._v;
								find[j] = true;
								chk[k] = true;
								break;
							}
						}
						if (!find[j]) v[2] = vertices[indices[j]]._v;
					}

					for (int j = 0; j < 3; j++) {
						if (!chk[j])
						{
							v[3] = vertices[triangles[i].v[j]]._v;
							break;
						}
					}

					bool t = isIntersection(v[0], v[2], v[1], v[3]);
					bool s = isIntersection(v[0], v[3], v[1], v[2]);
					if (t)
					{
						float ax = distance(v[1], v[0]);
						float ay = distance(v[3], v[0]);

						float dgreeA = atan2f(ay, ax) * 180 / 3.1415f;

						float bx = distance(v[1], v[2]);
						float by = distance(v[3], v[2]);

						float dgreeB = atan2f(by, bx) * 180 / 3.1415f;

						if (dgreeA + dgreeB > 180)
						{
							// 삼각형 제거
							vector<Triangle> ::iterator it = triangles.begin();
							it += i;
							triangles.erase(it);

							// 삼각형의 외심 제거
							for (int j = 0; j < circumCenters[i].neighborIndex.size(); j++)
							{
								int num = circumCenters[i].neighborIndex[j];
								circumCenters[num].RemoveNeighbor(i);
							}
							vector<Circmcenter> ::iterator iter = circumCenters.begin();
							iter += i;
							circumCenters.erase(iter);

							break;
						}
						else
						{
							equal = true;

							circumCenters[prevIndex].AddNeighbor(i);
							circumCenters[i].AddNeighbor(prevIndex);
							break;
						}
					}
					else if (s)
					{

						float ax = distance(v[0], v[1]);
						float ay = distance(v[3], v[1]);

						float dgreeA = atan2f(ay, ax) * 180 / 3.1415f;

						float bx = distance(v[0], v[2]);
						float by = distance(v[3], v[2]);

						float dgreeB = atan2f(by, bx) * 180 / 3.1415f;

						if (dgreeA + dgreeB > 180)
						{
							vector<Triangle> ::iterator it = triangles.begin();
							it += i;
							triangles.erase(it);

							// 삼각형의 외심 제거
							for (int j = 0; j < circumCenters[i].neighborIndex.size(); j++)
							{
								int num = circumCenters[i].neighborIndex[j];
								circumCenters[num].RemoveNeighbor(i);
							}
							vector<Circmcenter> ::iterator iter = circumCenters.begin();
							iter += i;
							circumCenters.erase(iter);
							break;
						}
						else 
						{
							equal = true; 
							circumCenters[prevIndex].AddNeighbor(i);
							circumCenters[i].AddNeighbor(prevIndex);
							break;
						}
					}
				}
			}
			if (!equal) {

				if (prevIndex != -1)
					circumCenters[prevIndex].AddNeighbor(triangles.size());
				
				center._color = D3DCOLOR_XRGB(255, 0, 0);
				Circmcenter cc(center, circumCenters.size());

				//반시계 방향순서로
				if (ConvexHull::isCCW(A, B, C))
				{
					if (prevIndex != -1)
					{
						triangles.push_back(Triangle(aIndex, cIndex, bIndex));
						cc.AddNeighbor(prevIndex);
					}
					else
						triangles.push_back(Triangle(aIndex, cIndex, bIndex));
				}
				else {
					if (prevIndex != -1) 
					{
						triangles.push_back(Triangle(aIndex, bIndex, cIndex));
						cc.AddNeighbor(prevIndex);
					}
					else
						triangles.push_back(Triangle(aIndex, bIndex, cIndex));
				}				
				circumCenters.push_back(cc);

				tri = true;
				break;
			}

		}
	}

		if (tri)
		{
			int index = triangles.size() - 1;
			bool chkC = _convexHull.CheckInclude(cIndex);

			if (!chkC || !_convexHull.CheckInclude(bIndex))
				FindTriangle(B, C, index);
			if (!chkC || !_convexHull.CheckInclude(aIndex))
				FindTriangle(A, C, index);
		}
	
}

void SetDelaunayTriangle()
{
	Point A = _convexHull.hull[0];
	Point B = _convexHull.hull[1];
	FindTriangle(A, B,-1);
}


vector <Edge> edges;

void SetVoronoi()
{
	int t = triangles.size();
	int c = circumCenters.size();

	for (int i = 0; i < circumCenters.size(); i++)
	{
		for (int j = 0; j < circumCenters[i].neighborIndex.size(); j++)
		{
			int index[2] = { i,circumCenters[i].neighborIndex[j] };

			bool r = false;
			for (int k = 0; k < edges.size(); k++)
			{
				if (edges[k].isEqual(index)) 
				{
					r = true; break;
				}
			}
			if (!r) {
				float dist = distance(circumCenters[index[0]].V, circumCenters[index[1]].V);
				edges.push_back(Edge(index,dist));
			}
		}
	}

}

LPDIRECT3DTEXTURE9 pTexHeightMap = nullptr; // heightMap에 사용할 텍스쳐

//텍스쳐 초기화
HRESULT InitTexture()
{
	if (FAILED(D3DXCreateTextureFromFileEx(Device,
		"tutorial.bmp", D3DX_DEFAULT, D3DX_DEFAULT,
		D3DX_DEFAULT, 0,
		D3DFMT_X8B8G8R8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0,
		NULL, NULL, &pTexHeightMap)))
	{
		MessageBox(NULL, "Can't not open the texture file.", "Error", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT InitVB()
{
	// D3D에서 텍스처에 대한 정보를 저장하기 위한 구조체
	D3DSURFACE_DESC ddsd;
	  pTexHeightMap->GetLevelDesc( 0, &ddsd );
	// 텍스쳐 메모리의 포인터를 저장하기 위한 구조체
	D3DLOCKED_RECT d3drc;

	// 텍스쳐 메모리 Lock()
	// surface의 메모리에 접근할 수 있는 포인터 주소값을 얻는다
	pTexHeightMap->LockRect(0, &d3drc, NULL, D3DLOCK_READONLY);

	Device->CreateVertexBuffer(circumCenters.size() * sizeof(Vertex), D3DUSAGE_WRITEONLY,
		Vertex::FVF, D3DPOOL_MANAGED, &vbuffer, NULL);
	Vertex* vv;
	vbuffer->Lock(0, 0, (void**)&vv, 0);

	//이미지 비율 맞추기용
	float a = ddsd.Width / 200.0f;
	float b = ddsd.Height / 200.0f;

	for (int k = 0; k < circumCenters.size(); k++)
	{
		Vertex v = circumCenters[k].V;
		float x = (v._x + 100) *a;
		float y = (v._y + 100)*b;
		
		//좌표에 따른 높이 값
		// 텍스처의 색상 및 명암값이 저장된 메모리 주소에 접근하여,
		// 텍스처의 색상 및 명암값과 0x000000ff와 & 연산하면
		// 명암값을 얻어 올 수 있다. 값은 0 ~ 255 사이이다.
		float value = ((float)(*((LPDWORD)d3drc.pBits +(DWORD)x+ (DWORD)y * (d3drc.Pitch / 4)) & 0x000000ff));
		// 색상 변경
		if (value <=60 ) v._color = D3DCOLOR_XRGB(0, 0, 255);
		else v._color = D3DCOLOR_XRGB(150, 50, 0);

		vv[k] = v;

	}
	vbuffer->Unlock();
	pTexHeightMap->UnlockRect(0);

	return S_OK;
}

//	else if(value <=130) v._color = D3DCOLOR_XRGB(200, 160, 80);

void BFS(int start, int end)
{
	int cur = start;
	vector<float> space(circumCenters.size(), -1);
	vector<Data> path;
	queue<Data> q;

	space[start] = 0;
	Data s(-1, start);
	q.push(s);

	bool find = false;
	while (!q.empty() || find)
	{
		Data here = q.front();
		q.pop();

		int curIndex = here.data;

		for (int i = 0; i < circumCenters[curIndex].neighborIndex.size(); i++)
		{
			Data there(circumCenters[here.data].neighborIndex[i]);

			if (space[there.data] == -1)
			{
				there.parent = here.data;
				q.push(there);

				float dist = distance(circumCenters[there.data].V, circumCenters[curIndex].V);
				space[there.data] = dist;

				if (there.data == end) 
				{
					find = true;
					break;
				}
			}
		}
	}
}

//https://sungjk.github.io/2016/05/12/BFS.html
vector<int> Path(int start, int end, const vector<Data>& p)
{
	vector<int>path;

	int i = end;
	while (true)
	{
		int parent = p[i].parent;
		if (parent == -1) break;
		else
		{
			path.push_back(i);
			i = parent;
		}
	}
	return path;
}

bool Setup()
{
	// ----------- 랜덤 점 생성 -------------------//
	Device->CreateVertexBuffer(nodes * sizeof(Vertex), D3DUSAGE_WRITEONLY,
		Vertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
	Vertex* v;
	vb->Lock(0, 0, (void**)&v, 0);

	//srand((unsigned int)time(NULL));
	
	int yCount = 0;
	int num = 0;
	int quota = nodes / 4;

	while (num < nodes)
	{
		int xVal, yVal;
		D3DCOLOR c;

		int xNum = num % 4;

		switch (xNum)
		{
		case 0:
			xVal = -80;
			break;
		case 1:
			xVal = -40;
			break;
		case 2:
			xVal = 0;
			break;
		default:
			xVal = 40;
			break;
		}

		switch (yCount)
		{
		case 0:
			yVal = 40;
			break;
		case 1:
			yVal = 0;
			break;
		case 2:
			yVal = -40;
			break;
		default:
			yVal = -80;

			break;
		}

		int x = rand() % 40 + xVal;
		int y = rand() % 40 + yVal;

		v[num] = Vertex(x, y, 1, 0.0f, D3DCOLOR_XRGB(255, 255, 255));
		
		bool equal = false;
		for (int j = yCount * quota; j < vertices.size(); j++)
		{
			equal = vertices[j].isEqual(v[num]);
			if (equal) break;
		}
		if (!equal)
		{
			vertices.push_back(Point(v[num], num));
			num++;
			if (num %quota == 0 )
			{
				yCount++;
			}
		}

	}

	vb->Unlock();

	// ------- Convex hull 생성 ------------------//
	_convexHull.vertices = vertices;
	_convexHull.SetConvexHull();

	//삼각분할 찾기
	SetDelaunayTriangle();
	
	//들로네 삼각분할
	Device->CreateIndexBuffer((triangles.size()*3)* sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, 0);
	WORD* indices = 0;
	ib->Lock(0, 0, (void**)&indices, 0);
	for (int i = 0; i < triangles.size(); i++)
	{
		int num = i * 3;
		indices[num] = triangles[i].v[0]; 
		indices[num+1]= triangles[i].v[1]; 
		indices[num+2] = triangles[i].v[2];
	}
	
	ib->Unlock();
	
	module::Perlin _moduel;
	_moduel.SetOctaveCount(3);
	_moduel.SetFrequency(1.0);
	//_moduel.SetPersistence(0.25);
	utils::NoiseMap heightMap;

	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(_moduel);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(200, 200);
	//heightMapBuilder.SetBounds(8.0, 10.0, 1.0, 3.0);
	heightMapBuilder.SetBounds(5.0, 7.0, 3.0, 5.0);
	heightMapBuilder.Build();

	utils::RendererImage renderer;
	utils::Image img;

	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(img);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(img);
	writer.SetDestFilename("tutorial.bmp");
	writer.WriteDestFile();

	InitTexture();
	InitVB();
	// 외심
	/*
	Device->CreateVertexBuffer(circumCenters.size() * sizeof(Vertex), D3DUSAGE_WRITEONLY,
		Vertex::FVF, D3DPOOL_MANAGED, &vbuffer, NULL);
	Vertex* vv;
	vbuffer->Lock(0, 0, (void**)&vv, 0);

	//http://telnet.or.kr/sec_directx/
	//https://mooneegee.blogspot.com/2015/03/directx9_8.html
	//https://m.blog.naver.com/PostView.nhn?blogId=lifeisforu&logNo=80022423850&proxyReferer=https%3A%2F%2Fwww.google.com%2F
	//https://camcap.tistory.com/entry/Terrain-%EB%86%92%EC%9D%B4%EB%A7%B5%EA%B3%BC-%ED%85%8D%EC%8A%A4%EC%B2%98%EB%A7%81

	float h = heightMap.GetHeight() / 160;
	float w = heightMap.GetWidth() / 160;
	for (int k = 0; k < circumCenters.size(); k++)
	{
		Vertex v = circumCenters[k].V; 
		float value = heightMap.GetValue(v._x *w, v._y*h);
		if (value >= 0.0f) v._color = D3DCOLOR_XRGB(150, 50, 0);
		else v._color = D3DCOLOR_XRGB(0, 0, 255);

		vv[k] = v;
	
	}
	vbuffer->Unlock();
	*/

	SetVoronoi();

	
	Device->CreateIndexBuffer((edges.size() * 2) * sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &voronoi, 0);
	WORD* voro = 0;
	voronoi->Lock(0, 0, (void**)&voro, 0);
	
	int n = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		n = i * 2;
		voro[n] = edges[i].index[0];
		voro[n+1] = edges[i].index[1];
	}

	voronoi->Unlock();


	////카메라 지정
	D3DXVECTOR3 position(0.0f, 0.0f, -80.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);
	

	//투영 변환
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f,
		(float)Width / (float)Height, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(5.0f));
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(5.0f));
	//Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(2.0f));
	//Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(2.0f));
	//Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(2.0f));
	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

//	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(vb);
	d3d::Release<IDirect3DVertexBuffer9*>(vbuffer);
	//d3d::Release<IDirect3DVertexBuffer9*>(lineBuffer);
	d3d::Release<IDirect3DIndexBuffer9*>(ib);
	d3d::Release<IDirect3DIndexBuffer9*>(voronoi);

	if (pTexHeightMap != NULL)
		pTexHeightMap->Release();
}


bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000ff, 1.0f, 0);
		Device->BeginScene();

		//카메라 지정
		float x = 0;
		float y = 0;
		static float z = -80;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) x -= 3;
		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) x += 3;
		if (::GetAsyncKeyState(VK_UP) & 0x8000f) y += 1.0f;
		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f) y -= 1.0f;
		if (::GetAsyncKeyState(VK_HOME) & 0x8000f) z += 5* timeDelta;
		if (::GetAsyncKeyState(VK_END) & 0x8000f) z -= 5 *timeDelta;

		D3DXVECTOR3 position(x, y, z);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

	//	Device->SetStreamSource(0, vb, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		
	//	Device->DrawPrimitive(D3DPT_POINTLIST, 0, nodes);

	//	Device->SetIndices(ib);
	//	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nodes, 0, triangles.size());
		//Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);


		// == 외심 그리기
		Device->SetStreamSource(0, vbuffer, 0, sizeof(Vertex));
		Device->DrawPrimitive(D3DPT_POINTLIST, 0, circumCenters.size());
		
		Device->SetIndices(voronoi);
		Device->DrawIndexedPrimitive(D3DPT_LINELIST, 0,0, circumCenters.size(),0, edges.size());

		//Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, circumCenters.size());
	
		// == Convex Hull 그리기
		//Device->SetStreamSource(0, lineBuffer, 0, sizeof(Vertex));
		//Device->DrawPrimitive(D3DPT_LINESTRIP, 0,_convexHull.GetHullSize());

		Device->EndScene();
		// Swap the back and front buffers.
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}