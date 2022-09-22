#ifndef OPENGLPLAYGROUND_GEOMETRY_H
#define OPENGLPLAYGROUND_GEOMETRY_H

struct Point {
    float x, y, z;
    Point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
    Point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
    Point operator -(const Point& a) const {
        return Point(x - a.x, y - a.y, z - a.z);
    }
    Point operator +(const Point& a) const {
        return Point(x + a.x, y + a.y, z + a.z);
    }
    Point operator *(const float& a) const {
        return Point(x * a, y * a, z * a);
    }
    Point operator /(const float& a) const {
        return Point(x / a, y / a, z / a);
    }
    float* toArray() {
        float array[] = { x, y, z, 1.0f };
        return array;
    }
};

struct Vertex {
    float Position[4];
    float Color[4];
    bool isSelected = false;
    float SelectedColor[4] = {.5f, .5f, .5f, .5f};
    bool swapped = false;

    Vertex(std::vector<float> pos, std::vector<float> color) {

        SetCoords(pos.data());
        SetColor(color.data());
    }

    Vertex() = default;

    Vertex(const Point &p) {

        Position[0] = p.x;
        Position[1] = p.y;
        Position[2] = p.z;
        Position[3] = 1.0f;

        Color[0] = 0.0f;
        Color[1] = 1.0f;
        Color[2] = 1.0f;
        Color[3] = 1.0f;
    }

    void SetCoords(float *coords) {
        Position[0] = coords[0];
        Position[1] = coords[1];
        Position[2] = coords[2];
        Position[3] = coords[3];
    }
    void SetColor(float *color) {
        Color[0] = color[0];
        Color[1] = color[1];
        Color[2] = color[2];
        Color[3] = color[3];
    }
};

#endif //OPENGLPLAYGROUND_GEOMETRY_H
