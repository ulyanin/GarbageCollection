#ifndef SMART_OBJECT_H
#define SMART_OBJECT_H

#include <vector>
#include "garbage_collection.h"

using std::size_t;

class SmartObject
{
public:
    bool hasCheckedByCollection_, isHeapObject_;
    SmartObject();
    virtual std::vector<SmartObject *> pointers() const = 0; // Should be implemented by user
    static void* operator new(size_t size);
    virtual ~SmartObject();
    bool isStackObject() const;
    bool hasCheckedByCollection() const;
    void markAsChecked();
protected:
};


#endif
