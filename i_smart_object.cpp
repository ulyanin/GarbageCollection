#include "i_smart_object.h"
#include <iostream>

ISmartObject::ISmartObject()
        : hasCheckedByCollection_(false)
{
#ifdef GC_LOG
    std::cout << "register object: " << this << std::endl;
#endif
    GarbageCollection::getInstance().registerObject_(this);
}

ISmartObject::~ISmartObject()
{
#ifdef GC_LOG
    std::cout << "unregister object: " << this << std::endl;
#endif
    GarbageCollection::getInstance().deleteObjectFromCollection_(this);
}

void * ISmartObject::operator new(size_t size)
{
    void * ptr = GarbageCollection::getInstance().allocate_(size);
//    std::cout << "new " << ptr << " " << addToVoidPointer(ptr, size) << " " << size << " bytes" << std::endl;
    return ptr;
}

//void ISmartObject::operator delete(void * data)
//{
//    //need to throw exception
//}

bool ISmartObject::hasCheckedByCollection() const
{
    return hasCheckedByCollection_;
}

void ISmartObject::markAsChecked()
{
    hasCheckedByCollection_ = true;
}

void ISmartObject::markAsUnChecked()
{
    hasCheckedByCollection_ = false;
}


