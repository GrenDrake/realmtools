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


void makeSVG(World &world, const std::vector<std::string> &arguments);
void statsDispatcher(World &world, const std::vector<std::string> &arguments);
void listDispatcher(World &world, const std::vector<std::string> &arguments);

void findPath(World &world, const std::vector<std::string> &arguments) {
    std::string fromStr, toStr;
    std::cout << "from> ";
    std::getline(std::cin, fromStr);
    if (fromStr.empty()) return;
    std::cout << "to> ";
    std::getline(std::cin, toStr);
    if (toStr.empty()) return;
    int from = strToInt(fromStr);
    int to = strToInt(toStr);
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
    std::string fromStr, toStr;
    std::cout << "from> ";
    std::getline(std::cin, fromStr);
    if (fromStr.empty()) return;
    std::cout << "to> ";
    std::getline(std::cin, toStr);
    if (toStr.empty()) return;
    int from = strToInt(fromStr);
    int to = strToInt(toStr);
    if (from < 0 || to < 0) return;

    std::cout << "Distance from " << from << " to " << to << ": ";
    std::cout << world.findDistance(from, to) << "\n\n";
}

void showRealm(World &world, const std::vector<std::string> &arguments) {
    std::string fromStr;
    std::cout << "realm#> ";
    std::getline(std::cin, fromStr);
    if (fromStr.empty()) return;
    int from = strToInt(fromStr);
    if (from < 0) return;

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
    std::cout << "Biome: " << r->biome[0];
    if (r->biome[1] != Biome::None) std::cout << " / " << r->biome[1];
    std::cout << "\n";
    std::cout << "Native species: ";
    const Species *s = world.speciesByIdent(r->species);
    if (s)  std::cout << s->name;
    else    std::cout << "BAD_SPECIES";
    std::cout << " [" << r->species << "]\n";

    std::cout << "\n";
}

void showSpecies(World &world, const std::vector<std::string> &arguments) {
    std::string fromStr;
    std::cout << "species#> ";
    std::getline(std::cin, fromStr);
    if (fromStr.empty()) return;
    int from = strToInt(fromStr);
    Species *s = nullptr;
    if (from >= 0) {
        s = world.speciesByIdent(from);
    }
    if (!s) {
        std::cout << "Invalid species " << fromStr << ".\n\n";
        return;
    }

    std::cout << s->name << " [" << s->ident << "]\n";
    std::cout << "Height: " << s->height << " cm\n";
    std::cout << "Stance: " << s->stance << "\n";
    std::cout << "Wings: " << s->wings << "\n";
    if (s->isBeastFolk) std::cout << "Beastfolk: Yes\n";
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
    std::cout << '\n' << std::right;
}


typedef void (*cmdHandler)(World&, const std::vector<std::string>&);
struct CommandInfo {
    std::string name;
    cmdHandler func;
};
std::vector<CommandInfo> commands{
    { "svg",           makeSVG },
    { "path",          findPath },
    { "dist",          findDistance },
    { "realm",         showRealm },
    { "species",       showSpecies },
    { "list",          listDispatcher },
    { "stats",         statsDispatcher },
    { "checknames",    checkNames },
    { "quit",          nullptr },
    { "q",             nullptr },
};

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
        if (cmd->func)  cmd->func(world, parts);
        else            return true;
    } else {
        std::cout << "Unknown command \"" << trim(commandText) << "\"\n\n";
    }
    return false;
}

int main(int argc, char *argv[]) {
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
