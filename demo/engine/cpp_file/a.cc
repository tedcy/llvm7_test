extern "C" {
    int pow2(int val);

    void dyFunc();

    static int globalValue = 0;

    int pow3() {
        return globalValue;
    }

    int pow4(int val) {
        dyFunc();
        return pow2(val) * pow2(val);
    }
}