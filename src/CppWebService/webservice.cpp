#include <cpprest/http_listener.h>
#include <cpprest/json.h>
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
#include <chrono>
#include <signal.h>
#include <thread>
#include "Graph.h"
#include "json.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

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

void handle_options(http_request request)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
    request.reply(response);
}

pair<vector<string>, int> dijkstra(const CampusGraph& g, const string& start, const string& end) {
    // Retrieve the adjacency list via getter
    const auto& adj = g.getAdj();

    try {
        if (adj.find(start) == adj.end()) {
            cerr << "Error: Start node does not exist: " << start << endl;
        }
        if (adj.find(end) == adj.end()) {
            cerr << "Error: End node does not exist: " << end << endl;
        }
        if (adj.find(start) == adj.end() || adj.find(end) == adj.end()) {
            return {};
        }


        unordered_map<string, int> distances;
        unordered_map<string, string> predecessors;
        for (const auto& node : adj) {
            distances[node.first] = numeric_limits<int>::max();  // Initialize all distances to infinity
        }
        distances[start] = 0;  // Distance to start is zero

        // Comparator for the priority queue
        auto compare = [&distances](const pair<int, string>& lhs, const pair<int, string>& rhs) {
            return lhs.first > rhs.first;  // Compare based on distances
        };

        priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(compare)> pq(compare);
        pq.push({0, start});  // Start from the start node

        while (!pq.empty()) {
            auto [current_distance, current] = pq.top(); pq.pop();

            cout << "Visiting node: " << current << " with distance: " << current_distance << endl;

            if (current_distance > distances[current]) {
                continue; // Skip outdated entries in the priority queue
            }

            for (const auto& [neighbor, weight] : adj.at(current)) {
                int newDist = current_distance + weight;
                if (newDist < distances[neighbor]) {
                    distances[neighbor] = newDist;
                    predecessors[neighbor] = current;
                    pq.push({newDist, neighbor});
                    cout << "Updating distance for " << neighbor << " to " << newDist << endl;
                }
            }
        }

        vector<string> path;
        string at = end;
        while (at != start) {
            path.push_back(at);
            at = predecessors.at(at);
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        cout << "Path found from " << start << " to " << end << endl;
        return {path, distances[end]};  // Return the path and the total distance
    } catch (const std::exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
        return {};
    }
}

CampusGraph initializeGraph()
{
    cout << "Refill graph? (y/n): ";
    char refill;
    cin >> refill;
    CampusGraph graph;
    if (refill == 'y') {
        // Path to your JSON file
        string path = "scripts/building_to_latlong.json";

        // Open the file stream
        ifstream inputFile(path);

        // Parse the file stream into a JSON object
        nlohmann::json j;
        inputFile >> j;

        // load environment variables
        unordered_map<string, string> env = loadEnv(".env");

        // graph.addLocation("ROYAL HALL", "39.0336435, -94.5769808");
        for (const auto& item : j.items()) {
            graph.addLocation(item.key(), item.value());
        }

        const auto nodes = graph.getNodes();
        for (auto outer = nodes.begin(); outer != nodes.end(); ++outer) {
            auto inner = outer; // Start from the next element after outer
            ++inner; // Move to the next element
            for (; inner != nodes.end(); ++inner) {
                ucout << U("Calculating distance from ") << outer->first << U(" to ") << inner->first << std::endl;
                // Example usage:
                auto [newLocations, newPaths] = graph.getDirections(outer->second, inner->second, env["API_KEY"]);
                if (!newLocations.empty()) {
                    // Add new locations to the graph
                    for (auto& loc : newLocations) {
                        graph.addLocation(loc.first, loc.second);
                    }
                    // Connect the origin to the first new location
                    auto firstVertex = newLocations.begin()->first;
                    graph.addPath(outer->first, firstVertex, 0);

                    // Connect the last new location to the destination
                    auto lastVertex = newLocations.rbegin()->first;
                    graph.addPath(lastVertex, inner->first, 0);
                }
                // Add new paths to the graph
                for (auto& path : newPaths) {
                    graph.addPath(std::get<0>(path), std::get<1>(path), std::get<2>(path));
                }
            }
        }

        if (graph.getAdj().empty()) {
            std::cout << "The adjacency list is empty." << std::endl;
        } else {
            std::cout << "Attempting to save graph..." << std::endl;
            graph.saveGraph("graph.txt");
            graph.saveGraphToDot("graph.dot");
        }
    } else {
        // Load the graph from a file
        graph.loadGraph("graph.txt");
    }
    return graph;
}

CampusGraph graph = initializeGraph();

void handlePost(http_request request) {
    // Extract JSON body from the request
    request.extract_json().then([=](json::value requestJson) {
        ucout << U("Received request: ") << requestJson.serialize() << std::endl;

        // Prepare the response object
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));

        // Parse start and end locations from the JSON body
        auto startLocation = requestJson[U("start")].as_string();
        auto destinationLocation = requestJson[U("end")].as_string();

        auto result = dijkstra(graph, startLocation, destinationLocation);
        vector<string> path = result.first;
        int distance = result.second;

        // Assuming you're using the nlohmann::json library
        nlohmann::json responseJson;
        nlohmann::json jsonPath = nlohmann::json::array();

        // Fill the JSON array with the path nodes
        for (const auto& node : path) {
            jsonPath.push_back(node);
        }

        // Create a JSON response
        responseJson["path"] = jsonPath;
        responseJson["distance"] = distance;

        // Send the response, making sure to convert the JSON object to a string if necessary
        // For example, using C++ REST SDK:
        response.set_body(responseJson.dump());
        request.reply(response);

    }).then([=](pplx::task<void> t) {
        try {
            t.get();  // Re-throw any exceptions caught during processing
        } catch (const http_exception &e) {
            // Handle exceptions and ensure CORS headers are set in error responses
            http_response response(status_codes::BadRequest);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            response.set_body(U("Error processing request"));
            request.reply(response);
        }
    });
}

bool isRunning = true;  // Control flag for the main loop

void handle_signals(int signal) {
    if (signal == SIGINT) {
        isRunning = false;  // Stop the server loop
    }
}

int main() {
    signal(SIGINT, handle_signals);  // Setup signal handling

    uri_builder uri(U("http://localhost:8080"));
    auto addr = uri.to_uri().to_string();
    http_listener listener(addr);

    listener.support(methods::POST, handlePost);
    listener.support(methods::OPTIONS, handle_options);

    try {
        listener.open().then([&listener]() {
            std::cout << "Starting to listen at: " << listener.uri().to_string() << std::endl;
        }).wait();

        // Use a simple loop to keep the main thread alive
        while (isRunning) {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Sleep to reduce CPU usage
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "Shutting down..." << std::endl;
    listener.close().wait();
    return 0;
}