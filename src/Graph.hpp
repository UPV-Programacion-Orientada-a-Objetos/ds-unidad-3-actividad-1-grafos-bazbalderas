#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <limits>
#include <chrono>

// Abstract Base Class
class IGraph {
public:
    virtual ~IGraph() {}
    virtual void loadData(const std::string& filename) = 0;
    virtual int getMaxDegreeNode() = 0;
    virtual std::vector<int> bfs(int startNode, int maxDepth) = 0;
    virtual int getNodeCount() const = 0;
    virtual int getEdgeCount() const = 0;
};

// Concrete Class using CSR (Compressed Sparse Row)
class SparseGraph : public IGraph {
private:
    // CSR format arrays
    int* values;        // Edge weights (or just 1s)
    int* col_indices;   // Column indices
    int* row_ptr;       // Row pointers

    int num_nodes;
    int num_edges;
    int capacity_edges; // For dynamic resizing during loading if needed (though 2-pass is better)

    void freeMemory();

public:
    SparseGraph();
    ~SparseGraph();

    void loadData(const std::string& filename) override;
    int getMaxDegreeNode() override;
    std::vector<int> bfs(int startNode, int maxDepth) override;
    
    int getNodeCount() const override { return num_nodes; }
    int getEdgeCount() const override { return num_edges; }

    // Helper to get neighbors for visualization if needed
    std::vector<int> getNeighbors(int node);
};

#endif // GRAPH_HPP
