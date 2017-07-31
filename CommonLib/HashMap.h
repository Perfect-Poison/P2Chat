#pragma once
#include "p2_common.h"
#include "p2_util.h"
#include "uthash.h"
#include "Iterator.h"
P2_NAMESPACE_BEG

template <class T1, class T2>
struct Pair
{
    typedef T1 first_type;
    typedef T2 second_type;
    T1 first;
    T2 second;
    Pair() : first(), second() {}
    Pair(const T1& val1, const T2& val2) :first(val1), second(val2) {}
    Pair(const Pair&) = default;
    Pair(Pair&&) = default;
    Pair<T1, T2>& operator=(Pair<T1, T2>&& _Right)
    {
        first = std::forward<T1>(_Right.first);
        second = std::forward<T2>(_Right.second);
        return (*this);
    }
};

struct HashMapEntry
{
    UT_hash_handle hh;
    union
    {
        BYTE d[16];
        void *p;
    } key;
    Pair<const void*,void*> pair;
};

class HashMapBase
{
public:
    virtual ~HashMapBase() { Clear(); }
    void Clear();
    int32 Size() const;
protected:
    HashMapBase(uint32 keyLength):fHashData(nullptr), fKeyLength(keyLength) {}
    void* Get(const void* key) const;
    void Set(const void* key, void* value);
    void Remove(const void* key);
    bool Contains(const void* key) const;
private:
    HashMapEntry *Find(const void *key) const;
    HashMapEntry *fHashData;
    uint32 fKeyLength;
    friend class HashMapIterator;
};

template <class K, class V>
class HashMap : public HashMapBase
{
public:
    HashMap() : HashMapBase(sizeof(K)) {};
    virtual ~HashMap() {};
    V* Get(const K& key) { return (V *)HashMapBase::Get(&key); }
    void Set(const K& key, V* value) { HashMapBase::Set(&key, (void *)value); }
    void Remove(const K& key) { HashMapBase::Remove(&key); }
    bool Contains(const K& key) { return HashMapBase::Contains(&key); }
    Iterator<Pair<K, V>>* iterator() { return new Iterator<Pair<K, V>>(new HashMapIterator(this)); }
};

P2_NAMESPACE_END