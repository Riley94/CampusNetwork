#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

void handle_options(http_request request)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
    request.reply(response);
}

void handlePost(http_request request) {

    // Add CORS headers to response
    http_response response(status_codes::OK);
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));

    // Extract JSON body from the request
    request.extract_json().then([=](json::value requestJson) {
        ucout << U("Received request: ") << requestJson.serialize() << std::endl;

        // Parse start and end locations from the JSON body
        auto startLocation = requestJson[U("start")].as_string();
        auto destinationLocation = requestJson[U("end")].as_string();

        // Dummy example - Replace this with your actual graph algorithm
        auto path = U("Shortest path between ") + startLocation + U(" and ") + destinationLocation;
        auto distance = 42; // Dummy distance

        // Create a JSON response
        json::value responseJson;
        responseJson[U("path")] = json::value::string(path);
        responseJson[U("distance")] = json::value::number(distance);

        // Set the response
        request.reply(status_codes::OK, responseJson);
    }).wait();
}

int main() {
    uri_builder uri(U("http://localhost:8080"));
    auto addr = uri.to_uri().to_string();
    http_listener listener(addr);

    listener.support(methods::POST, handlePost);
    listener.support(methods::OPTIONS, handle_options);

    try {
        listener
            .open()
            .then([&listener](){ ucout << U("Starting to listen at: ") << listener.uri().to_string() << std::endl; })
            .wait();

        std::string line;
        std::getline(std::cin, line);
    } catch (const std::exception &e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}