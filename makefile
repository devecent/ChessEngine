
CXX = g++
CXXFLAGS = -std=c++20 -O2


SRC_DIR = src

SRCS = $(wildcard $(SRC_DIR)/*.cpp)


TARGET = devecent.exe


all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)


$(BUILD_DIR):
	mkdir $(BUILD_DIR)

clean:
	del /Q $(TARGET)
