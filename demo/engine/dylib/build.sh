g++ -std=c++11 dylib.cpp -c -fPIC
g++ -std=c++11 dylib.o -shared -o libdylib.so