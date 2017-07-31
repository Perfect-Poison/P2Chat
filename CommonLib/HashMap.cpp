#include "HashMap.h"
P2_NAMESPACE_BEG

void HashMapBase::Clear()
{
    HashMapEntry *entry, *tmp;
    HASH_ITER(hh, fHashData, entry, tmp)
    {
        HASH_DEL(fHashData, entry);
        if (this->fKeyLength > 16)
            safe_free(entry->key.p);
        safe_free(entry);
    }
}

int32 HashMapBase::Size() const
{
    return HASH_COUNT(fHashData);
}

void* HashMapBase::Get(const void* key) const
{
    HashMapEntry* entry;
    HASH_FIND(hh, fHashData, key, fKeyLength, entry);
    return entry ? entry->pair.second : nullptr;
}

void HashMapBase::Set(const void* key, void* value)
{
    if (key == nullptr)
        return;

    HashMapEntry* entry = Find(key);
    if (entry != nullptr)
        entry->pair.second = value;
    else
    {
        entry = (HashMapEntry *)malloc(sizeof(HashMapEntry));
        if (fKeyLength <= 16)
        {
            memcpy(entry->key.d, key, fKeyLength);
            entry->pair = Pair<decltype(key), decltype(value)>(entry->key.d, value);;
        }
        else
        {
            entry->key.p = memdup(key, fKeyLength);
            entry->pair = Pair<decltype(key), decltype(value)>(entry->key.p, value);;
        }
        HASH_ADD_KEYPTR(hh, fHashData, (fKeyLength > 16 ? entry->key.p : entry->key.d), fKeyLength, entry);
    }
}

void HashMapBase::Remove(const void* key)
{
    HashMapEntry* entry;
    HASH_FIND(hh, fHashData, key, fKeyLength, entry);
    if (entry != nullptr)
    {
        HASH_DEL(fHashData, entry);
        if (fKeyLength > 16)
            safe_free(entry->key.p);
        safe_free(entry);
    }
}

bool HashMapBase::Contains(const void* key) const
{
    return Find(key) != nullptr;
}

HashMapEntry* HashMapBase::Find(const void *key) const
{
    if (key == nullptr)
        return nullptr;

    HashMapEntry *entry;
    HASH_FIND(hh, fHashData, key, fKeyLength, entry);
    return entry;
}

P2_NAMESPACE_END

