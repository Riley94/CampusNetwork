#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
// Include other headers as necessary

class CampusGraph {
private:
    struct Node {
        std::string name;
        std::string coordinates; // Latitude and longitude
    };

    std::unordered_map<std::string, Node> nodes; // Keyed by location name
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> adj; // Adjacency list

public:
    CampusGraph();
    ~CampusGraph();

    void addLocation(const std::string& name, const std::string& coordinates);
    void addPath(const std::string& from, const std::string& to, int distance);
    void populatePaths(const std::string& apiKey);
    void getDirections(const std::string& origin, const std::string& destination, const std::string& apiKey);
    int getDistance(const std::string& origin, const std::string& destination, const std::string& apiKey);

    void setAdj(const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& adj) {
        this->adj = adj;
    }

    void setNodes(const std::unordered_map<std::string, Node>& nodes) {
        this->nodes = nodes;
    }

    const std::unordered_map<std::string, Node>& getNodes() const {
        return nodes;
    }

    const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& getAdj() const {
        return adj;
    }

    // Other methods as needed, e.g., to find paths or print the graph
};
