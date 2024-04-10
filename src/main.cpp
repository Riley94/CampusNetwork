#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <list>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "json.hpp"
#include "Graph.h"

using json = nlohmann::json;

using namespace std;

// Function to load .env file contents into a map
unordered_map<string, string> loadEnv(const string& filePath) {
    unordered_map<string, string> envMap;
    ifstream file(filePath);
    string line;

    while (getline(file, line)) {
        istringstream lineStream(line);
        string key, value;

        if (getline(lineStream, key, '=')) {
            if (getline(lineStream, value)) {
                envMap[key] = value;
            }
        }
    }

    return envMap;
}

vector<string> dijkstra(const CampusGraph& g, const string& start, const string& end) {
    // Retrieve the adjacency list via getter
    const auto& adj = g.getAdj();

    unordered_map<string, int> distances;
    unordered_map<string, string> predecessors;
    for (const auto& node : adj) {
        distances[node.first] = numeric_limits<int>::max();
    }
    distances[start] = 0;

    auto compare = [&distances](const string& lhs, const string& rhs) {
        return distances[lhs] > distances[rhs];
    };
    priority_queue<string, vector<string>, decltype(compare)> pq(compare);
    pq.push(start);

    while (!pq.empty()) {
        string current = pq.top();
        pq.pop();

        // Here we use adj, which was obtained from g.getAdj()
        for (const auto& neighbor : adj.at(current)) {
            int newDist = distances[current] + neighbor.second;
            if (newDist < distances[neighbor.first]) {
                distances[neighbor.first] = newDist;
                predecessors[neighbor.first] = current;
                pq.push(neighbor.first);
            }
        }
    }

    // Reconstruct the path
    vector<string> path;
    for (string at = end; at != start; at = predecessors[at]) {
        path.push_back(at);
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

int main()
{
    string apiKey;
    string filePath = "../.env"; // Adjust path as needed
    auto envMap = loadEnv(filePath);

    // Path to your JSON file
    string path = "scripts/building_to_latlong.json";

    // Open the file stream
    ifstream inputFile(path);

    // Parse the file stream into a JSON object
    json j;
    inputFile >> j;

    CampusGraph graph;

    // graph.addLocation("ROYAL HALL", "39.0336435, -94.5769808");
    for (const auto& item : j.items()) {
        graph.addLocation(item.key(), item.value());
    }

    graph.populatePaths(envMap["API_KEY"]);

    string origin = "40.6655101,-73.89188969999998"; // Example coordinates
    string destination = "40.6905615,-73.9976592"; // Example coordinates

    /*
    graph.getDirections(origin, destination, apiKey);

    int distance = graph.getDistance(origin, destination, apiKey);

    if (distance != -1) {
        cout << "Distance: " << distance << " meters" << endl;
    } else {
        cout << "Failed to retrieve distance." << endl;
    } */

    // Parse the JSON response to extract the distance
    // Add the distance as an edge weight in your graph

    /* string start, end;
    cout << "Enter start location: ";
    cin >> start;
    cout << "Enter end location: ";
    cin >> end;

    // Inside main, after getting user input and filling the graph
    vector<string> path = dijkstra(graph, start, end);
    int totalDistance = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cout << path[i] << " -> ";
        // Calculate totalDistance if needed, by looking up each edge in the graph
    }
    cout << path.back() << endl; */

    return 0;
}