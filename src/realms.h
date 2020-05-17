#ifndef REALMS_H
#define REALMS_H

#include <iosfwd>
#include <string>
#include <vector>

const int MAX_NAME_LENGTH = 20;

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
    Stance stance;
    Wings wings;
    bool isBeastFolk;
    int height; // centimeters
    std::string skinType;
};

enum class Biome {
    Forest, Desert, Tundra, Grasslands, Savanna, Jungle,
    Mountain,
    BiomeCount,

    None = 9999,
};

struct Link {
    int linkTo;
};

struct Realm {
    int ident;
    std::string name;
    int x, y;
    int species;
    Biome biome[2];
    std::vector<Link> links;
    int diameter;
    int populationDensity;
    int faction;
    bool factionHome;
    int work1;

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
};

std::ostream& operator<<(std::ostream &out, const Biome &biome);
std::ostream& operator<<(std::ostream &out, const Stance &stance);
std::ostream& operator<<(std::ostream &out, const Wings &wing);

std::string intToString(int number);

int percent(int value, int ofMax);
const std::string& trim(const std::string &text);
std::string& trim(std::string &text);
std::vector<std::string> explodeOnWhitespace(std::string text);
std::vector<std::string> explode(const std::string &text, char onChar);
double distance(double x1, double y1, double x2, double y2);
int strToInt(const std::string &text);
std::string intToString(int number);
void rngInit(int seed);
int rngNext(int max);

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
