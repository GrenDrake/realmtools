#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "realms.h"

World *w = nullptr;

bool realmNameSort(const Realm *l, const Realm *r) {
    return l->name < r->name;
}
bool realmSpeciesSort(const Realm *l, const Realm *r) {
    if (l->species <= 0 && r->species <= 0) {
        return realmNameSort(l, r);
    }

    if (l->species > 0 && r->species <= 0) return false;
    if (l->species <= 0 && r->species > 0) return true;

    Species *ls = w->speciesByIdent(l->species);
    Species *rs = w->speciesByIdent(r->species);
    return ls->name < rs->name;
}
bool realmFactionSort(const Realm *l, const Realm *r) {
    if (l->faction <= 0 && r->faction <= 0) {
        return realmNameSort(l, r);
    }

    if (l->faction >= 0 && r->faction < 0) return false;
    if (l->faction < 0 && r->faction >= 0) return true;

    Faction *ls = w->factionByIdent(l->faction);
    Faction *rs = w->factionByIdent(r->faction);
    return ls->name < rs->name;
}

void listRealms(World &world, const std::vector<std::string> &arguments) {
    if (arguments.size() > 3) {
        std::cout << "Excess elements in command.\nUSAGE: list factions [sort]\n\n";
        return;
    }

    std::vector<Realm*> sorted = world.realms;
    if (arguments.size() > 2) {
        w = &world;
        if (arguments[2] == "name") std::sort(sorted.begin(), sorted.end(), realmNameSort);
        else if (arguments[2] == "species") std::sort(sorted.begin(), sorted.end(), realmSpeciesSort);
        else if (arguments[2] == "faction") std::sort(sorted.begin(), sorted.end(), realmFactionSort);
        else {
            std::cout << "Unknown sort key \"" << arguments[2] << "\".\n\n";
            return;
        }
    }

    for (const Realm *s : sorted) {
        const Faction *fac = world.factionByIdent(s->faction);
        const Species *spc = world.speciesByIdent(s->species);

        std::cout << std::left;
        std::cout << std::setw(3) << s->ident << "  ";
        std::cout << std::setw(20) << s->name << "  ";
        if (s->factionHome) std::cout << "H ";
        else                std::cout << "  ";

        std::stringstream facStr;
        if (fac) {
            facStr << fac->name << " [" << fac->ident << "]";
        } else if (s->faction == -1) {
            facStr << "unclaimed";
        } else {
            facStr << "BAD FACTION [" << s->faction << "]";
        }

        std::stringstream spcStr;
        if (spc) {
            spcStr << spc->name << " [" << spc->ident << "]";
        } else if (s->species == -1) {
            spcStr << "no native species";
        } else {
            spcStr << "BAD SPECIES [" << s->species << "]";
        }

        std::cout << std::setw(24) << facStr.str() << "  " << std::setw(24) << spcStr.str() << '\n';
    }
    std::cout << '\n';
}

bool factionNameSort(const Faction *l, const Faction *r) {
    return l->name < r->name;
}

void listFactions(World &world, const std::vector<std::string> &arguments) {
    std::map<int, int> population;
    std::map<int, int> frequency;

    if (arguments.size() > 3) {
        std::cout << "Excess elements in command.\nUSAGE: list factions [sort]\n\n";
        return;
    }

     for (const Realm *r : world.realms) {
        ++frequency[r->species];
        population[r->species] += r->population();
    }

    std::vector<Faction*> sorted = world.factions;
    if (arguments.size() > 2) {
        if (arguments[2] == "name") std::sort(sorted.begin(), sorted.end(), factionNameSort);
        else {
            std::cout << "Unknown sort key \"" << arguments[2] << "\".\n\n";
            return;
        }
    }

    for (const Faction *s : sorted) {
        const Realm *home = world.realmByIdent(s->home);

        std::cout << std::left;
        std::cout << std::setw(3) << s->ident << "  ";
        std::cout << std::setw(20) << s->name << "  ";
        if (home) {
            std::cout << home->name << " [" << home->ident << "]";
        } else if (s->home == -1) {
            std::cout << "no home realm";
        } else {
            std::cout << "BAD REALM [" << s->home << "]";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}


bool speciesNameSort(const Species *l, const Species *r) {
    return l->name < r->name;
}

void listSpecies(World &world, const std::vector<std::string> &arguments) {
    std::map<int, int> population;
    std::map<int, int> frequency;

    if (arguments.size() > 3) {
        std::cout << "Excess elements in command.\nUSAGE: list species [sort]\n\n";
        return;
    }

     for (const Realm *r : world.realms) {
        ++frequency[r->species];
        population[r->species] += r->population();
    }

    std::vector<Species*> sorted = world.species;
    if (arguments.size() > 2) {
        if (arguments[2] == "name") std::sort(sorted.begin(), sorted.end(), speciesNameSort);
        else {
            std::cout << "Unknown sort key \"" << arguments[2] << ".\n\n";
            return;
        }
    }

    for (const Species *s : sorted) {
        std::cout << std::left;
        std::cout << std::setw(3) << s->ident << "  ";
        std::cout << std::setw(24) << s->name << "  ";
        std::cout << std::setw(3) << frequency[s->ident] << " (";
        std::cout << percent(frequency[s->ident], world.realms.size()) << "%)  ";
        std::cout << std::setw(24) << intToString(population[s->ident]) << "\n";
    }
    std::cout << '\n';
}


void listDispatcher(World &world, const std::vector<std::string> &arguments) {
    if (arguments.size() >= 2) {
        if (arguments[1] == "species") {
            listSpecies(world, arguments);
            return;
        }

        if (arguments[1] == "realms") {
            listRealms(world, arguments);
            return;
        }

        if (arguments[1] == "factions") {
            listFactions(world, arguments);
            return;
        }
    }

    std::cout << "Must specify 'factions', 'realms', or 'species'.\n\n";
}
