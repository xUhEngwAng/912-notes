#ifndef DICT_H_
#define DICT_H_

//Dict virtual class
template <typename K, typename V>
class Dict{
public:
	virtual int  size() = 0;
	virtual V    get(K key) = 0;
	virtual bool put(K key, V value) = 0;
	virtual bool remove(K key) = 0;
};

#endif
