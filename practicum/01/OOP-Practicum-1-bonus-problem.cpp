#include <iostream>

struct Vertex {
	char name[11]; // 1 for '\0'
};

struct Edge {
	Vertex* start;
	Vertex* end;

	// Default constructor initializing pointers to nullptr
	Edge() : start(nullptr), end(nullptr) {}
};

struct Graph {
	size_t verticesCount;
	size_t edgesCount;
	Edge* edges;
};




void deleteGraph(Graph& graph)
{
	delete[] graph.edges;
	graph.edges = nullptr;
	graph.edgesCount = 0;
	graph.verticesCount = 0;
}

void expandGraphEdges(Graph& graph, size_t increment) {
    Edge* expandedEdges = new Edge[graph.edgesCount + increment];
    for (size_t i = 0; i < graph.edgesCount; i++) {
        expandedEdges[i] = graph.edges[i];
    }
    delete[] graph.edges; // Free the old array
    graph.edges = expandedEdges;
    // Do not increment graph.edgesCount here; increment it after adding new edges
}


bool doesEdgeExist(	const Edge& edge, const Graph& graph)
{
	if (!edge.start || !edge.end) {
		return false; // nullptr exceptions
	}

	for (size_t i = 0; i < graph.edgesCount; i++) {
		if (graph.edges[i].start == edge.start &&
			graph.edges[i].end == edge.end) {
			return true;
		}
	}
	return false;
}


void addEdge(const Edge& edge, Graph& graph)
{
	if (!edge.start || !edge.end) {
		return; // nullptr exceptions
	}

	if (doesEdgeExist(edge, graph)) {
		return;
	}

	expandGraphEdges(graph, 1);
	graph.edgesCount++;

	graph.edges[graph.edgesCount - 1] = edge;
}

void connectVertices(Vertex* start, Vertex* end, Graph& graph)
{
	if (!start || !end) {
		return; // nullptr exceptions
	}

	Edge edge{ };
	edge.start = start;
	edge.end = end;

	addEdge(edge, graph);
}

Vertex initVertex(char name[11])
{
	Vertex vertex{};
	for (size_t i = 0; i < 11; i++) {
		vertex.name[i] = name[i];
	}

	return vertex;
}

Vertex inputVertex()
{
	char name[11];
	std::cin.getline(name, 11);

	Vertex vertex = initVertex(name);
	return vertex;
}

Graph initGraph(size_t m)
{
	Graph graph{ };
	graph.verticesCount = 0; // Initially, no vertices are defined.
	graph.edgesCount = m;
	graph.edges = new Edge[graph.edgesCount];

	return graph;
}

Graph inputGraph()
{
	size_t m;
	std::cin >> m;

	Graph graph = initGraph(0);

	for (size_t i = 0; i < m; i++) {
		Vertex start = inputVertex();
		Vertex end = inputVertex();
		connectVertices(&start, &end, graph);
	}

	return graph;
}

// I am going to count both in-degree and out-degree,
// because it's not specified in the problem description
unsigned getVertexDegree(const Vertex& vertex, const Graph& graph)
{
	unsigned counter = 0;
	for (size_t i = 0; i < graph.edgesCount; i++) {
		if (graph.edges[i].start == &vertex ||
			graph.edges[i].end == &vertex) {
			counter++;
		}
	}

	return counter;
}

bool isCompleteGraph(const Graph& graph)
{
	// Calculate the expected number of edges in a complete graph
	// For an undirected graph, the formula is n(n-1)/2
	// Assuming the graph is undirected for this example
	size_t expectedEdges = graph.verticesCount * (graph.verticesCount - 1) / 2;

	return graph.edgesCount == expectedEdges;
}


void deleteEdge(const Edge& edge, Graph& graph)
{
	// Assuming doesEdgeExist function is correctly implemented to check edge existence
	if (!doesEdgeExist(edge, graph)) {
		return; // Edge not found, nothing to do
	}

	bool found = false;
	for (size_t i = 0; i < graph.edgesCount; i++) {
		if (found) {
			// Once the edge is found, shift all subsequent edges to the left
			graph.edges[i - 1] = graph.edges[i];
		}
		else if (graph.edges[i].start == edge.start && graph.edges[i].end == edge.end) {
			// Found the edge to delete
			found = true;
		}
	}

	if (found) {
		// Reduce the edgesCount since we've removed an edge
		graph.edgesCount--;

		// Allocate a new array of edges with one less element
		Edge* smallerEdges = new Edge[graph.edgesCount];

		// Copy the existing edges (except the deleted one) to the new array
		for (size_t i = 0; i < graph.edgesCount; ++i) {
			smallerEdges[i] = graph.edges[i];
		}

		// Delete the old edges array and update the graph's edges pointer
		delete[] graph.edges;
		graph.edges = smallerEdges;
	}
}


int main()
{
	inputGraph();
}