CXX = g++
CXXFLAGS = -Wall -g
TARGET = webservice
OBJS = webservice.o
all: $(TARGET)

# Link the target with object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lcpprest -lboost_system -lcrypto -lssl

webservice.o: webservice.cpp
	$(CXX) $(CXXFLAGS) -c webservice.cpp

clean:
	rm -f $(OBJS) $(TARGET)