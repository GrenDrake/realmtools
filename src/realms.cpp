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

void showHelp(World &world, const std::vector<std::string> &arguments);

void makeSQL(World &world, const std::vector<std::string> &arguments);
void makeSVG(World &world, const std::vector<std::string> &arguments);
void statsDispatcher(World &world, const std::vector<std::string> &arguments);
void listDispatcher(World &world, const std::vector<std::string> &arguments);

void findPath(World &world, const std::vector<std::string> &arguments) {
    int from = strToInt(arguments[1]);
    int to = strToInt(arguments[2]);
    if (from < 0 || to < 0) return;

    std::cout << "Path from " << from << " to " << to << ": ";
    auto path = world.findPath(from, to);
    bool first = true;
    for (int i : path) {
        if (first) first = false;
        else        std::cout << " -> ";
        std::cout << i;
    }
    std::cout << "\nDistance: " << path.size() << "\n\n";
}

void findDistance(World &world, const std::vector<std::string> &arguments) {
    int from = strToInt(arguments[1]);
    int to = strToInt(arguments[2]);
    if (from < 0 || to < 0) return;

    std::cout << "Distance from " << from << " to " << to << ": ";
    std::cout << world.findDistance(from, to) << "\n\n";
}

bool realmNearSort(const Realm *l, const Realm *r) {
    if (l->work2 < r->work2) return true;
    if (l->work2 > r->work2) return false;
    return l->name < r->name;
}
void findNear(World &world, const std::vector<std::string> &arguments) {
    int to = strToInt(arguments[1]);
    int dist = strToInt(arguments[2]);
    if (to < 0 || dist < 1) {
        std::cout << "Invalid argument.\n\n";
        return;
    }

    std::vector<Realm*> work;
    for (Realm *r : world.realms) {
        if (r->ident == to) continue;
        int d = world.findDistance(to, r->ident);
        if (d <= dist) {
            r->work2 = d;
            work.push_back(r);
        }
    }
    std::sort(work.begin(), work.end(), realmNearSort);

    std::cout << "     REALM                   DIST  HR  PRIMARY SPECIES\n";
    for (const Realm *r : work) {
        Species *s = world.speciesByIdent(r->primarySpecies);
        std::cout << std::setw(3) << r->ident << ": ";
        std::cout << std::left << std::setw(MAX_NAME_LENGTH) << r->name << std::right << "    ";
        std::cout << std::setw(4) << r->work2 << "  ";
        if (r->speciesHome) std::cout << 'X';
        else                std::cout << ' ';
        std::cout << "   " << s->name << " [" << s->ident << "]\n";
    }
    std::cout << '\n';
}

void findNearXY(World &world, const std::vector<std::string> &arguments) {
    int X = strToInt(arguments[1]);
    int Y = strToInt(arguments[2]);
    int count = 1;
    if (arguments.size() > 3) count = strToInt(arguments[3]);
    if (X < 0 || Y < 1 || count < 1) {
        std::cout << "Invalid argument.\n\n";
        return;
    }

    std::vector<Realm*> work;
    for (Realm *r : world.realms) {
        r->work2 = distance(r->x, r->y, X, Y) * 1000;
        work.push_back(r);
    }
    std::sort(work.begin(), work.end(), realmNearSort);

    std::cout << "     REALM                   DIST  X   Y   HR  PRIMARY SPECIES\n";
    for (unsigned i = 0; i < work.size() && i < count; ++i) {
        const Realm *r = work[i];
        Species *s = world.speciesByIdent(r->primarySpecies);
        std::cout << std::setw(3) << r->ident << ": ";
        std::cout << std::left << std::setw(MAX_NAME_LENGTH) << r->name << std::right;
        std::cout << std::setw(8) << r->work2 / 1000.0 << "  ";
        std::cout << std::left << std::setw(4) << r->x;
        std::cout << std::setw(4) << r->y << std::right;
        if (r->speciesHome) std::cout << 'X';
        else                std::cout << ' ';
        std::cout << "   " << s->name << " [" << s->ident << "]\n";
    }
    std::cout << '\n';
}

void randomRealm(World &world, const std::vector<std::string> &arguments) {
    int count = 1;
    if (arguments.size() > 1) count = strToInt(arguments[1]);
    if (count < 1) {
        std::cout << "Invalid argument.\n\n";
        return;
    }

    if (count > world.realms.size()) {
        std::cout << "Selection count cannot be greater than realm count. (Asked for ";
        std::cout << count << " realms, but only " << world.realms.size() << " exist.)\n\n";
        return;
    }

    std::vector<const Realm*> work;
    while (work.size() < count) {
        int rng = rngNext(world.realms.size());
        Realm *s = world.realms[rng];
        bool isDup = false;
        for (const Realm *r : work) {
            if (r == s) isDup = true;
            break;
        }
        if (!isDup) work.push_back(s);
    }

    std::cout << "     REALM                   HR  PRIMARY SPECIES\n";
    for (const Realm *r : work) {
        Species *s = world.speciesByIdent(r->primarySpecies);
        std::cout << std::setw(3) << r->ident << ": ";
        std::cout << std::left << std::setw(MAX_NAME_LENGTH) << r->name << std::right << "    ";
        if (r->speciesHome) std::cout << 'X';
        else                std::cout << ' ';
        std::cout << "   " << s->name << " [" << s->ident << "]\n";
    }
    std::cout << '\n';
}

void showRealm(World &world, const std::vector<std::string> &arguments) {
    int from = strToInt(arguments[1]);
    if (from < 0) {
        std::cout << "Invalid argument.\n\n";
        return;
    }

    Realm *r = world.realmByIdent(from);
    if (!r) {
        std::cout << "Invalid ident " << from << ".\n\n";
        return;
    }

    std::cout << r->name << " [" << r->ident << "]\n";
    std::cout << "Links:";
    for (const Link &l : r->links) {
        std::cout << " <" << l.linkTo << '>';
    }
    std::cout << "\nDiameter: " << r->diameter << " mi.\n";
    std::cout << "Area: " << intToString(r->area()) << " sq mi.\n";
    std::cout << "Pop. Density: " << r->populationDensity << " per sq mi.\n";
    std::cout << "Population: " << intToString(r->population()) << "\n";
    std::cout << "Biome: " << r->biome << "\n";
    std::cout << "Tech Level: " << r->techLevel << "\n";
    std::cout << "Magic Level: " << r->magicLevel << "\n";
    const Species *s = world.speciesByIdent(r->primarySpecies);
    if (s) {
        std::cout << "Primary species: " << s->name;
        std::cout << " [" << r->primarySpecies << "]";
        if (r->speciesHome) std::cout << " (home realm)";
        std::cout << '\n';
    }

    std::cout << "\n";
}

void showSpecies(World &world, const std::vector<std::string> &arguments) {
    int from = strToInt(arguments[1]);
    Species *s = nullptr;
    if (from >= 0) {
        s = world.speciesByIdent(from);
    }
    if (!s) {
        std::cout << "Invalid species " << arguments[1] << ".\n\n";
        return;
    }

    std::cout << s->name << " [" << s->ident << "]\n";
    std::cout << "Home Realm: " << s->homeRealm << "\n";
    std::cout << "Height: " << s->height << " cm\n";
    std::cout << "Stance: " << s->stance << "\n";
    std::cout << "Wings: " << s->wings << "\n";
    std::cout << '\n';
}


void checkNames(World &world, const std::vector<std::string> &arguments) {
    std::cout << "Factions:\n" << std::left;
    for (Faction *r : world.factions) {
        if (r->name.size() > MAX_NAME_LENGTH) {
            std::cout << '\t' << std::setw(3) << r->ident << "  " << r->name << '\n';
        }
    }
    std::cout << '\n' << std::right;

    std::cout << "Realms:\n" << std::left;
    for (Realm *r : world.realms) {
        if (r->name.size() > MAX_NAME_LENGTH) {
            std::cout << '\t' << std::setw(3) << r->ident << "  " << r->name << '\n';
        }
    }
    std::cout << '\n' << std::right;

    std::cout << "Species:\n" << std::left;
    for (Species *r : world.species) {
        if (r->name.size() > MAX_NAME_LENGTH) {
            std::cout << '\t' << std::setw(3) << r->ident << "  " << r->name << '\n';
        }
    }

    // check species abbreviations are unique
    std::cout << "\nSpecies Abbreviations:\n" << std::left;
    for (Species *a : world.species) {
        for (Species *b : world.species) {
            // if (a == b) continue;
            if (a->ident >= b->ident) continue;
            if (a->abbrev == b->abbrev) {
                std::cout << '\t';
                std::cout << std::setw(MAX_NAME_LENGTH) << a->name << " <> ";
                std::cout << std::setw(MAX_NAME_LENGTH) << b->name << "\n";
            }
        }
    }
    std::cout << '\n' << std::right;
}


typedef void (*cmdHandler)(World&, const std::vector<std::string>&);
struct CommandInfo {
    std::string name;
    cmdHandler func;
    unsigned minArgs, maxArgs;
    std::string usage, description;
};
std::vector<CommandInfo> commands{
    { "checknames",    checkNames,      1, 1, "",
                                              "Check length of names does not exceed maximum." },
    { "dist",          findDistance,    3, 3, "(from) (to)",
                                              "Finds the minimum number of transits required to travel between two realms." },
    { "help",          showHelp,        1, 2, "[command]",
                                              "Display list of valid commands. If a command is specified, displays information on command usage instead." },
    { "list",          listDispatcher,  2, 3, "(factions|realms|species) [sort by]",
                                              "Displays list of all factions, realms, or species." },
    { "near",          findNear,        3, 3, "(to realm) (within distance)",
                                              "Display a list of realms within a certain distance of the one specified." },
    { "nearxy",        findNearXY,      3, 4, "(x) (y) [count]",
                                              "Display up to count realms closest to the provided visual XY coordinates. If not specified, count is 1." },
    { "path",          findPath,        3, 3, "(from) (to)",
                                              "Finds the shortest path between two realms." },
    { "quit",          nullptr,         1, 1, "",
                                              "Exit program." },
    { "q",             nullptr,         1, 1, "",
                                              "Exit program." },
    { "random",        randomRealm,     1, 2, "[count]",
                                              "Select one or more random realms. If unspecified, count is 1." },
    { "realm",         showRealm,       2, 2, "(realm id)",
                                              "Displays realm information." },
    { "species",       showSpecies,     2, 2, "(species id)",
                                              "Displays species information" },
    { "sql",           makeSQL,         1, 1, "",
                                              "Creates SQL file with realms data." },
    { "svg",           makeSVG,         1, 1, "",
                                              "Outputs map of all realm connects as an SVG file." },
    { "stats",         statsDispatcher, 2, 2, "(faction|realm|species)",
                                              "Calculate and display stats for one of factions, realms, or species." },
};

void showHelp(World &world, const std::vector<std::string> &arguments) {
    if (arguments.size() > 1) {
        for (const CommandInfo &cmd : commands) {
            if (cmd.name == arguments[1]) {
                std::cout << cmd.name << ' ' << cmd.usage << "\n\n";
                std::cout << cmd.description << "\n\n";
                return;
            }
        }
        std::cout << "Unknown command \"" << arguments[1] << "\".\n";
        return;
    }

    std::cout << "Valid commands:\n" << std::left;
    for (const CommandInfo &cmd : commands) {
        std::cout << "  " << std::setw(12) << cmd.name << "  " << cmd.usage << "\n";
    }
}

bool processCommand(World &world, std::string commandText) {
    auto parts = explodeOnWhitespace(commandText);
    if (parts.empty()) return false;

    const CommandInfo *cmd = nullptr;
    for (const CommandInfo &c : commands) {
        if (c.name == parts[0]) {
            cmd = &c;
            break;
        }
    }

    if (cmd) {
        if (cmd->func)  {
            if (parts.size() < cmd->minArgs || parts.size() > cmd->maxArgs) {
                std::cout << "Invalid argument count. Try \"help " << cmd->name << "\" for usage.\n\n";
            } else cmd->func(world, parts);
        }
        else            return true;
    } else {
        std::cout << "Unknown command \"" << trim(commandText) << "\"\n\n";
    }
    return false;
}

int main(int argc, char *argv[]) {
    rngInit(0);
    World world;
    if (!world.readFromFile("realms.txt")) {
        std::cerr << "Failed to read realms data.\n";
        return 1;
    }
    std::cout << "Read " << world.realms.size() << " realms.\n";
    std::cout << "Read " << world.factions.size() << " factions.\n";
    std::cout << "Read " << world.species.size() << " species.\n\n";

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::cout << "> " << argv[i] << "\n";
            if (processCommand(world, argv[i])) break;
        }
        return 0;
    }

    while (1) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);
        if (processCommand(world, input)) break;
    }

    return 0;
}
