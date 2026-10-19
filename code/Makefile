CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Wshadow

all: stub

stub: stub.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# 内存/未定义行为检查（M7 用；先跑通功能再开）
asan: stub.cpp
	$(CXX) -O1 -g -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer $< -o stub_asan

# 数据竞争检查（写多线程搜索时用）
tsan: stub.cpp
	$(CXX) -O1 -g -std=c++17 -fsanitize=thread $< -o stub_tsan

clean:
	rm -f stub stub_asan stub_tsan

.PHONY: all asan tsan clean
