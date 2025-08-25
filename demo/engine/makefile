# 定义变量
CXX = g++                     # 使用 g++ 作为编译器
SRC = main.cpp                # 源文件
OBJ = main.o                  # 目标文件
EXEC = engine                   # 输出的可执行文件

# 包含路径和库路径
INCLUDE += -I`llvm-config --includedir`
LIB += `llvm-config --ldflags --libs --system-libs`

# 链接 clang 库
#LIB += -lclangDynamicASTMatchers \
       -lclangFrontendTool \
       -lclangHandleCXX \
       -lclangHandleLLVM \
       -lclangToolingASTDiff \
       -lclangToolingRefactor \
       -Wl,--whole-archive \
       -lclangARCMigrate \
       -lclangAST \
       -lclangASTMatchers \
       -lclangAnalysis \
       -lclangBasic \
       -lclangCodeGen \
       -lclangCrossTU \
       -lclangDriver \
       -lclangEdit \
       -lclangFormat \
       -lclangFrontend \
       -lclangIndex \
       -lclangLex \
       -lclangParse \
       -lclangRewrite \
       -lclangRewriteFrontend \
       -lclangSema \
       -lclangSerialization \
       -lclangStaticAnalyzerCheckers \
       -lclangStaticAnalyzerCore \
       -lclangStaticAnalyzerFrontend \
       -lclangTooling \
       -lclangToolingCore \
       -lclangToolingInclusions \
       -Wl,--no-whole-archive

# 编译选项
CXXFLAGS += -std=c++14 -g -Wall -fnon-call-exceptions -DPY_SSIZE_T_CLEAN

# 构建规则
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LIB)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(EXEC)