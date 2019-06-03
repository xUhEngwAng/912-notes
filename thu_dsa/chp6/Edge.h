#ifndef EDGE_H_
#define EDGE_H_

#include "Graph.h"

template <typename Te>
class Edge{
	Te data;
	double weight = 1;
	Etype type    = UNDETERMINED;

	Edge(Te data) : data(data) {}
	Edge(Te data, double w) : data(data), weight(w) {}
};

#endif
