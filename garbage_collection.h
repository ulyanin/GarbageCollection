#ifndef GARBAGECOLLECTION_GARBAGE_COLLECTION_H
#define GARBAGECOLLECTION_GARBAGE_COLLECTION_H

#include <cstddef>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <exception>
#include "i_smart_object.h"

using std::size_t;

class ISmartObject;

class GarbageCollection;

class GarbageCollectionDestroyer
{
private:
    GarbageCollection* instance_;
public:
    ~GarbageCollectionDestroyer();
    void initialize(GarbageCollection* p);
};

class GarbageCollection
{
private:
    static GarbageCollection* instance_;
    static GarbageCollectionDestroyer destroyer_;
protected:
    GarbageCollection();
    GarbageCollection( const GarbageCollection& ) = delete;
    GarbageCollection& operator=( GarbageCollection& ) = delete;
    ~GarbageCollection();
    friend class GarbageCollectionDestroyer;

    bool isInRegisteredHeapMemory_(void * ptr);
    bool isInRegisteredHeapObjects_(ISmartObject * objPtr);
    bool isInRegisteredStackObjects_(ISmartObject * objPtr);
    void * pieceOfMemoryObjectBelongsTo_(ISmartObject *objPtr);
    void markUsedMemory_();
    void deleteObjectsInUnusedMemory_();
    void deleteUnusedMemory_();  /* must be called after deleteObjectsInUnusedMemory_ !!! */
    void deleteObjectFromCollection_(ISmartObject * object);
    /* these functions delete mempry pieces from map */
    void deleteMemoryPieceFromRegisteredHeapMemoryPieces_(void * ptrEnd);
    void deleteMemoryPieceFromRegisteredObjectsInMemoryPiece_(void * ptrEnd);
    void deleteMemoryPieceFromIsAchievableByDFSMemoryPiece_(void *ptrEnd);
    void deleteMemoryPieceFromCollection_(void * ptrEnd);
    void DFS_(ISmartObject *object);
    void * allocate_(size_t memsize);
    void registerObject_(ISmartObject *objPtr);
    void registerHeapMemory_(void *ptr, size_t size);
    void resetFlagsAndMarks_();
    friend class GarbageCollecionDestroyer;

    friend class ISmartObject;

    std::set<ISmartObject *> registeredHeapObjects_,
                             registeredStackObjects_;
    std::map<void *, ptrdiff_t> registeredHeapMemoryPieces_;
    std::map<void *, std::vector<ISmartObject *> > registeredObjectsInMemoryPiece_;
    std::map<void *, bool> isAchievableByDfsMemoryPiece_;
public:
    static GarbageCollection& getInstance();
    void collectGarbage();
    bool isPointerSteelAlive(ISmartObject * objPtr);  // return true if this objPtr stores in garbage collector

};


#endif