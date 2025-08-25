#include <stdio.h>

extern "C" {
    void dyFunc() {
        printf("%s\n", __func__);
    }

    void byLinked() {
        printf("%s\n", __func__);
    }
}