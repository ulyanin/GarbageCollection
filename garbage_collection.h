#ifndef GARBAGECOLLECTION_GARBAGE_COLLECTION_H
#define GARBAGECOLLECTION_GARBAGE_COLLECTION_H

#include <cstddef>
#include <cstdlib>
#include <vector>

using std::size_t;

const size_t MEMORY_SIZE_MB = 128;
const size_t MEMORY_SIZE = 1024 * 1024 * MEMORY_SIZE_MB;

class GarbageCollection;

class GarbageCollectionDestroyer
{
private:
    GarbageCollection * instance_;
public:
    ~GarbageCollectionDestroyer();
    void initialize(GarbageCollection * p);
};

class GarbageCollection
{
public:
    static GarbageCollection& getInstance();
    void * allocate(size_t memsize);
    void * deallocate(void * data);
    void collectGarbage();
private:
    static GarbageCollection * instance_;
    static GarbageCollectionDestroyer destroyer_;
protected:
    GarbageCollection();
    ~GarbageCollection();
    friend class GarbageCollecionDestroyer;
    char * memory_;
    size_t last_;
};

#endif //GARBAGECOLLECTION_GARBAGE_COLLECTION_H
