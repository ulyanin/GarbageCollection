#include "garbage_collection.h"

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
    : last_(0)
{
    memory_ = new char[MEMORY_SIZE_MB];
}

GarbageCollection::~GarbageCollection()
{
    delete [] memory_;
}

void* GarbageCollection::allocate(size_t memsize)
{
    void * mem = static_cast<void *>(memory_ + last_);
    last_ += memsize;
    return mem;
}

void* GarbageCollection::deallocate(void *data)
{ }
