#include <iostream>
#include <unordered_map>
#include <vector>
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
    void populatePaths(const string& apiKey);
    void getDirections(const string& origin, const string& destination, const string& apiKey);
    void loadAndPopulatePaths(const string& apiKey);
    int getDistance(const string& origin, const string& destination, const string& apiKey);

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
