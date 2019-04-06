#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{
	class MyOctant
	{
		static uint m_uOctantCount;
		static uint m_uMaxLevel;
		static uint m_uIdealEntityCount;

		uint m_uID = 0;
		uint m_uLevel = 0;
		uint m_uChildren = 0;

		float m_fSize = 0.0f;

		MeshManager* m_pMeshMngr = nullptr;
		MyEntityManager* m_pEntityMngr = nullptr;

		vector3 m_vCenter = vector3(0.0f);
		vector3 m_vMax = vector3(0.0f);
		vector3 m_vMin = vector3(0.0f);

		MyOctant* m_pParent = nullptr;
		MyOctant* m_pChild[8];

		std::vector<uint> m_EntityList;

		MyOctant* m_pRoot = nullptr;
		std::vector<MyOctant*> m_lChild;

	public:
		MyOctant(uint m_uMaxLevel = 2, uint m_uIdealEntityCount = 5);
		MyOctant(vector3 m_vCenter, float m_fSize);
		MyOctant(MyOctant const& other);
		MyOctant& operator=(MyOctant const& other);
		~MyOctant(void);
		void Swap(MyOctant& other);
		float GetSize(void);
		vector3 GetCenterGlobal(void);
		vector3 GetMinGlobal(void);
		vector3 GetMaxGlobal(void);
		bool IsColliding(uint a_uRBIndex);
		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
		void Display(vector3 a_v3Color = C_YELLOW);
		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);
		void ClearEntityList(void);
		void Subdivide(void);
		MyOctant* GetChild(uint m_nChild);
		MyOctant* GetParent(void);
		bool IsLeaf(void);
		bool ContainsMoreThan(uint a_nEntites);
		void KillBranches(void);
		void ConstructTree(uint a_nMaxLevel = 3);
		void AssignIDToEntity(void);
		uint GetOctantCount(void);
	private:
		void Relese(void);
		void Init(void);
		void ConstructList(void);
	};
}
#endif // __MYOCTANTCLASS_H_