#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

template <typename K, typename V>
class entry{
public:
	K key;
	V value;

	//constructor
	entry() = default;
	entry(K k, V v) : key(k), value(v) {}

	//overload operator
	bool operator==(entry<K, V> const e) { return key == e.key; }
	bool operator!=(entry<K, V> const e) { return key != e.key; }
	bool operator>(entry<K, V> const e) { return key > e.key; }
	bool operator<(entry<K, V> const e) { return key < e.key; }
	bool operator>=(entry<K, V> const e) { return (key > e.key) || (key == e.key); }
};

template <typename K, typename V>
class PriorityQueue{
	virtual int         size()   = 0;
	virtual bool        empty()  = 0;
	virtual entry<K, V> getMax() = 0;
	virtual entry<K, V> delMax() = 0;
	virtual void        insert(K key, V value) = 0;					//duplicate keys are allowed
	virtual void        insert(entry<K, V> e)  = 0;
};

#endif
