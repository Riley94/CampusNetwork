CXX = g++
CXXFLAGS = -Wall -g
TARGET = main
OBJS = main.o Graph.o
all: $(TARGET)

# Link the target with object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lcurl

main.o: main.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Graph.o: Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c Graph.cpp

clean:
	rm -f $(OBJS) $(TARGET)
	