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
    void dfs_(ISmartObject * object);
protected:
    GarbageCollection();
    GarbageCollection( const GarbageCollection& ) = delete;
    GarbageCollection& operator=( GarbageCollection& ) = delete;
    ~GarbageCollection();
    friend class GarbageCollectionDestroyer;

    bool isInRegisteredHeapMemory_(void * ptr);
    bool isInRegisteredHeapObjects_(ISmartObject * objPtr);
    bool isInRegisteredStackObjects_(ISmartObject * objPtr);
    void markUsedMemory_();
    void deleteObjectsInUnusedMemory_();
    void deleteUnusedMemory_();  /* must be called after deleteObjectsInUnusedMemory_ !!! */
    void deleteObjectFromCollection_(ISmartObject * object);
    void deleteMemoryPieceFromCollection_(void * ptrEnd);
    friend class GarbageCollecionDestroyer;
    friend class ISmartObject;

    std::set<ISmartObject *> registeredHeapObjects_,
                             registeredStackObjects_;

    std::map<void *, ptrdiff_t> registeredHeapMemoryPieces_;
    std::map<void *, bool> isAchievableByDfsMemoryPiece_;
public:
    static GarbageCollection& getInstance();
    enum objectType
    {
        stackObject,
        heapObject,
        //array
    };
    void * allocate(size_t memsize);
//    void * deallocate(void * data);
    void registerObject(ISmartObject * obj_ptr);
    void registerHeapMemory(void * ptr, size_t size);
    void collectGarbage();
};

//inline void * addToVoidPointer(void * ptr, std::ptrdiff_t diff);

//class GarbageCollection;
//
//class GarbageCollectionDestroyer
//{
//private:
//    GarbageCollection * instance_;
//public:
//    ~GarbageCollectionDestroyer();
//    void initialize(GarbageCollection * p);
/*

class GarbageCollection
{
public:
    enum objectType
    {
        stackObject,
        heapObject,
        //array
    };
    static GarbageCollection& getInstance();
    void * allocate(size_t memsize);
    void * deallocate(void * data);
    void registerObject(ISmartObject * obj_ptr);
    //void registerHeapMemory(void * ptr);
    void collectGarbage();
private:
    static GarbageCollection * instance_;
    static GarbageCollectionDestroyer destroyer_;
    void dfs_(ISmartObject * object);
protected:
    GarbageCollection();
    ~GarbageCollection();
    GarbageCollection(const GarbageCollection &) = delete;
    GarbageCollection&operator=(const GarbageCollection&) = delete;
    bool isInRegisteredHeapMemory_(void * ptr);
    bool isInRegisteredHeapObjects_(ISmartObject * objPtr);
    bool isInRegisteredStackObjects_(ISmartObject * objPtr);
    friend class GarbageCollecionDestroyer;
    friend class ISmartObject;

    std::set<ISmartObject *> registeredHeapObjects_,
                             registeredStackObjects_;
    std::map<void *, ptrdiff_t> registeredHeapMemoryPieces_;
};
*/

//};

#endif