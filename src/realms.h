#ifndef REALMS_H
#define REALMS_H

#include <iosfwd>
#include <string>
#include <vector>

const int MAX_NAME_LENGTH = 20;

struct Colour {
    int r, g, b;
};

enum class Stance {
    Biped, Quad, Taur, Thero,
    Count
};
enum class Wings {
    None, Back, Arm,
    Count
};

struct Species {
    int ident;
    std::string name;
    std::string abbrev;
    Stance stance;
    Wings wings;
    int height; // centimeters
    int homeRealm;
    Colour colour;
};

enum class Biome {
    Forest, Desert, Tundra, Grasslands, Savanna, Jungle,
    Aquatic,
    BiomeCount,

    None = 9999,
};

enum class MagicLevel {
    NoMagic, SemiMagic, FullMagic, Count
};
enum class TechLevel {
    NoTech, SemiTech, FullTech, Count
};

struct Link {
    int linkTo;
};

struct Realm {
    int ident;
    std::string name;
    int x, y;
    int primarySpecies;
    bool speciesHome;
    Biome biome;
    std::vector<Link> links;
    int diameter; // kilometres
    int populationDensity;
    int faction;
    bool factionHome;
    MagicLevel magicLevel;
    TechLevel techLevel;
    int work1, work2;

    int area() const;
    int population() const;

    bool addLink(Realm *target);
    bool hasLink(int to);
    const Link& getLink(int to);
};

struct Faction {
    int ident;
    std::string name;
    int r, g, b;
    int home;
};

struct World {
    std::vector<Realm*> realms;
    std::vector<Faction*> factions;
    std::vector<Species*> species;
    int maxX, maxY;

    bool writeToFile(const std::string &filename) const;
    bool readFromFile(const std::string &filename);

    Realm* getNearest(int x, int y, int notIdent = -1, double maxDist = 86543489);
    Realm* getNearest(int x, int y, std::vector<int> notIdent, double maxDist = 86543489);
    Realm* getNearestNotGroup(int x, int y, int notGroup);
    Realm* realmByIdent(int ident);
    Faction* factionByIdent(int ident);
    Species* speciesByIdent(int ident);
    std::vector<int> findPath(int from, int to);
    int findDistance(int from, int to);
    void setDistances(int ident);
    int factionSize(int ident) const;
};

std::ostream& operator<<(std::ostream &out, const Biome &biome);
std::ostream& operator<<(std::ostream &out, const TechLevel &level);
std::ostream& operator<<(std::ostream &out, const MagicLevel &level);
std::ostream& operator<<(std::ostream &out, const Stance &stance);
std::ostream& operator<<(std::ostream &out, const Wings &wing);

int percent(int value, int ofMax);
unsigned long long calcArea(double radius);
const std::string& trim(const std::string &text);
std::string& trim(std::string &text);
std::vector<std::string> explodeOnWhitespace(std::string text);
std::vector<std::string> explode(const std::string &text, char onChar);
double distance(double x1, double y1, double x2, double y2);
int strToInt(const std::string &text);
std::string intToString(long long number);
void rngInit(int seed);
int rngNext(int max);

// bb_generator.cpp
std::string makeName();
Species* makeSpecies();


template<class T>
const T& rngVector(const std::vector<T> &v) {
    return v[rngNext(v.size())];
}

template<class T>
bool contains(const std::vector<T> &v, const T &item) {
    for (const T &i : v) {
        if (i == item) return true;
    }
    return false;
}

#endif
