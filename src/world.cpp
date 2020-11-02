#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <queue>

#include "realms.h"

const double PI = 3.14159;

int Realm::area() const {
    double radius = diameter / 2.0;
    return PI * radius * radius;
}

int Realm::population() const {
    return area() * populationDensity;
}

bool Realm::hasLink(int to) {
    for (const Link &l : links) {
        if (l.linkTo == to) return true;
    }
    return false;
}

bool Realm::addLink(Realm *target) {
    if (!target) return false;

    if (!hasLink(target->ident)) {
        links.push_back(Link{target->ident});
        target->links.push_back(Link{ident});
        return true;
    }
    return false;
}

const Link BAD_LINK{ -1 };
const Link& Realm::getLink(int to) {
    for (const Link &l : links) {
        if (l.linkTo == to) return l;
    }
    return BAD_LINK;
}


bool World::writeToFile(const std::string &filename) const {
    std::ofstream realmList(filename);
    if (!realmList) return false;

    for (const Faction *f : factions) {
        realmList << "F | " << std::setw(3) << f->ident << " | ";
        realmList << std::setw(3) << f->r << " | ";
        realmList << std::setw(3) << f->g << " | ";
        realmList << std::setw(3) << f->b << " | ";
        realmList << std::setw(3) << f->home << " | ";
        realmList << f->name << "\n";
    }

    for (const Species *s : species) {
        realmList << "S | ";
        realmList << std::setw(3) << s->ident << " | ";
        realmList << std::setw(2) << static_cast<int>(s->stance) << " | ";
        realmList << std::setw(2) << static_cast<int>(s->wings) << " | ";
        realmList << s->isBeastFolk << " | ";
        realmList << std::setw(3) << s->height << " | ";
        realmList << std::setw(4) << s->homeRealm << " | ";
        realmList << s->name << "\n";
    }

    for (const Realm *r : realms) {
        realmList << "R | ";
        realmList << std::setw(3) << r->ident << " | ";
        realmList << std::setw(3) << r->x << " | " << std::setw(3) << r->y << " | ";
        realmList << std::setw(4) << r->diameter << " | ";
        realmList << std::setw(3) << r->faction << " | ";
        realmList << r->factionHome << " | ";
        realmList << std::setw(4) << r->populationDensity << " | ";
        realmList << std::setw(4) << r->species << " | ";
        realmList << std::setw(4) << static_cast<int>(r->biome[0]) << " | ";
        realmList << std::setw(4) << static_cast<int>(r->biome[1]) << " | ";
        bool first = true;
        for (unsigned i = 0; i < r->links.size(); ++i) {
            if (first) first = false;
            else realmList << " ; ";
            realmList << r->links[i].linkTo;
        }
        realmList << " | " << r->name << '\n';
    }

    return true;
}


bool World::readFromFile(const std::string &filename) {
    int newMaxX = 0, newMaxY = 0;
    std::vector<Realm*> newRealms;
    std::vector<Faction*> newFactions;
    std::vector<Species*> newSpecies;

    std::ifstream realmList(filename);
    if (!realmList) return false;
    std::string line;
    int lineNo = 0;
    while (std::getline(realmList, line)) {
        ++lineNo;
        auto parts = explode(line, '|');
        if (parts.empty()) continue;

        if (parts[0] == "R") {
            if (parts.size() != 13) {
                std::cerr << lineNo << ": realm has wrong number of data items.\n";
                continue;
            }

            Realm *r = new Realm;
            r->ident        = strToInt(parts[1]);
            r->x            = strToInt(parts[2]);
            r->y            = strToInt(parts[3]);
            r->diameter     = strToInt(parts[4]);
            r->faction      = strToInt(parts[5]);
            r->factionHome  = strToInt(parts[6]);
            r->populationDensity = strToInt(parts[7]);
            r->species      = strToInt(parts[8]);
            r->biome[0]     = static_cast<Biome>(strToInt(parts[9]));
            r->biome[1]     = static_cast<Biome>(strToInt(parts[10]));
            r->name         = parts[12];
            if (r->x > newMaxX) newMaxX = r->x;
            if (r->y > newMaxY) newMaxY = r->y;

            auto links = explode(parts[11], ';');
            for (const std::string &s : links) {
                int to = strToInt(s);
                if (to < 0) continue;
                r->links.push_back(Link{to});
            }

            newRealms.push_back(r);

        } else if (parts[0] == "F") {
            if (parts.size() != 7) {
                std::cerr << lineNo << ": faction has wrong number of data items.\n";
                continue;
            }

            Faction *f = new Faction;
            f->ident    = strToInt(parts[1]);
            f->r        = strToInt(parts[2]);
            f->g        = strToInt(parts[3]);
            f->b        = strToInt(parts[4]);
            f->home     = strToInt(parts[5]);
            f->name     = parts[6];
            newFactions.push_back(f);

        } else if (parts[0] == "S") {
            if (parts.size() != 8) {
                std::cerr << lineNo << ": species has wrong number of data items.\n";
                continue;
            }

            Species *s = new Species;
            s->ident        = strToInt(parts[1]);
            s->stance       = static_cast<Stance>(strToInt(parts[2]));
            s->wings        = static_cast<Wings>(strToInt(parts[3]));
            s->isBeastFolk  = strToInt(parts[4]);
            s->height       = strToInt(parts[5]);
            s->homeRealm    = strToInt(parts[6]);
            s->name         = parts[7];
            newSpecies.push_back(s);

        } else {
            std::cerr << lineNo << ": Unknown data type " << parts[0] << ".\n";
        }
    }

    maxX = newMaxX;
    maxY = newMaxY;
    realms = newRealms;
    factions = newFactions;
    species = newSpecies;
    return true;
}


Realm* World::getNearest(int x, int y, int notIdent, double maxDist) {
    return getNearest(x, y, std::vector<int>{notIdent}, maxDist);
}

Realm* World::getNearest(int x, int y, std::vector<int> notIdent, double maxDist) {
    Realm *nearest = nullptr;
    double nearestDist = maxDist;
    for (Realm *r : realms) {
        bool forbidden = std::find(notIdent.begin(), notIdent.end(), r->ident) != notIdent.end();
        if (forbidden) continue;

        double dist = distance(x, y, r->x, r->y);
        if (dist < nearestDist) {
            nearestDist = dist;
            nearest = r;
        }
    }
    return nearest;
}

Realm* World::getNearestNotGroup(int x, int y, int notGroup) {
    Realm *nearest = nullptr;
    double nearestDist = 99999999;
    for (Realm *r : realms) {
        double dist = distance(x, y, r->x, r->y);
        if (dist < nearestDist && r->work1 != notGroup) {
            nearestDist = dist;
            nearest = r;
        }
    }
    return nearest;
}

Realm* World::realmByIdent(int ident) {
    for (Realm *r : realms) {
        if (r->ident == ident) return r;
    }
    return nullptr;
}

Faction* World::factionByIdent(int ident) {
    for (Faction *r : factions) {
        if (r->ident == ident) return r;
    }
    return nullptr;
}

Species* World::speciesByIdent(int ident) {
    for (Species *r : species) {
        if (r->ident == ident) return r;
    }
    return nullptr;
}

std::vector<int> World::findPath(int from, int to) {
    std::vector<int> path;
    setDistances(to);
    int cur = from;
    while (cur != to) {
        path.push_back(cur);
        Realm *r = realmByIdent(cur);
        int lowest = -1;
        int lowestDist = 9999;
        for (const Link &l : r->links) {
            Realm *t = realmByIdent(l.linkTo);
            if (!t) return path;
            if (t->work1 < lowestDist) {
                lowestDist = t->work1;
                lowest = t->ident;
            }
        }
        if (lowest < 0) return path;
        cur = lowest;
    }
    path.push_back(to);
    return path;
}

int World::findDistance(int from, int to) {
    Realm *r = realmByIdent(to);
    if (!r) return -1;
    setDistances(from);
    return r->work1;
}


class realmSortByWork1 {
public:
    bool operator()(const Realm *l, const Realm *r) {
        return l->work1 > r->work1;
    }
};
void World::setDistances(int ident) {
    for (Realm *r : realms) {
        r->work1 = -1;
    }

    Realm *start = realmByIdent(ident);
    if (!start) return;
    start->work1 = 0;

    std::priority_queue<Realm*, std::vector<Realm*>, realmSortByWork1> queue;
    queue.push(start);

    while (!queue.empty()) {
        Realm *r = queue.top();
        queue.pop();
        for (const Link &l : r->links) {
            Realm *t = realmByIdent(l.linkTo);
            if (t->work1 >= 0) continue;
            t->work1 = r->work1 + 1;
            queue.push(t);
        }
    }
}


std::ostream& operator<<(std::ostream &out, const Biome &biome) {
    switch(biome) {
        case Biome::Forest:
            out << "forest";
            break;
        case Biome::Desert:
            out << "desert";
            break;
        case Biome::Tundra:
            out << "tundra";
            break;
        case Biome::Grasslands:
            out << "plains";
            break;
        case Biome::Mountain:
            out << "mountain";
            break;
        case Biome::Savanna:
            out << "savanna";
            break;
        case Biome::Jungle:
            out << "jungle";
            break;
        case Biome::None:
            out << "none";
            break;
        default:
            out << "bad biome";
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const Stance &stance) {
    switch(stance) {
        case Stance::Biped:
            out << "biped";
            break;
        case Stance::Quad:
            out << "quadruped";
            break;
        case Stance::Taur:
            out << "taur";
            break;
        case Stance::Thero:
            out << "theropod";
            break;
        default:
            out << "bad stance";
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const Wings &wing) {
    switch(wing) {
        case Wings::None:
            out << "no wings";
            break;
        case Wings::Back:
            out << "back wings";
            break;
        case Wings::Arm:
            out << "arm wings";
            break;
        default:
            out << "bad wings";
    }
    return out;
}
