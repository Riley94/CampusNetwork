# Campus Navigation System

# Overview
  The Campus Navigation System is designed to help users navigate large campus environments efficiently. Utilizing a combination of Flask, C++, and Google Maps API, this system allows users to select starting and ending points on a map, and calculates the shortest path between these points. The system is split into a frontend developed with Flask, serving JavaScript, HTML, and CSS, and a backend implemented in C++ using the C++ REST SDK.

# Graph Representation
  The C++ graph consists of an unordered map of string-node pairs, where the string is the name and the node is the graph node consisting of coordinates and a name. These nodes are stored in an adjacency list which can be referred to, to either rebuild the graph or access it. It has member functions to add paths and nodes to graph. The Dijikstra algorithm is separate from the graph implementation (simply a design choice).

# Features
  Interactive map with clickable elements for setting start and end locations.
Display of path information and total distance using a responsive web interface.
Backend API in C++ for robust and efficient graph operations.

# Prerequisites
Before you start, ensure you have the following installed:

- Python 3.x
- Flask
- C++ Compiler (e.g., g++, clang)
- C++ REST SDK
- Google Maps API key

# Installation

Setting up the Backend

- Clone the repository:

`git clone [repository-url]`
`cd [repository-folder]`

- Compile the C++ Webservice:

`cd src/CppWebservice`
`make all  # or use your specific build command`

- To run:

`./webservice`

Setting up the Frontend

- Navigate to the frontend directory:

`cd src/frontend`

- Install Python dependencies:

`pip install flask `

- To run:

`python app.py`

# Running the Application

- Start the C++ API server:

`./webservice`

- Run the Flask server:

`python app.py`

# Access the Application:

Open your web browser and navigate to http://127.0.0.1:5000/.
Usage
Select Starting and Ending Locations:
Click on the map to set your start and end points. The locations are marked and can be adjusted as needed.
Calculate the Path:
Press the submit button. The path, including nodes in coordinates and the total distance, will be displayed in the information box.

License
Distributed under the MIT License. See LICENSE for more information.
