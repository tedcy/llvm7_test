clang++ -m64 -g -std=c++0x -fno-use-cxa-atexit -fnon-call-exceptions -c -emit-llvm a.cc
llvm-dis a.bc

# got模式（加入全局变量以后，MCJIT不支持）
# llc -filetype=obj a.bc -o a.o

# 绝对地址模式
llc -filetype=obj -code-model=large a.bc -o a.o