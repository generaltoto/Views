
#include "IResourceObject.h"

IResourceObject::IResourceObject(std::string name) : m_Name(std::move(name)), m_HeapIndex(0), m_ResType(0)
{
    m_Resource = nullptr;
    m_UploadHeap = nullptr;
}

IResourceObject::~IResourceObject()
{
    RELPTR(m_Resource);
    RELPTR(m_UploadHeap);
}
