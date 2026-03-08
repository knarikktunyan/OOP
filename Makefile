CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET1 = method1
TARGET2 = method2

SRC1 = ex_1.cpp
SRC2 = ex_2.cpp

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(SRC1)
	$(CXX) $(CXXFLAGS) $(SRC1) -o $(TARGET1)

$(TARGET2): $(SRC2)
	$(CXX) $(CXXFLAGS) $(SRC2) -o $(TARGET2)

$(TARGET3): $(SRC3)
	$(CXX) $(CXXFLAGS) $(SRC3) -o $(TARGET3)

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3)