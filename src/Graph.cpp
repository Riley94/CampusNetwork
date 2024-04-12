#include <curl/curl.h>

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
    adj[from].push_back(std::make_pair(to, distance));
    adj[to].push_back(std::make_pair(from, distance)); // add reverse path
}

// Populate paths between all locations using the getDistance function
void CampusGraph::populatePaths(const string& apiKey) {
    for (auto& source : nodes) {
        for (auto& destination : nodes) {
            if (source.first != destination.first) { // Avoid self-loops
                int distance = getDistance(source.second.coordinates, destination.second.coordinates, apiKey);
                if (distance != -1) { // Check for a valid distance
                    addPath(source.first, destination.first, distance);
                }
            }
        }
    }
}

void CampusGraph::loadAndPopulatePaths(const string& apiKey) {
    std::vector<std::pair<string, string>> connections = {
        {"Admin Center", "Oak Residence Hall"},
        {"Admin Center", "Oak Residence Hall"},
        {"Admin Center", "Oak Residence Hall"},
        {"Admin Center", "Oak Residence Hall"},
        {"Building B", "Building C"},
        // Add more predefined connections
    };

    for (const auto& connection : connections) {
        int distance = getDistance(connection.first, connection.second, apiKey);
        if (distance != -1) { // valid distance
            addPath(connection.first, connection.second, distance);
        }
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

void CampusGraph::getDirections(const string& origin, const string& destination, const string& apiKey) {
    CURL *curl;
    CURLcode res;
    string readBuffer;
    string url = "https://maps.googleapis.com/maps/api/directions/json?origin=" + origin + "&destination=" + destination + "&key=" + apiKey;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK) {
            cout << "API Response: " << readBuffer << endl;
            // Here you'd parse the JSON response to extract needed information
        } else {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
    }
}

int CampusGraph::getDistance(const string& origin, const string& destination, const string& apiKey) {
    CURL *curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        string url = "https://maps.googleapis.com/maps/api/directions/json?origin=" + origin + "&destination=" + destination + "&key=" + apiKey;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return -1; // Indicate failure
        } else {
            auto jsonResponse = json::parse(readBuffer);
            // Assuming the first route and leg is what we want
            int distance = jsonResponse["routes"][0]["legs"][0]["distance"]["value"].get<int>();
            return distance;
        }
    }
    return -1; // Indicate failure if curl couldn't initialize
}