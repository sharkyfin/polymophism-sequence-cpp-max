CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

BUILD_DIR = build
APP = $(BUILD_DIR)/lab2
TEST_APP = $(BUILD_DIR)/lab2_tests
HEADERS = $(shell find core sequence deque matrix tests -name '*.hpp')
APP_SRC = main.cpp
TEST_SRC = tests/tests.cpp tests/matrix_tests.cpp

.PHONY: all run test tests clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRC) $(TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(APP_SRC) $(TEST_SRC) -o $(APP)

run: $(APP)
	./$(APP)

test: $(TEST_APP)
	./$(TEST_APP)

tests:
	$(MAKE) test

$(TEST_APP): $(TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DTESTS_MAIN $(TEST_SRC) -o $(TEST_APP)

clean:
	rm -f $(APP) $(TEST_APP) lab2 lab2_tests tests
