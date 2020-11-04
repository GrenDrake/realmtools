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


void findNear(World &world, const std::vector<std::string> &arguments) {
    int to = strToInt(arguments[1]);
    int dist = strToInt(arguments[2]);
    if (to < 0 || dist < 1) {
        std::cout << "Invalid argument.\n\n";
        return;
    }

    for (const Realm *r : world.realms) {
        if (r->ident == to) continue;
        int d = world.findDistance(to, r->ident);
        if (d <= dist) {
            std::cout << r->name << "\n";
        }
    }
    std::cout << '\n';
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
    const Species *s = world.speciesByIdent(r->speciesHome);
    if (s) {
        std::cout << "Home realm for species: " << s->name;
        std::cout << " [" << r->speciesHome << "]\n";
    }

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
    std::cout << "Home Realm: " << s->homeRealm << "\n";
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
    unsigned minArgs, maxArgs;
    std::string usage, description;
};
std::vector<CommandInfo> commands{
    { "svg",           makeSVG,         1, 1, "", "" },
    { "path",          findPath,        1, 1, "", "" },
    { "dist",          findDistance,    1, 1, "", "" },
    { "near",          findNear,        3, 3, "(to realm) (within distance)",
                                              "Display a list of realms within a certain distance of the one specified." },
    { "realm",         showRealm,       1, 1, "", "" },
    { "species",       showSpecies,     1, 1, "", "" },
    { "list",          listDispatcher,  2, 2, "[ factions | realms | species ]",
                                              "Displays list of all factions, realms, or species." },
    { "stats",         statsDispatcher, 2, 2, "[ factions | realms | species ]",
                                              "" },
    { "help",          showHelp,        1, 2, "[command]",
                                              "Display list of valid commands. If a command is specified, displays information on command usage instead." },
    { "checknames",    checkNames,      1, 1, "",
                                              "Check length of names does not exceed maximum." },
    { "quit",          nullptr,         1, 1, "",
                                              "Exit program." },
    { "q",             nullptr,         1, 1, "",
                                              "Exit program." },
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
