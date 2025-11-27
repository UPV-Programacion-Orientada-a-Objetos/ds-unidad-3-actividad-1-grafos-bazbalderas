from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "Graph.hpp":
    cdef cppclass SparseGraph:
        SparseGraph() except +
        void loadData(string filename)
        int getMaxDegreeNode()
        vector[int] bfs(int startNode, int maxDepth)
        vector[int] getNeighbors(int node)
        int getNodeCount()
        int getEdgeCount()
