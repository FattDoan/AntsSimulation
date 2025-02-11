CXX=g++
CXXFLAGS= -Wall -std=c++17 -g $(shell pkg-config --cflags gtkmm-4.0)
LDFLAGS = $(shell pkg-config --libs gtkmm-4.0)

SRC_BACKEND_DIR = backend
SRC_GUI_DIR = GUI
BUILD_DIR = build
TEST_DIR = test

SRC_BACKEND_FILES = $(wildcard $(SRC_BACKEND_DIR)/*.cpp)
SRC_GUI_FILES = $(wildcard $(SRC_GUI_DIR)/*.cpp)

OBJ_BACKEND_FILES = $(patsubst $(SRC_BACKEND_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_BACKEND_FILES))
OBJ_GUI_FILES = $(patsubst $(SRC_GUI_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_GUI_FILES))

EXECUTABLE = main
TEST_EXECUTABLE = testExec

all: $(EXECUTABLE)

$(BUILD_DIR)/%.o: $(SRC_BACKEND_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_GUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJ_BACKEND_FILES) $(OBJ_GUI_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)/*.o $(EXECUTABLE)

test:
	g++ -Wall -std=c++17 $(TEST_DIR)/test.cpp $(SRC_BACKEND_DIR)/coord.cpp -o $(TEST_EXECUTABLE) && ./$(TEST_EXECUTABLE)
