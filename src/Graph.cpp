#include "Graph.hpp"
#include <cstring>
#include <iostream>

SparseGraph::SparseGraph() : values(nullptr), col_indices(nullptr), row_ptr(nullptr), num_nodes(0), num_edges(0), capacity_edges(0) {}

SparseGraph::~SparseGraph() {
    freeMemory();
}

void SparseGraph::freeMemory() {
    if (values) { delete[] values; values = nullptr; }
    if (col_indices) { delete[] col_indices; col_indices = nullptr; }
    if (row_ptr) { delete[] row_ptr; row_ptr = nullptr; }
    num_nodes = 0;
    num_edges = 0;
}

void SparseGraph::loadData(const std::string& filename) {
    freeMemory();
    
    std::cout << "[C++ Core] Loading dataset '" << filename << "'..." << std::endl;

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::vector<std::pair<int, int>> edges;
    int u, v;
    int max_id = 0;
    
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        if (ss >> u >> v) {
            edges.push_back({u, v});
            if (u > max_id) max_id = u;
            if (v > max_id) max_id = v;
        }
    }
    infile.close();

    num_nodes = max_id + 1;
    num_edges = edges.size();

    std::cout << "[C++ Core] Load complete. Nodes: " << num_nodes << " | Edges: " << num_edges << std::endl;

    // Sort edges by source, then destination
    std::sort(edges.begin(), edges.end());

    // Allocate CSR arrays
    row_ptr = new int[num_nodes + 1];
    col_indices = new int[num_edges];
    values = new int[num_edges]; 

    // Initialize row_ptr with 0
    std::fill(row_ptr, row_ptr + num_nodes + 1, 0);
    
    // Count degrees
    for (const auto& edge : edges) {
        row_ptr[edge.first + 1]++;
    }

    // Prefix sum
    for (int i = 0; i < num_nodes; i++) {
        row_ptr[i+1] += row_ptr[i];
    }
    
    // Fill col_indices and values
    // We can re-iterate edges because they are sorted and match the row_ptr structure
    for (size_t i = 0; i < edges.size(); ++i) {
        col_indices[i] = edges[i].second;
        values[i] = 1; // Default weight
    }

    std::cout << "[C++ Core] CSR structure built." << std::endl;
}

int SparseGraph::getMaxDegreeNode() {
    int max_degree = -1;
    int max_node = -1;
    for (int i = 0; i < num_nodes; ++i) {
        int degree = row_ptr[i+1] - row_ptr[i];
        if (degree > max_degree) {
            max_degree = degree;
            max_node = i;
        }
    }
    return max_node;
}

std::vector<int> SparseGraph::bfs(int startNode, int maxDepth) {
    std::vector<int> visited_nodes;
    if (startNode < 0 || startNode >= num_nodes) return visited_nodes;

    std::vector<int> dist(num_nodes, -1);
    std::queue<int> q;

    q.push(startNode);
    dist[startNode] = 0;
    visited_nodes.push_back(startNode);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        if (dist[u] >= maxDepth) continue;

        int start = row_ptr[u];
        int end = row_ptr[u+1];

        for (int i = start; i < end; ++i) {
            int v = col_indices[i];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                visited_nodes.push_back(v);
                q.push(v);
            }
        }
    }
    return visited_nodes;
}

std::vector<int> SparseGraph::getNeighbors(int node) {
    std::vector<int> neighbors;
    if (node < 0 || node >= num_nodes) return neighbors;
    
    int start = row_ptr[node];
    int end = row_ptr[node+1];
    for (int i = start; i < end; ++i) {
        neighbors.push_back(col_indices[i]);
    }
    return neighbors;
}
