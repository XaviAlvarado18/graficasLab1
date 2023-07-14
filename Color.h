#ifndef COLOR_H
#define COLOR_H
#include <iostream>

using namespace std;

class Color {

public:
    Color() : r(0), g(0), b(0) {} // Constructor por defecto que inicializa los valores a cero
    Color(int red, int green, int blue) {
        r = clampValue(red);
        g = clampValue(green);
        b = clampValue(blue);
    }

    int clampValue(int value) {
        if (value < 0) {
            return 0;
        } else if (value > 255) {
            return 255;
        }
        return value;
    }

    Color operator+(const Color& other) const {
        int newR = r + other.r;
        int newG = g + other.g;
        int newB = b + other.b;

        return Color(newR > 255 ? 255 : newR, newG > 255 ? 255 : newG, newB > 255 ? 255 : newB);
    }

    Color operator*(float factor) const {
        int newR = static_cast<int>(r * factor);
        int newG = static_cast<int>(g * factor);
        int newB = static_cast<int>(b * factor);

        return Color(newR > 255 ? 255 : newR, newG > 255 ? 255 : newG, newB > 255 ? 255 : newB);
    }

    friend ostream& operator<<(ostream& os, const Color& color) {
        os << "Color(" << color.r << ", " << color.g << ", " << color.b << ")";
        return os;
    }

    int b;
    int g;
    int r;
};

#endif