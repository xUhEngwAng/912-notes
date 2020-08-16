#ifndef VERTEX_H_
#define VERTEX_H_

#include "Graph.h"

template <typename Tv>
class Vertex{
public:
	Tv data;
	VStatus status = UNDISCOVERED;
	int parent     = -1;
	int dTime      = -1;
	int fTime      = -1;
	int priority   = INT_MAX;
	int inDegree   = 0;
	int outDegree  = 0;

	Vertex() = default;
	Vertex(Tv data) : data(data) {}
};

#endif
