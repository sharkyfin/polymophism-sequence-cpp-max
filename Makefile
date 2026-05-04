CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
GUI_CXX = g++
THIRD_PARTY_DIR = third_party
IMGUI_DIR = third_party/imgui
IMGUI_REPO = https://github.com/ocornut/imgui.git
GLFW_DIR = third_party/glfw
GLFW_REPO = https://github.com/glfw/glfw.git
GLFW_BUILD_DIR = $(BUILD_DIR)/glfw
GLFW_LIB = $(GLFW_BUILD_DIR)/src/libglfw3.a

BUILD_DIR = build
APP = $(BUILD_DIR)/app
TEST_APP = $(BUILD_DIR)/tests
GUI_APP = $(BUILD_DIR)/gui
HEADERS = $(shell find core sequence deque matrix tests -name '*.hpp')
APP_SRC = main.cpp
TEST_SRC = tests/tests.cpp tests/matrix_tests.cpp
GUI_LOCAL_SRC = gui/main_gui.cpp
GUI_HEADERS = $(shell find gui -name '*.hpp')
IMGUI_SRC = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
GUI_SRC = $(GUI_LOCAL_SRC) $(IMGUI_SRC)
GUI_CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(GLFW_DIR)/include

UNAME_S := $(shell uname -s 2>/dev/null)
ifeq ($(OS),Windows_NT)
	GUI_PLATFORM_LIBS = -lopengl32 -lgdi32 -luser32 -lshell32
else ifeq ($(UNAME_S),Darwin)
	GUI_PLATFORM_LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore
else
	GUI_PLATFORM_LIBS = -lGL -ldl -lpthread -lm -lX11 -lXrandr -lXi -lXxf86vm -lXcursor
endif

.PHONY: all run gui run-gui imgui glfw test tests clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRC) $(TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(APP_SRC) $(TEST_SRC) -o $(APP)

run: $(APP)
	./$(APP)

gui: $(GUI_APP)

run-gui: $(GUI_APP)
	./$(GUI_APP)

imgui:
	@if [ ! -f "$(IMGUI_DIR)/imgui.cpp" ]; then \
		echo "Fetching Dear ImGui into $(IMGUI_DIR)"; \
		mkdir -p "$(THIRD_PARTY_DIR)"; \
		git clone --depth 1 "$(IMGUI_REPO)" "$(IMGUI_DIR)"; \
	fi

glfw:
	@if [ ! -f "$(GLFW_DIR)/CMakeLists.txt" ]; then \
		echo "Fetching GLFW into $(GLFW_DIR)"; \
		mkdir -p "$(THIRD_PARTY_DIR)"; \
		git clone --depth 1 "$(GLFW_REPO)" "$(GLFW_DIR)"; \
	fi
	@if ! command -v cmake >/dev/null 2>&1; then \
		echo "CMake is required to build GLFW"; \
		exit 1; \
	fi
	@if [ ! -f "$(GLFW_LIB)" ]; then \
		echo "Building GLFW"; \
		cmake -S "$(GLFW_DIR)" -B "$(GLFW_BUILD_DIR)" \
			-D GLFW_BUILD_EXAMPLES=OFF \
			-D GLFW_BUILD_TESTS=OFF \
			-D GLFW_BUILD_DOCS=OFF \
			-D BUILD_SHARED_LIBS=OFF; \
		cmake --build "$(GLFW_BUILD_DIR)"; \
	fi

test: $(TEST_APP)
	./$(TEST_APP)

tests:
	$(MAKE) test

$(TEST_APP): $(TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DTESTS_MAIN $(TEST_SRC) -o $(TEST_APP)

$(GUI_APP): imgui glfw $(GUI_LOCAL_SRC) $(GUI_HEADERS) $(HEADERS) | $(BUILD_DIR)
	$(GUI_CXX) $(GUI_CXXFLAGS) $(GUI_SRC) $(GLFW_LIB) $(GUI_PLATFORM_LIBS) -o $(GUI_APP)

clean:
	rm -rf $(APP) $(TEST_APP) $(GUI_APP) $(GLFW_BUILD_DIR) $(BUILD_DIR)/lab2 $(BUILD_DIR)/lab2_tests $(BUILD_DIR)/lab2_gui lab2 lab2_tests
