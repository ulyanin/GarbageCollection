#ifndef I_SMART_OBJECT_H
#define I_SMART_OBJECT_H

#include <vector>
#include "garbage_collection.h"

using std::size_t;

class GarbageCollector;

class ISmartObject
{
public:
    ISmartObject();
    virtual std::vector<ISmartObject *> pointers() const = 0; // Should be implemented by user
    static void* operator new(size_t size);
    static void operator delete(void * data);
    virtual ~ISmartObject();
    bool hasCheckedByCollection() const;
    void markAsChecked();  // marking as checked; need for dfs in Garbage Collection
    void markAsUnChecked();  // inverse to markAsChecked method

protected:
    bool hasCheckedByCollection_;
};


#endif
