#include <cstdlib>
#include <memory>
#include <iostream>
#include <malloc.h>
#include <algorithm>
#include "garbage_collection.h"


/* singleton pattern */

/* instances */
GarbageCollection * GarbageCollection::instance_ = nullptr;
GarbageCollectionDestroyer GarbageCollection::destroyer_;

GarbageCollectionDestroyer::~GarbageCollectionDestroyer()
{
#ifdef GC_LOG
    std::cout << "GBdestroyer destructor" << std::endl;
#endif
    if (instance_ != nullptr)
        instance_->collectGarbage();
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
{
#ifdef GC_LOG
    std::cout << "GB destructor" << std::endl;
#endif
}

void* GarbageCollection::allocate_(size_t mem_size)
{
    void * mem_ptr = malloc(mem_size);
    if (mem_ptr == nullptr) {
        throw std::bad_alloc();
    }
    registerHeapMemory_(mem_ptr, mem_size);  // stores pointer in maps
    return mem_ptr;
}

bool GarbageCollection::isInRegisteredHeapMemory_(void * objPtr)
{
    auto itUp = registeredHeapMemoryPieces_.upper_bound(objPtr);
    if (itUp == registeredHeapMemoryPieces_.end())
        return false;
    /*
     * begin of found piece is lesser then obj address
     * so return true if exist piece (begin, end) : begin <= objPtr < end
     */
    return addToVoidPointer(itUp->first, -itUp->second) <= objPtr;
}

void* GarbageCollection::pieceOfMemoryObjectBelongsTo_(ISmartObject *objPtr)
{
    auto itUp = registeredHeapMemoryPieces_.upper_bound(objPtr);
    if (itUp == registeredHeapMemoryPieces_.end())
        return nullptr;
    return itUp->first;
}

void GarbageCollection::registerObject_(ISmartObject *objPtr)
{
    if (isInRegisteredHeapMemory_(static_cast<void *>(objPtr))) {
        registeredHeapObjects_.insert(objPtr);
        void * ptrPiece = pieceOfMemoryObjectBelongsTo_(objPtr);
        if (ptrPiece == nullptr) {
            std::cerr << "trying to insert object in unregistered memory piece" << std::endl;
            return;
        }
        registeredObjectsInMemoryPiece_[ptrPiece].push_back(objPtr);
    } else {
        registeredStackObjects_.insert(objPtr);
    }
}

void GarbageCollection::registerHeapMemory_(void *ptr, size_t size)
{
    auto ptr_end = addToVoidPointer(ptr, size);
#ifdef GC_LOG
    std::cout << "\tstore " << ptr << " " << ptr_end << std::endl;
#endif
    /*
     * we stores end of memory to be able to just
     * upper_bound to find some object in piece
     */
    registeredHeapMemoryPieces_[ptr_end] = size;
    isAchievableByDfsMemoryPiece_[ptr_end] = false;
    registeredObjectsInMemoryPiece_[ptr_end] = std::vector<ISmartObject *>();
}

void GarbageCollection::markUsedMemory_()
{
    for (auto obj : registeredHeapObjects_) {
        if (!obj->hasCheckedByCollection()) {
            continue;
        }
        auto itUp = isAchievableByDfsMemoryPiece_.upper_bound(obj);
        if (itUp == isAchievableByDfsMemoryPiece_.end()) {
#ifdef GC_LOG
            std::cerr << "markUsedMem: unregistered object in memory; ptr=" << obj << std::endl;
#endif
            continue;
        }
        itUp->second = true;
    }
}

void GarbageCollection::deleteObjectsInUnusedMemory_()
{
    std::vector<ISmartObject *> toDelete;
    /*
     * we store them in vector because ~ISmartObject changes registeredHeapObjects_
     */
    for (auto &memPiece : registeredObjectsInMemoryPiece_) {
        if (!isAchievableByDfsMemoryPiece_[memPiece.first]) {
            for (auto obj : memPiece.second) {
                toDelete.push_back(obj);
            }
        }
    }
    for (auto obj : toDelete) {
        if (isInRegisteredHeapObjects_(obj)) {
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

void GarbageCollection::deleteMemoryPieceFromIsAchievableByDFSMemoryPiece_(void *ptrEnd)
{
    auto itUp1 = isAchievableByDfsMemoryPiece_.find(ptrEnd);
    if (itUp1 == isAchievableByDfsMemoryPiece_.end()) {
#ifdef GC_LOG
        std::cerr << "delMemPieceFromAchievableByDfsMemPiece: unregistered mem piece; ptr="
                  << ptrEnd << std::endl;
#endif
        return;
    }
    isAchievableByDfsMemoryPiece_.erase(itUp1);
}

void GarbageCollection::deleteMemoryPieceFromRegisteredHeapMemoryPieces_(void *ptrEnd)
{
    auto itUp2 = registeredHeapMemoryPieces_.find(ptrEnd);
    if (itUp2 == registeredHeapMemoryPieces_.end()) {
#ifdef GC_LOG
        std::cerr << "delMemPieceFromRegHeapMemPiece: unregistered mem piece; ptr=" << ptrEnd << std::endl;
#endif
        return;
    }
    registeredHeapMemoryPieces_.erase(itUp2);
}

void GarbageCollection::deleteMemoryPieceFromRegisteredObjectsInMemoryPiece_(void *ptrEnd)
{
    auto itUp3 = registeredObjectsInMemoryPiece_.find(ptrEnd);
    if (itUp3 == registeredObjectsInMemoryPiece_.end()) {
#ifdef GC_LOG
        std::cerr << "delMemPieceFromRegisteredObjInMemPiece: unregistered mem piece; ptr="
                  << ptrEnd << std::endl;
#endif
        return;
    }
    registeredObjectsInMemoryPiece_.erase(itUp3);
}

void GarbageCollection::deleteMemoryPieceFromCollection_(void * ptrEnd)
{
    auto itUp2 = registeredHeapMemoryPieces_.find(ptrEnd);
    void *ptrBegin = addToVoidPointer(itUp2->first, -itUp2->second);
#ifdef GC_LOG
    std::cout << "\tfree " << ptrBegin << " " << ptrEnd << std::endl;
#endif
    free(ptrBegin);
    deleteMemoryPieceFromIsAchievableByDFSMemoryPiece_(ptrEnd);
    deleteMemoryPieceFromRegisteredObjectsInMemoryPiece_(ptrEnd);
    deleteMemoryPieceFromRegisteredHeapMemoryPieces_(ptrEnd);
}

bool GarbageCollection::isInRegisteredHeapObjects_(ISmartObject *objPtr)
{
    return registeredHeapObjects_.find(objPtr) != registeredHeapObjects_.end();
}

bool GarbageCollection::isInRegisteredStackObjects_(ISmartObject *objPtr)
{
    return registeredStackObjects_.find(objPtr) != registeredStackObjects_.end();
}

/* called by ~ISmartObject() */
void GarbageCollection::deleteObjectFromCollection_(ISmartObject *object)
{
    if (isInRegisteredHeapObjects_(object))
        registeredHeapObjects_.erase(object);
    else if (isInRegisteredStackObjects_(object)) {
        registeredStackObjects_.erase(object);
    } else {
#ifdef GC_LOG
        std::cerr << "object " << object
                  << " is neither in registered stack and heap objects"
                  << std::endl;
#endif
    }
}

void GarbageCollection::DFS_(ISmartObject *object)
{

    if (object->hasCheckedByCollection())
        return;
    object->markAsChecked();
    // visit all direct children
    for (auto subject: object->pointers()) {
        DFS_(subject);
    }
    if (isInRegisteredStackObjects_(object))
        return;
    // we should visit all objects in our memory piece
    void * ownMemory = pieceOfMemoryObjectBelongsTo_(object);
    for (auto subject: registeredObjectsInMemoryPiece_[ownMemory]) {
        DFS_(subject);
    }
}

void GarbageCollection::resetFlagsAndMarks_()
{
    for (auto obj : registeredHeapObjects_) {
        obj->markAsUnChecked();
    }
    for (auto obj : registeredStackObjects_) {
        obj->markAsUnChecked();
    }
    for (auto &pairIt : isAchievableByDfsMemoryPiece_) {
        pairIt.second = false;
    }
}

void GarbageCollection::collectGarbage()
{
    resetFlagsAndMarks_();
    for (auto obj : registeredStackObjects_) {
        DFS_(obj);
    }
    markUsedMemory_();
    deleteObjectsInUnusedMemory_();
    deleteUnusedMemory_();
}

bool GarbageCollection::isPointerSteelAlive(ISmartObject *objPtr)
{
    return registeredHeapObjects_.find(objPtr) != registeredHeapObjects_.end();
}
