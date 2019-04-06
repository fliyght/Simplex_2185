#ifndef __THEOCTANTCLASS_H_
#define __THEOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{
	class TheOctant
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

		vector3 m_v3Center = vector3(0.0f);
		vector3 m_v3Max = vector3(0.0f);
		vector3 m_v3Min = vector3(0.0f);

		TheOctant* m_pParent = nullptr;
		TheOctant* m_pChild[8];

		std::vector<uint> m_EntityList;

		TheOctant* m_pRoot = nullptr;
		std::vector<TheOctant*> m_lChild;

	public:
		TheOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);
		TheOctant(vector3 a_nCenter, float a_nSize);
		TheOctant(TheOctant const& other);
		TheOctant& operator=(TheOctant const& other);
		~TheOctant(void);
		void Swap(TheOctant& other);
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
		TheOctant* GetChild(uint a_nChild);
		TheOctant* GetParent(void);
		bool IsLeaf(void);
		bool ContainsMoreThan(uint a_nEntites);
		void KillBranches(void);
		void ConstructTree(uint a_nMaxLevel = 3);
		void AssignIDToEntity(void);
		void CheckCollisions();
		uint GetOctantCount(void);
	private:
		void Release(void);
		void Init(void);
		void ConstructList(void);
	};
}
#endif // __THEOCTANTCLASS_H_