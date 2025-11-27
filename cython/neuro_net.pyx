# distutils: language = c++

from libcpp.vector cimport vector
from libcpp.string cimport string
from neuro_net cimport SparseGraph

cdef class NeuroNetEngine:
    cdef SparseGraph* c_graph

    def __cinit__(self):
        self.c_graph = new SparseGraph()

    def __dealloc__(self):
        del self.c_graph

    def load_data(self, filename: str):
        cdef string c_filename = filename.encode('utf-8')
        self.c_graph.loadData(c_filename)

    def get_max_degree_node(self):
        return self.c_graph.getMaxDegreeNode()

    def bfs(self, start_node: int, max_depth: int):
        return self.c_graph.bfs(start_node, max_depth)

    def get_neighbors(self, node: int):
        return self.c_graph.getNeighbors(node)
    
    def get_node_count(self):
        return self.c_graph.getNodeCount()

    def get_edge_count(self):
        return self.c_graph.getEdgeCount()
