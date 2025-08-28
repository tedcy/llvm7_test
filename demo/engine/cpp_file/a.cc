extern "C" {
    int pow2(int val);

    void dyFunc();

    //符号未导出，所以还是C++风格的，asm指定C风格命名
    static int globalV0 asm("globalV0") = 0;
    static const int globalConstV0 asm("globalConstV0") = 0;
    static int globalV1 asm("globalV1") = 1;

    void pow3() {
        //使用全局变量防止被优化掉
        globalV0 = 2;
        *(int*)&globalConstV0 = 2;
        globalV1 = 2;
    }

    int pow4(int val) {
        dyFunc();
        return pow2(val) * pow2(val);
    }
}