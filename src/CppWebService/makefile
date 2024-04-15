CXX = g++
CXXFLAGS = -Wall -g
TARGET = webservice
OBJS = webservice.o Graph.o
all: $(TARGET)

# Link the target with object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lcpprest -lboost_system -lcrypto -lssl -lcurl

webservice.o: webservice.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c webservice.cpp

Graph.o: Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c Graph.cpp

clean:
	rm -f $(OBJS) $(TARGET)