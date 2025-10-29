# 定义编译器选项
CXX := g++
CXXFLAGS := -std=c++14 -pthread -g -I..

# 自动查找源文件
LIB_SRC := $(wildcard ../lib/*.cpp)
NETWORK_SRC := $(wildcard ../network/*.cpp)
UTIL_SRC := $(wildcard ../util/*.cpp)
SRC := $(LIB_SRC) $(NETWORK_SRC) $(UTIL_SRC)

# 目标文件
SERVER_TARGET := server
CLIENT_TARGET := client
TEST_TARGET := test

# 默认目标
all: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

# 使用 $^ 表示所有依赖文件， $@ 表示目标文件，避免重复输入文件名。
# 编译 server
$(SERVER_TARGET): server.cpp $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 编译 client
$(CLIENT_TARGET): client.cpp $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 编译 test
$(TEST_TARGET): test.cpp $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 清理生成的文件
clean:
	rm $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)
