#include "smart_object.h"

SmartObject::SmartObject()
    : hasCheckedByCollection_(false)
    , isHeapObject_(false)

{ }

SmartObject::~SmartObject()
{ }

void * SmartObject::operator new(size_t size)
{
}

bool SmartObject::hasCheckedByCollection() const
{
    return hasCheckedByCollection_;
}

bool SmartObject::isStackObject() const
{
    return !isHeapObject_;
}

void SmartObject::markAsChecked()
{
    hasCheckedByCollection_ = true;
}


