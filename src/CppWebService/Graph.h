#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <string>
// Include other headers as necessary

using namespace std;

class CampusGraph {
private:
    struct Node {
        string name;
        string coordinates; // Latitude and longitude
    };

    unordered_map<string, Node> nodes; // Keyed by location name
    unordered_map<string, vector<pair<string, int>>> adj; // Adjacency list

public:
    void addLocation(const string& name, const string& coordinates);
    void addPath(const string& from, const string& to, int distance);
    std::pair<std::vector<std::pair<std::string, std::string>>, std::vector<std::tuple<std::string, std::string, double>>> getDirections(const Node& origin, const Node& destination, const string& apiKey);
    void loadGraph(const string& filePath);
    void saveGraph(const string& filePath) const;

    void setAdj(const unordered_map<string, vector<pair<string, int>>>& adj) {
        this->adj = adj;
    }

    void setNodes(const unordered_map<string, Node>& nodes) {
        this->nodes = nodes;
    }

    const unordered_map<string, Node>& getNodes() const {
        return nodes;
    }

    const unordered_map<string, vector<pair<string, int>>>& getAdj() const {
        return adj;
    }

    // Other methods as needed, e.g., to find paths or print the graph
};
