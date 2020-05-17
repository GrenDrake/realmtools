#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "realms.h"

const int MAX_FACTIONS = 9;
const int MAX_HEIGHT = 50;
const int MAX_WIDTH = 90;
const int MAX_ITERATIONS = 100;
const int MAX_REALMS = 1600;
const int SPECIES_DIVISOR = 5;
const int MAX_SPECIES = MAX_REALMS / 5;
const int RNG_SEED = 234;
const int MAX_LINK_DIST = 10;

struct Color {
    int r, g, b;
};
std::vector<Color> factionColours = {
    {240,163,255},
    {0,117,220},
    {153,63,0},
    {76,0,92},
    {25,25,25},
    {0,92,49},
    {43,206,72},
    {255,204,153},
    {128,128,128},
    {148,255,181},
    {143,124,0},
    {157,204,0},
    {194,0,136},
    {0,51,128},
    {255,164,5},
    {255,168,187},
    {66,102,0},
    {255,0,16},
    {94,241,242},
    {0,153,143},
    {224,255,102},
    {116,10,255},
    {153,0,0},
    {255,255,128},
    {255,255,0},
    {255,80,5}
};

std::vector<std::string> usedNames;
std::vector<int> wordCount{
    1, 1, 2, 2, 2, 3
};
std::vector<std::string> syllableForms{
    "CV", "CV", "CV", "CVC", "CVC"
};
std::vector<std::string> C{
    "b", "d", "f", "g", "j",   "k", "l", "m", "n",
    "p", "r", "s", "t", "v",   "z"
};
std::vector<std::string> V{
    "a", "e", "i", "o", "u"
};
std::string makeName() {
    const int MIN_SYLLABLES = 2;
    const int MAX_SYLLABLES = 5;
    int words = rngVector(wordCount);
    std::string name;

    for (int i = 0; i < words; ++i) {
        if (i != 0) name += " ";
        int sylCount = MIN_SYLLABLES + rngNext(MAX_SYLLABLES - MIN_SYLLABLES);
        for (int j = 0; j < sylCount; ++j) {
            std::string form = rngVector(syllableForms);
            for (char c : form) {
                if (c == 'C') name += rngVector(C);
                if (c == 'V') name += rngVector(V);
            }
        }
    }

    name[0] = name[0] - ('a' - 'A');
    if (contains(usedNames, name)) {
        return makeName();
    } else {
        usedNames.push_back(name);
        return name;
    }
}

// https://stackoverflow.com/questions/9043805/test-if-two-lines-intersect-javascript-function
bool linesIntersect(double a, double b, double c, double d,
                        double p, double q, double r, double s) {
    double det, gamma, lambda;
    det = (c - a) * (s - q) - (r - p) * (d - b);
    if (det == 0) {
        return false;
    } else {
        lambda = ((s - q) * (r - a) + (p - r) * (s - b)) / det;
        gamma = ((b - d) * (r - a) + (c - a) * (s - b)) / det;
        return (0 < lambda && lambda < 1) && (0 < gamma && gamma < 1);
    }
};

bool validLink(World &world, Realm *origin, Realm *target, int minDist, int maxDist, int notWork, int notWorkLessThan) {
    if (!origin || !target) {
        return false;
    }

    if (target->work1 == notWork) {
        return false;
    }

    for (const Link &l : origin->links) {
        if (l.linkTo == target->ident) {
            return false;
        }
    }
    for (const Link &l : target->links) {
        if (l.linkTo == origin->ident) {
            return false;
        }
    }

    double dist = distance(origin->x, origin->y, target->x, target->y);
    if (dist < minDist || dist > maxDist) {
        return false;
    }

    if (target->work1 < notWorkLessThan) {
        return false;
    }

    for (Realm *a : world.realms) {
        for (const Link &l : a->links) {
            Realm *b = world.realmByIdent(l.linkTo);
            if (a == origin && b == target) continue;
            if (b == origin && a == target) continue;
            if (linesIntersect(origin->x, origin->y, target->x, target->y,
                               a->x, a->y, b->x, b->y)) {
               return false;
           }
        }
    }

    return true;
}

int assignGroup(World &world, int rootIdent, int groupId) {
    int count = 0;
    Realm *c = world.realmByIdent(rootIdent);
    if (!c || c->work1 >= 0) return 0;
    c->work1 = groupId;
    ++count;
    for (const Link &l : c->links) {
        count += assignGroup(world, l.linkTo, groupId);
    }
    return count;
}

int main() {
    rngInit(RNG_SEED);
    World world;

    const int minDist = 3;

    std::cerr << "Allocating and positioning realms...\n";
    for (unsigned i = 0; i < MAX_REALMS; ++i) {
        Realm *r = new Realm;
        r->ident = i + 1;

        // generate realm location
        int x, y, iter = 0;
        do {
            x = rngNext(MAX_WIDTH);
            y = rngNext(MAX_HEIGHT);
            ++iter;
        } while (iter < MAX_ITERATIONS && world.getNearest(x, y, -1, minDist));
        if (iter < MAX_ITERATIONS) {
            r->x = x;
            r->y = y;
        } else {
            delete r;
            std::cerr << "\tRealm generation terminated -- out of positions.\n";
            break;
        }

        world.realms.push_back(r);
    }
    std::cerr << "\tGenerated " << world.realms.size() << " realms.\n";
    if (world.realms.size() <= 0) return 1;

    std::cerr << "Assigning realm details...\n";
    for (Realm *r : world.realms) {
        r->name = makeName();
        r->faction = -1;
        r->factionHome = false;
        r->species = -1;
        r->diameter = 700 + rngNext(600);
        r->populationDensity = 15 + rngNext(70);
        r->biome[0] = static_cast<Biome>(rngNext(static_cast<int>(Biome::BiomeCount)));
        if (r->biome[0] == Biome::Mountain || rngNext(3) == 1) {
            r->biome[1] = static_cast<Biome>(rngNext(static_cast<int>(Biome::Mountain)));
        } else r->biome[1] = Biome::None;
    }


    std::cerr << "Assigning initial links...\n";
    for (Realm *r : world.realms) {
        Realm *target = world.getNearest(r->x, r->y, r->ident);
        if (!target) continue;
        r->addLink(target);
    }

    std::cerr << "Eliminating groups...\n";
    int groupCount = 9, lastGroupCount = 4;
    while (groupCount > 1 && groupCount != lastGroupCount) {
        lastGroupCount = groupCount;
        groupCount = 0;
        for (Realm *r : world.realms) {
            r->work1 = -1;
        }

        int nextGroup = 1;
        for (Realm *r : world.realms) {
            if (r->work1 < 0) {
                assignGroup(world, r->ident, nextGroup++);
                ++groupCount;
            }
        }
        std::cerr << '\t' << groupCount << " groups remain.\n";
        if (groupCount <= 1) break;

        std::set<int> groupsDone;
        for (Realm *r : world.realms) {
            if (groupsDone.count(r->work1)) continue;

            int iter = 0;
            Realm *target = nullptr;
            std::vector<int> forbid{r->ident};
            do {
                ++iter;
                if (iter >= MAX_ITERATIONS) break;
                target = world.getNearest(r->x, r->y, forbid);
                if (target) {
                    forbid.push_back(target->ident);
                    if (!validLink(world, r, target, 0, MAX_LINK_DIST, r->work1, -1000)) {
                        target = nullptr;
                    }
                }
            } while (!target);

            if (target && r->addLink(target)) {
                groupsDone.insert(r->work1);
                groupsDone.insert(target->work1);
            }
        }
    }

    std::cerr << "Expanding some leafs...\n";
    for (Realm *r : world.realms) {
        if (r->links.size() != 1) continue;
        world.setDistances(r->ident);
        int iter = 0;
        Realm *target = nullptr;
        std::vector<int> forbid{r->ident};
        do {
            ++iter;
            if (iter >= MAX_ITERATIONS) break;
            target = world.getNearest(r->x, r->y, forbid);
            if (target) {
                forbid.push_back(target->ident);
                if (!validLink(world, r, target, 0, MAX_LINK_DIST, -1, 6)) {
                    target = nullptr;
                }
            }
        } while (!target);
        r->addLink(target);
    }


    std::cerr << "Assigning factions...\n";
    // allocate the faction data
    for (int i = 0; i < MAX_FACTIONS; ++i) {
        Faction *f = new Faction;
        f->ident = i;
        if (i == 0) f->name = "Independant";
        else        f->name = makeName();
        f->r = factionColours[i].r;
        f->g = factionColours[i].g;
        f->b = factionColours[i].b;
        f->home = -1;
        world.factions.push_back(f);
    }

    // determine realm factions
    std::vector<Realm*> homes;
    for (int i = 1; i < MAX_FACTIONS; ++i) {
        Realm *r = nullptr;
        bool valid;
        do {
            valid = true;
            int id = 1 + rngNext(world.realms.size());
            r = world.realmByIdent(id);
            for (const Realm *c : homes) {
                int dist = world.findPath(id, c->ident).size();
                if (dist < 5) {
                    valid = false;
                    break;
                }
            }
        } while (!valid);
        if (!valid) {
            std::cerr << "\tFailed to place faction " << i << ".\n";
        }
        r->faction = i;
        r->factionHome = true;
        world.factions[i]->home = r->ident;
        homes.push_back(r);
    }

    int assigned;
    do {
        assigned = 0;
        for (Realm *r : world.realms) {
            if (r->faction >= 0) continue;
            std::map<int, int> neighbors;
            for (const Link &l : r->links) {
                const Realm *t = world.realmByIdent(l.linkTo);
                if (t && t->faction > 0) {
                    ++neighbors[t->faction];
                }
            }

            if (neighbors.size() > 1) {
                r->faction = 0;
                ++assigned;
            } else if (neighbors.size() == 1) {
                r->faction = neighbors.begin()->first;
                ++assigned;
            }
        }
    } while (assigned > 0);

    // clean up any missed factions
    for (Realm *r : world.realms) {
        if (r->faction < 0) r->faction = 0;
    }

    std::cerr << "Building species...\n";
    std::vector<Stance> stanceList{
        Stance::Biped, Stance::Biped, Stance::Biped, Stance::Biped,
        Stance::Biped, Stance::Quad, Stance::Quad, Stance::Taur,
        Stance::Taur, Stance::Thero,
    };
    std::vector<Wings> wingList{
        Wings::None, Wings::None, Wings::None, Wings::None,
        Wings::Arm, Wings::Arm, Wings::Back,
    };
    unsigned speciesToMake = MAX_SPECIES;
    if (speciesToMake > world.realms.size() / SPECIES_DIVISOR) {
        speciesToMake = world.realms.size() / SPECIES_DIVISOR;
    }
    for (unsigned i = 0; i < speciesToMake; ++i) {
        Species *s = new Species;
        s->ident = i + 1;
        s->name = makeName();
        s->height = 50 + rngNext(150);
        s->isBeastFolk = rngNext(13) == 1;
        s->stance = rngVector(stanceList);
        if (s->stance == Stance::Taur)  s->wings = Wings::None;
        else                            s->wings = rngVector(wingList);
        world.species.push_back(s);
    }
    unsigned counter = 0;
    for (Realm *r : world.realms) {
        Species *s = nullptr;
        if (counter < world.species.size()) {
            s = world.species[counter];
        } else {
            s = rngVector(world.species);
        }
        r->species = s->ident;
        ++counter;
    }


    std::cerr << "Saving data to file...\n";
    world.writeToFile("realms.txt");

    return 0;
}
