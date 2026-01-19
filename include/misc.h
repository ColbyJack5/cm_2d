#ifndef MISC_H
#define MISC_H
#include <cmath>
#include <optional>
#include <algorithm>
#include <string>

class Pos
{
    public:
        int x;
        int y;
        Pos(int x_, int y_){
            x=x_;
            y=y_;
        };
        Pos()=default;
        int DistanceTo(Pos otherPos) const {
            return std::sqrt(std::pow(std::abs(x - otherPos.x),2) + std::pow(std::abs(y - otherPos.y),2));
        }
        bool operator==(Pos other) const {
            return x == other.x && y == other.y;
        }
        bool operator!=(Pos other) const {
            return x != other.x || y != other.y;
        }
        Pos operator+(int numberToAdd) const {
            return Pos(x+numberToAdd,y+numberToAdd);
        }
        Pos operator-(int numberToSub) const {
            return Pos(x-numberToSub,y-numberToSub);
        }
        Pos operator*(int numberToMultiply) const {
            return Pos(x * numberToMultiply, y * numberToMultiply);
        }
        Pos operator/(int numberToDevide) const {
            return Pos(x / numberToDevide, y / numberToDevide);
        }
        Pos operator+(Pos other) const {
            return Pos(x + other.x, y + other.y);
        }
        Pos& operator+=(const Pos& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
        Pos operator-(Pos other) const {
            return Pos(x - other.x, y - other.y);
        }
        Pos& operator-=(const Pos& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        Pos operator*(Pos other) const {
            return Pos(x * other.x, y * other.y);
        }
        Pos& operator*=(const Pos& other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        Pos operator/(Pos other) const {
            return Pos(x / other.x, y / other.y);
        }
        Pos& operator/=(const Pos& other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }
};


struct Identifier
{
    std::optional<int> screen;
    int id;
    int generation;

    Identifier() : id(-1), generation(-1) {}

    Identifier(int id, int generation): id(id), generation(generation){}

    bool operator==(Identifier const& other) const {
        return /*screen == other.screen &&*/ id == other.id && generation == other.generation;
    }
};


inline std::string TrimPath(std::string path) {
    size_t lastDot = path.find_last_of('.');
    if (lastDot != std::string::npos) {
        path = path.substr(0, lastDot);
    }
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos) {
        path = path.substr(lastSlash + 1);
    }
    return path;
}

#endif