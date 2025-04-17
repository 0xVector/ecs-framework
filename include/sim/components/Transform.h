#ifndef TRANSFORM_H
#define TRANSFORM_H

struct Transform {
    int x;
    int y;

    Transform():
        x(0), y(0) {
    }

    Transform(int x, int y):
        x(x), y(y) {
    }
};

#endif //TRANSFORM_H
