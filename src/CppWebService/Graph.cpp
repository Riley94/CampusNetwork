#include <curl/curl.h>
#include <cpprest/uri.h>
#include <utility>

#include "json.hpp"
#include "Graph.h"
using json = nlohmann::json;


using namespace std;

// Add a location to the graph
void CampusGraph::addLocation(const string& name, const string& coordinates) {
    Node node = {name, coordinates};
    nodes[name] = node;
}

// Add a weighted edge between two locations
void CampusGraph::addPath(const string& from, const string& to, int distance) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end()) {
        cerr << "Error: One of the nodes does not exist: " << from << " or " << to << endl;
        return;
    }
    // Avoid adding the same edge twice
    auto& edges_from = adj[from];
    if (find_if(edges_from.begin(), edges_from.end(), [&](const pair<string, int>& edge) {
        return edge.first == to;
    }) == edges_from.end()) {
        adj[from].push_back(make_pair(to, distance));
        adj[to].push_back(make_pair(from, distance)); // add reverse path
    }
}

// Callback function writes data to a std::string, and always returns size * nmemb
size_t writeCallBack(void *contents, size_t size, size_t nmemb, string *s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch(bad_alloc &e) {
        // Handle memory problem
        return 0;
    }
    return newLength;
}

// Function to fetch directions and return paths and nodes
std::pair<std::vector<std::pair<std::string, std::string>>, std::vector<std::tuple<std::string, std::string, double>>> CampusGraph::getDirections(const Node& origin, const Node& destination, const string& apiKey) {
    CURL *curl;
    CURLcode res;
    string readBuffer;
    auto encodedOrigin = web::uri::encode_data_string(origin.coordinates);
    auto encodedDestination = web::uri::encode_data_string(destination.coordinates);
    string url = "https://maps.googleapis.com/maps/api/directions/json?origin=" + encodedOrigin + "&destination=" + encodedDestination + "&key=" + apiKey;

    std::vector<std::pair<std::string, std::string>> locationsToAdd;
    std::vector<std::tuple<std::string, std::string, double>> pathsToAdd;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto jsonResponse = json::parse(readBuffer);
            if (jsonResponse.contains("routes") && !jsonResponse["routes"].empty()) {
                auto& firstRoute = jsonResponse["routes"][0];
                auto& firstLeg = firstRoute["legs"][0];
                auto& steps = firstLeg["steps"];

                // Handle adding locations and paths based on steps
                for (auto& step : steps) {
                    std::string start_vertex = std::to_string(step["start_location"]["lat"].get<double>()) + "," + std::to_string(step["start_location"]["lng"].get<double>());
                    std::string end_vertex = std::to_string(step["end_location"]["lat"].get<double>()) + "," + std::to_string(step["end_location"]["lng"].get<double>());
                    double distance = step["distance"]["value"].get<double>();

                    locationsToAdd.push_back({start_vertex, start_vertex});
                    locationsToAdd.push_back({end_vertex, end_vertex});
                    pathsToAdd.emplace_back(start_vertex, end_vertex, distance);
                }
            } else {
                cerr << "No routes found or invalid API response." << endl;
                if (jsonResponse.contains("error_message")) {
                    cerr << "API Error Message: " << jsonResponse["error_message"].get<string>() << endl;
                }
            }
        } else {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
    }
    return {locationsToAdd, pathsToAdd};
}

// Save graph to file
void CampusGraph::saveGraph(const string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file for writing: " << filename << endl;
        return;
    }

    try {
        for (const auto& node : nodes) {
            file << node.second.name << "," << node.second.coordinates << "\n";
            if (adj.find(node.first) == adj.end()) {
                cerr << "No edges found for node: " << node.first << endl;
                continue;
            }

            const auto& edges = adj.at(node.first);
            for (const auto& edge : edges) {
                if (nodes.find(edge.first) == nodes.end()) {
                    cerr << "Edge points to non-existent node: " << edge.first << endl;
                    continue;
                }
                file << nodes.at(edge.first).name << "," << edge.second << "\n";
            }
            file << "-\n"; // Marker for the end of a node's adjacency list
        }
    } catch (const std::exception& e) {
        cerr << "Exception caught during file write: " << e.what() << endl;
    }

    file.close();
}

// Load graph from file
void CampusGraph::loadGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    std::string line;
    std::string current_node_name;
    Node* current_node = nullptr;

    while (getline(file, line)) {
        if (line == "-") {
            current_node = nullptr; // Reset current node after its edges are processed
            continue;
        }

        if (!current_node) {
            size_t comma_pos = line.find(',');
            if (comma_pos == std::string::npos) {
                std::cerr << "Error parsing node line: " << line << std::endl;
                continue;
            }
            std::string name = line.substr(0, comma_pos);
            std::string coords = line.substr(comma_pos + 1);

            addLocation(name, coords);
            current_node_name = name;
            current_node = &nodes[current_node_name]; // This should not fail since we just added it
        } else {
            size_t comma_pos = line.find(',');
            if (comma_pos == std::string::npos) {
                std::cerr << "Error parsing edge data: " << line << std::endl;
                continue;
            }
            std::string connected_node = line.substr(0, comma_pos);
            int weight = std::stoi(line.substr(comma_pos + 1));
            if (nodes.find(connected_node) == nodes.end()) {
                std::cerr << "Referenced node not found: " << connected_node << std::endl;
                continue;
            }
            addPath(current_node_name, connected_node, weight);
        }
    }

    file.close();
}