#include "i_smart_object.h"


ISmartObject::ISmartObject()
        : hasCheckedByCollection_(false)
{
    GarbageCollection::getInstance().registerObject(this);
}

ISmartObject::~ISmartObject()
{
    GarbageCollection::getInstance().deleteObjectFromCollection_(this);
}

void * ISmartObject::operator new(size_t size)
{
    return GarbageCollection::getInstance().allocate(size);
}

void ISmartObject::operator delete(void * data)
{
    GarbageCollection::getInstance().deallocate(data);
}

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


