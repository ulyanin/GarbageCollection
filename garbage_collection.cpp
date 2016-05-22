#include <cstdlib>
#include <memory>
#include <iostream>
#include <malloc.h>
#include "garbage_collection.h"

inline void * addToVoidPointer(void * ptr, std::ptrdiff_t diff)
{
    return static_cast<void *>(static_cast<char *>(ptr) + diff);
}

/* singleton pattern */

/* instances */
GarbageCollection * GarbageCollection::instance_ = nullptr;
GarbageCollectionDestroyer GarbageCollection::destroyer_;

GarbageCollectionDestroyer::~GarbageCollectionDestroyer()
{
    delete instance_;
}

void GarbageCollectionDestroyer::initialize(GarbageCollection *p)
{
    instance_ = p;
}

/* getting singleton instance */

GarbageCollection& GarbageCollection::getInstance()
{
    if (!instance_) {
        instance_ = new GarbageCollection();
        destroyer_.initialize(instance_);
    }
    return *instance_;
}

/* end of singleton pattern functions */

GarbageCollection::GarbageCollection()
{ }

GarbageCollection::~GarbageCollection()
{ }

void* GarbageCollection::allocate(size_t mem_size)
{
    void * mem_ptr = malloc(mem_size);
    if (mem_ptr == nullptr) {
        throw std::bad_alloc();
    }
    registerHeapMemory(mem_ptr, mem_size);
    return mem_ptr;
}

void* GarbageCollection::deallocate(void *data)
{

}

bool GarbageCollection::isInRegisteredHeapMemory_(void * objPtr)
{
    auto itUp = registeredHeapMemoryPieces_.find(objPtr);
    if (itUp == registeredHeapMemoryPieces_.end())
        return false;
    return objPtr <= addToVoidPointer(itUp->first, -itUp->second);
}

void GarbageCollection::registerObject(ISmartObject *ptr)
{
    if (isInRegisteredHeapMemory_(static_cast<void *>(ptr))) {
        registeredHeapObjects_.insert(ptr);
    } else {
        registeredStackObjects_.insert(ptr);
    }
}

void GarbageCollection::registerHeapMemory(void *ptr, size_t size)
{
    auto ptr_end = addToVoidPointer(ptr, size);
    registeredHeapMemoryPieces_[ptr_end] = size;
    isAchievableByDfsMemoryPiece_[ptr_end] = false;
}

void GarbageCollection::dfs_(ISmartObject *object)
{

    if (object->hasCheckedByCollection())
        return;
    object->markAsChecked();
    for (auto subject: object->pointers()) {
        dfs_(subject);
    }
}

void GarbageCollection::markUsedMemory_()
{
    for (auto obj : registeredHeapObjects_) {
        if (!obj->hasCheckedByCollection()) {
            continue;
        }
        auto itUp = isAchievableByDfsMemoryPiece_.upper_bound(obj);
        if (itUp == isAchievableByDfsMemoryPiece_.end()) {
            std::cerr << "unregistered object in memory; ptr=" << obj << std::endl;
            continue;
        }
        itUp->second = true;
    }
}

void GarbageCollection::deleteObjectsInUnusedMemory_()
{
    for (auto obj : registeredHeapObjects_) {
        auto itUp = isAchievableByDfsMemoryPiece_.upper_bound(obj);
        if (itUp == isAchievableByDfsMemoryPiece_.end()) {
            std::cerr << "unregistered object in memory; ptr=" << obj << std::endl;
            continue;
        }
        if (!itUp->second) { /*so we are in unused memory*/
            obj->~ISmartObject();
        }
    }
}

void GarbageCollection::deleteUnusedMemory_()
{
    std::vector <void *> unused;
    for (const auto &it : isAchievableByDfsMemoryPiece_) {
        if (it.second) {
            continue;
        }
        unused.push_back(it.first);
    }
    for (auto ptr : unused) {
        deleteMemoryPieceFromCollection_(ptr);
    }
}

void GarbageCollection::deleteMemoryPieceFromCollection_(void * ptrInside)
{
    auto itUp1 = isAchievableByDfsMemoryPiece_.upper_bound(ptrInside);
    if (itUp1 == isAchievableByDfsMemoryPiece_.end()) {
        std::cerr << "unregistered object in memory; ptr=" << ptrInside << std::endl;
        return;
    }
    isAchievableByDfsMemoryPiece_.erase(itUp1);
    auto itUp2 = registeredHeapMemoryPieces_.upper_bound(ptrInside);
    if (itUp2 == registeredHeapMemoryPieces_.end()) {
        std::cerr << "unregistered object in memory; ptr=" << ptrInside << std::endl;
        return;
    }
    registeredHeapMemoryPieces_.erase(itUp2);
}

bool GarbageCollection::isInRegisteredHeapObjects_(ISmartObject *objPtr)
{
    return registeredHeapObjects_.find(objPtr) != registeredHeapObjects_.end();
}

bool GarbageCollection::isInRegisteredStackObjects_(ISmartObject *objPtr)
{
    return registeredStackObjects_.find(objPtr) != registeredStackObjects_.end();
}

/* calling by ~ISmartObject() */
void GarbageCollection::deleteObjectFromCollection_(ISmartObject *object)
{
    if (isInRegisteredHeapObjects_(object))
        registeredHeapObjects_.erase(object);
    else if (isInRegisteredStackObjects_(object)) {
        registeredStackObjects_.erase(object);
    } else {
        std::cerr << "object " << object
                  << " is neither in registered stack and heap objects"
                  << std::endl;
    }
}

void GarbageCollection::collectGarbage()
{
    for (auto obj : registeredHeapObjects_) {
        obj->markAsUnChecked();
    }
    for (auto obj : registeredStackObjects_) {
        obj->markAsUnChecked();
    }
    for (auto obj : registeredStackObjects_) {
        dfs_(obj);
    }
    markUsedMemory_();
    deleteObjectsInUnusedMemory_();
    deleteUnusedMemory_();
}
