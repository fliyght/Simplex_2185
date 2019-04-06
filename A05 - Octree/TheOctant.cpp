#include "TheOctant.h"
using namespace Simplex;

uint TheOctant::m_uOctantCount = 0;
uint TheOctant::m_uMaxLevel = 3;
uint TheOctant::m_uIdealEntityCount = 5;
uint TheOctant::GetOctantCount(void) { return m_uOctantCount; }

void TheOctant::Init(void)
{
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_v3Center = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}

void TheOctant::Swap(TheOctant & other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

TheOctant * TheOctant::GetParent(void){ return m_pParent; }

void TheOctant::Release(void)
{
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

TheOctant::TheOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax;

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;
	for (int i = 1; i < 3; i++)
	{
		if (fMax < vHalfWidth[i])
		{
			fMax = vHalfWidth[i];
		}
	}
	vector3 v3Center = pRigidBody->GetCenterLocal();
	lMinMax.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Max = m_v3Center + (vector3(fMax));
	m_v3Min = m_v3Center - (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}

TheOctant::TheOctant(vector3 a_nCenter, float a_nSize)
{
	Init();
	m_v3Center = a_nCenter;
	m_fSize = a_nSize;

	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

TheOctant::TheOctant(TheOctant const & other)
{
	m_uChildren, other.m_uChildren;
	m_v3Center, other.m_v3Center;
	m_v3Max, other.m_v3Max;
	m_v3Min, other.m_v3Min;

	m_fSize, other.m_fSize;
	m_uID, other.m_uID;
	m_uLevel, other.m_uLevel;
	m_pParent, other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i], other.m_pChild[i];
	}
}

TheOctant &TheOctant::operator=(TheOctant const & other)
{
	if (this != &other)
	{
		Release();
		Init();
		TheOctant temp(other);
		Swap(temp);
	}
	return *this;
}

TheOctant::~TheOctant(void){ Release(); }
float TheOctant::GetSize(void){ return m_fSize; }
vector3 TheOctant::GetCenterGlobal(void){ return m_v3Center; }
vector3 TheOctant::GetMinGlobal(void){ return m_v3Min; }
vector3 TheOctant::GetMaxGlobal(void){ return m_v3Max; }

void TheOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_nIndex);
	}
}

void TheOctant::Display(vector3 a_v3Color)
{
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void TheOctant::Subdivide(void)
{
	printf("%d %d\n", m_uLevel, m_uMaxLevel);

	if (m_uLevel >= m_uMaxLevel)
		return;
	if (m_uChildren != 0)
		return;

	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new TheOctant(v3Center, fSizeD);

	v3Center.x += fSizeD;
	m_pChild[1] = new TheOctant(v3Center, fSizeD);

	v3Center.z += fSizeD;
	m_pChild[2] = new TheOctant(v3Center, fSizeD);

	v3Center.x -= fSizeD;
	m_pChild[3] = new TheOctant(v3Center, fSizeD);

	v3Center.y += fSizeD;
	m_pChild[4] = new TheOctant(v3Center, fSizeD);

	v3Center.z -= fSizeD;
	m_pChild[5] = new TheOctant(v3Center, fSizeD);

	v3Center.x += fSizeD;
	m_pChild[6] = new TheOctant(v3Center, fSizeD);

	v3Center.z += fSizeD;
	m_pChild[7] = new TheOctant(v3Center, fSizeD);

	for (uint nIndex = 0; nIndex < 8; nIndex++)
	{
		m_pChild[nIndex]->m_pRoot = m_pRoot;
		m_pChild[nIndex]->m_pParent = this;
		m_pChild[nIndex]->m_uLevel = m_uLevel + 1;
		if (m_pChild[nIndex]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[nIndex]->Subdivide();
		}
	}
}

TheOctant * TheOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
	{
		return nullptr;
	}
	return m_pChild[a_nChild];
}

bool TheOctant::IsColliding(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	if (a_uRBIndex >= nObjectCount)
		return false;

	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MaxD = pRigidBody->GetMaxGlobal();
	vector3 v3MinD = pRigidBody->GetMinGlobal();

	if (m_v3Max.x < v3MinD.x)
		return false;
	if (m_v3Min.x > v3MaxD.x)
		return false;

	if (m_v3Max.y < v3MinD.y)
		return false;
	if (m_v3Min.y > v3MaxD.y)
		return false;

	if (m_v3Max.z < v3MinD.z)
		return false;
	if (m_v3Min.z > v3MaxD.z)
		return false;
}

bool TheOctant::IsLeaf(void){ return m_uChildren == 0; }

bool TheOctant::ContainsMoreThan(uint a_nEntites)
{
	uint nCount = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	for (uint n = 0; n < nObjectCount; n++)
	{
		if (IsColliding(n))
			nCount++;
		if (nCount > a_nEntites)
			return true;
	}
	return false;
}

void TheOctant::KillBranches(void)
{
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}

void TheOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint nLeafs = m_lChild.size();

	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void TheOctant::ClearEntityList(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}

void TheOctant::ConstructTree(uint a_nMaxLevel)
{
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDToEntity();
	ConstructList();
}

void TheOctant::AssignIDToEntity(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->AssignIDToEntity();
	}
	if (m_uChildren == 0)
	{
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (uint nIndex = 0; nIndex < nEntities; nIndex++)
		{
			if (IsColliding(nIndex))
			{
				m_EntityList.push_back(nIndex);
				m_pEntityMngr->AddDimension(nIndex, m_uID);
			}
		}
	}
}

void TheOctant::ConstructList(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}

void TheOctant::CheckCollisions()
{
	if (m_uChildren != 0)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChild[i]->CheckCollisions();

			// dont check for collisions if the chlidren already did it
			return;
		}
	}

	//Clear all collisions
	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		m_pEntityMngr->GetEntity(m_EntityList[i])->ClearCollisionList();
	}

	//check collisions
	for (uint i = 0; i < m_EntityList.size() - 1; i++)
	{
		for (uint j = i + 1; j < m_EntityList.size(); j++)
		{
			m_pEntityMngr->GetEntity(m_EntityList[i])->IsColliding(m_pEntityMngr->GetEntity(m_EntityList[j]));
		}
	}
}