#include "MyOctant.h"
using namespace Simplex;
uint Simplex::MyOctant::m_uMaxLevel = 0;
uint Simplex::MyOctant::m_uOctantCount = 0;

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) {
    //Set the max subdivision level
    m_uMaxLevel = a_nMaxLevel;
    // m_uIdealEntityCount = a_nIdealEntityCount;

    //Set the singletons
    m_pEntityManager = MyEntityManager::GetInstance();
    m_pMeshManager = MeshManager::GetInstance();

    //Set id and level (base octant)
    m_uLevel = m_uID = 0;

    //Set size to radius of the circle used in appclass.cpp (base octant)
    m_v3Size = vector3(75.0f);

    //Set min and max points
    m_v3Max = m_v3Center + m_v3Size / 2.0f; // divide by 2.0f to use half width
    m_v3Min = m_v3Center - m_v3Size / 2.0f; // divide by 2.0f to use half width

                                            //Initialize all children to null pointers
    for (uint i = 0; i < 8; i++)
    {
        m_pChild[i] = nullptr;
    }

    //Increase total octant count
    m_uOctantCount++;

    //Start building the octree
    ConstructTree(m_uLevel);
}

Simplex::MyOctant::MyOctant(uint a_uMaxLevel)
{
    //Set the max subdivision level
    m_uMaxLevel = a_uMaxLevel;

    //Set the singletons
    m_pEntityManager = MyEntityManager::GetInstance();
    m_pMeshManager = MeshManager::GetInstance();

    //Set id and level (base octant)
    m_uLevel = m_uID = 0;

    //Set size to radius of the circle used in appclass.cpp (base octant)
    m_v3Size = vector3(75.0f);

    //Set min and max points
    m_v3Max = m_v3Center + m_v3Size / 2.0f; // divide by 2.0f to use half width
    m_v3Min = m_v3Center - m_v3Size / 2.0f; // divide by 2.0f to use half width

                                                    //Initialize all children to null pointers
    for (uint i = 0; i < 8; i++)
    {
        m_pChild[i] = nullptr;
    }

    //Increase total octant count
    m_uOctantCount++;

    //Start building the octree
    ConstructTree(m_uLevel);
}

Simplex::MyOctant::MyOctant(MyOctant* a_pParent, vector3 a_v3Center, vector3 a_fSize, uint a_uLevel)
{
    //Set variables
    m_pParent = a_pParent;
    m_v3Center = a_v3Center;
    m_v3Size = a_fSize;
    m_uLevel = a_uLevel;
    m_uID = m_uOctantCount;

    //Set the singletons
    m_pEntityManager = MyEntityManager::GetInstance();
    m_pMeshManager = MeshManager::GetInstance();

    //Set min and max points
    m_v3Max = m_v3Center + m_v3Size / 2.0f; // divide by 2.0f to use half width
    m_v3Min = m_v3Center - m_v3Size / 2.0f; //divide by 2.0f to use half width

                                                    //Initialize all children to null pointers
    for (uint i = 0; i < 8; i++)
    {
        m_pChild[i] = nullptr;
    }

    //Increase total octant count
    m_uOctantCount++;

    //Continue building the octree
    ConstructTree(a_uLevel);
}

Simplex::MyOctant::~MyOctant()
{
    KillBranches();

    //Reset static variables when root is deleted
    if (m_uID == 0)
    {
        m_uOctantCount = 0;
        m_uMaxLevel = 0;
    }
}

vector3 Simplex::MyOctant::GetSize()
{
    return m_v3Size;
}

vector3 Simplex::MyOctant::GetCenterGlobal()
{
    return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal()
{
    return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal()
{
    return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uIndex)
{
    //Check if the index is valid
    if (a_uIndex < 0 || a_uIndex >= m_pEntityManager->GetEntityCount())
    {
        return false;
    }

    //Get the entity
    MyEntity* entity = m_pEntityManager->GetEntity(a_uIndex);

    //Get globals for max and min points on the entity
    vector3 maxG = entity->GetRigidBody()->GetMaxGlobal();
    vector3 minG = entity->GetRigidBody()->GetMinGlobal();

    //Collisions
    if ((m_v3Min.x <= maxG.x && m_v3Max.x >= minG.x) && (m_v3Min.y <= maxG.y && m_v3Max.y >= minG.y) && (m_v3Min.z <= maxG.z && m_v3Max.z >= minG.z))
    {
        return true;
    }

    //No collisions
    return false;
}

void Simplex::MyOctant::Display()
{
    //Display this octant
    matrix4 m4Scale = glm::scale(m_v3Size);
    matrix4 m4Translation = glm::translate(m_v3Center);
    matrix4 m4Model = m4Translation * m4Scale;
    m_pMeshManager->AddWireCubeToRenderList(m4Model, C_YELLOW, RENDER_WIRE);

    //Display all child octants
    for (uint i = 0; i < m_uChildren; i++)
    {
        m_pChild[i]->Display();
    }
}

void Simplex::MyOctant::Display(uint a_uIndex, vector3 a_v3Color)
{
    //Check to see if index is valid
    if (a_uIndex < 0 || a_uIndex >= m_uOctantCount)
    {
        //Display normally
        Display();
    }

    //Display the target octant
    else
    {
        //Target octant found
        if (m_uID == a_uIndex)
        {
            //Display this octant
            matrix4 m4Scale = glm::scale(m_v3Size);
            matrix4 m4Translation = glm::translate(m_v3Center);
            matrix4 m4Model = m4Translation * m4Scale;
            m_pMeshManager->AddWireCubeToRenderList(m4Model, C_YELLOW, RENDER_WIRE);
        }

        else
        {
            //Search children
            for (uint i = 0; i < m_uChildren; i++)
            {
                m_pChild[i]->Display(a_uIndex);
            }
        }
    }
}

void Simplex::MyOctant::ClearEntityList()
{
    m_lEntityList.clear();
}

MyOctant * Simplex::MyOctant::GetChild(uint a_uIndex)
{
    //Check if index is valid
    if (a_uIndex < 0 || a_uIndex >= 8)
    {
        return nullptr;
    }

    return m_pChild[a_uIndex];
}

MyOctant * Simplex::MyOctant::GetParent()
{
    return m_pParent;
}

bool Simplex::MyOctant::IsLeaf()
{
    //Check if it has children
    if (m_uChildren > 0)
    {
        return false;
    }

    return true;
}

uint Simplex::MyOctant::GetOctantCount()
{
    return m_uOctantCount;
}

void Simplex::MyOctant::ConstructTree(uint a_uLevel)
{
    //If recursively got to max level, stop going further
    if (a_uLevel >= m_uMaxLevel)
    {
        //Set which objects are in this octant/dimension
        AssignIDtoEntity();

        return;
    }

    vector3 offset = ZERO_V3; //offset for child position
    float size = m_v3Size.length() / 2.0f * 0.5f; //half width of child

                                        //Create the 8 children recursively
    for (uint i = 0; i < 8; i++)
    {
        if (i & 1)
        {
            offset.x = size;
        }
        else
        {
            offset.x = -size;
        }

        if (i & 2)
        {
            offset.y = size;
        }
        else
        {
            offset.y = -size;
        }

        if (i & 4)
        {
            offset.z = size;
        }
        else
        {
            offset.z = -size;
        }

        //Create child (Parameters: parent, center, size, level)
        m_pChild[i] = new MyOctant(this, m_v3Center + offset, vector3(size * 2.0f), a_uLevel + 1);
    }

    //Set number of children
    m_uChildren = 8;
}

void Simplex::MyOctant::AssignIDtoEntity()
{
    //The commented out code is used to assign IDs if
    //you don't assign IDs when constructing the tree
    /*
    //Check octant if it is a leaf
    if (IsLeaf())
    {
    for (uint i = 0; i < m_pEntityManager->GetEntityCount(); i++)
    {
    //If entity collides with octant, add entity to octant (dimension)
    if (IsColliding(i))
    {
    m_pEntityManager->AddDimension(i, m_uID);
    m_EntityList.push_back(i);
    }
    }
    }
    //Recursively traverse to leaf nodes
    else
    {
    for (uint i = 0; i < m_uChildren; i++)
    {
    m_pChild[i]->AssignIDtoEntity();
    }
    }
    */

    //Check if octant is a leaf
    if (IsLeaf())
    {
        for (uint i = 0; i < m_pEntityManager->GetEntityCount(); i++)
        {
            //If entity collides with octant, add entity to octant (dimension)
            if (IsColliding(i))
            {
                m_pEntityManager->AddDimension(i, m_uID);
                m_lEntityList.push_back(i);
            }
        }
    }
}

void Simplex::MyOctant::KillBranches()
{
    //Recursively runs deletion code on parent and all children
    //All lower level nodes (higher level number = lower level)
    //are deleted first then the higher level nodes
    //(lower level number = higher level) are deleted, 
    //with the root node being deleted last.
    //So deletion runs from bottom to top.
    if (!IsLeaf())
    {
        for (uint i = 0; i < m_uChildren; i++)
        {
            delete m_pChild[i];
        }
    }
}