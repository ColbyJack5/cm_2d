#ifndef MISC_H
#define MISC_H
#include <cmath>
#include <optional>

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
        Pos operator+(int numberToAdd) const {
            return Pos(x+numberToAdd,y+numberToAdd);
        }
        Pos operator-(int numberToSub) const {
            return Pos(x-numberToSub,y-numberToSub);
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



#endif