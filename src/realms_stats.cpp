#include <algorithm>
// #include <cmath>
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

const double PI = 3.14159265358979323846;

void showSpeciesStats(World &world, const std::vector<std::string> &arguments) {
    int speciesCount = world.species.size();
    int beastFolk = 0;

    std::map<Stance, int> stances;
    std::map<Wings, int> wings;

    int minHeight = 9999;
    int maxHeight = 0;
    int totalHeight = 0;

    for (const Species *s : world.species) {
        ++stances[s->stance];
        ++wings[s->wings];
        if (s->isBeastFolk) ++beastFolk;

        if (s->height < minHeight) minHeight = s->height;
        if (s->height > maxHeight) maxHeight = s->height;
        totalHeight += s->height;
    }

    std::cout << "Biped: " << stances[Stance::Biped]  << " (" << percent(stances[Stance::Biped], speciesCount) << "%)\n";
    std::cout << "Taur:  " << stances[Stance::Taur]   << " (" << percent(stances[Stance::Taur],  speciesCount) << "%)\n";
    std::cout << "Quad:  " << stances[Stance::Quad]   << " (" << percent(stances[Stance::Quad],  speciesCount) << "%)\n";
    std::cout << "Thero: " << stances[Stance::Thero]  << " (" << percent(stances[Stance::Thero], speciesCount) << "%)\n";
    std::cout << '\n';
    std::cout << "No Wings: " << wings[Wings::None]   << " (" << percent(wings[Wings::None],     speciesCount) << "%)\n";
    std::cout << "Back Wings: " << wings[Wings::Back] << " (" << percent(wings[Wings::Back],     speciesCount) << "%)\n";
    std::cout << "Arm Wings: " << wings[Wings::Arm]   << " (" << percent(wings[Wings::Arm],      speciesCount) << "%)\n";
    std::cout << '\n';
    std::cout << "Beastfolk: " << beastFolk   << " (" << percent(beastFolk,     speciesCount) << "%)\n";
    std::cout << '\n';
    std::cout << "Max Height: " << maxHeight << " cm\n";
    std::cout << "Min Height: " << minHeight << " cm\n";
    std::cout << "Average Height: " << (totalHeight / speciesCount) << " cm\n";
    std::cout << '\n';
}

void showRealmStats(World &world, const std::vector<std::string> &arguments) {
    int realmCount = world.realms.size();

    std::map<Biome, int> biome1;
    std::map<Biome, int> biome2;
    std::map<Biome, int> biomeDual;
    int biomeCount = 0;

    int maxDiameter = 0;
    int minDiameter = 999999;
    int totalDiameter = 0;

    int maxPopDensity = 0;
    int minPopDensity = 999999;
    int totalPopDensity = 0;

    unsigned long totalPopulation = 0;

    unsigned maxLinks = 0;
    int totalLinks = 0;

    for (const Realm *r : world.realms) {
        ++biome1[r->biome[0]];
        ++biome2[r->biome[1]];
        ++biomeDual[r->biome[0]];
        if (r->biome[1] != Biome::None) {
            ++biomeDual[r->biome[1]];
            ++biomeCount;
        }

        if (r->links.size() > maxLinks) maxLinks = r->links.size();
        totalLinks += r->links.size();

        if (r->diameter > maxDiameter) maxDiameter = r->diameter;
        if (r->diameter < minDiameter) minDiameter = r->diameter;
        totalDiameter += r->diameter;

        if (r->populationDensity > maxPopDensity) maxPopDensity = r->populationDensity;
        if (r->populationDensity < minPopDensity) minPopDensity = r->populationDensity;
        totalPopDensity += r->populationDensity;

        int area = r->diameter/2 * r->diameter/2 * PI;
        totalPopulation += area * r->populationDensity;
    }

    std::cout << '\n';
    std::cout << "Biome      |  Primary  | Secondary | Combined\n";
    std::cout << "-----------+-----------+-----------+----------\n";
    for (int i = 0; i < static_cast<int>(Biome::BiomeCount); ++i) {
        Biome b = static_cast<Biome>(i);
        std::cout << std::setw(10) << std::left << b << " | " << std::right;
        std::cout << std::setw(2) << biome1[b] << "   " << std::setw(3) << percent(biome1[b], realmCount) << "% | ";
        if (b == Biome::Mountain) {
            std::cout << "          | ";
        } else {
            std::cout << std::setw(2) << biome2[b] << "   " << std::setw(3) << percent(biome2[b], realmCount) << "% | ";
        }
        std::cout << std::setw(2) << biomeDual[b] << "   " << std::setw(3) << percent(biomeDual[b], biomeCount) << "%\n";
    }

    std::cout << "\nMost Links: " << maxLinks << "\n";
    std::cout << "Average Links: " << totalLinks / realmCount << "\n";

    std::cout << "\nLargest Diameter: " << maxDiameter << " km\n";
    std::cout << "Average Diameter: " << (totalDiameter / realmCount) << " km\n";
    std::cout << "Smallest Diameter: " << minDiameter << " km\n";

    std::cout << "\nLargest Pop. Density: " << maxPopDensity << "\n";
    std::cout << "Average Pop. Density: " << (totalPopDensity / realmCount) << "\n";
    std::cout << "Smallest Pop. Density: " << minPopDensity << "\n\n";

    std::cout << "Total Realms: " << world.realms.size() << "\n";
    std::cout << "Total Population: " << totalPopulation << "\n";
}

void showFactionStats(World &world, const std::vector<std::string> &arguments) {
    int realmCount = world.realms.size();

    std::map<unsigned, int> counts;
    for (const Realm *r : world.realms) {
        ++counts[r->faction];
    }
    for (auto iter : counts) {
        if (iter.first < 0 || iter.first >= world.factions.size()) {
            std::cout << "BADFACTION  ";
        } else  {
            std::cout << std::setw(12) << world.factions[iter.first]->name;
        }
        std::cout << " [" << iter.first << "]: " << iter.second;
        std::cout << " (" << percent(iter.second, realmCount) << "%)\n";
    }

    std::cout << "\n    |";
    for (const Faction *o : world.factions) {
        if (o->ident == 0) continue;
        std::cout << ' ' << std::setw(3) << o->ident;
    }
    std::cout << "\n";
    std::cout << "----+";
    for (const Faction *o : world.factions) {
        if (o->ident == 0) continue;
        std::cout << "----";
    }
    std::cout << "\n";
    for (const Faction *o : world.factions) {
        if (o->ident == 0) continue;
        std::cout << std::setw(3) << o->ident << " |";
        for (const Faction *i : world.factions) {
            if (i->ident == 0) continue;
            if (o == i) {
                std::cout << "  --";
            } else {
                int dist = world.findDistance(o->home, i->home);
                std::cout << ' ' << std::setw(3) << dist;
            }
        }
        std::cout << "\n";
    }
    std::cout << '\n';
}

void statsDispatcher(World &world, const std::vector<std::string> &arguments) {
    if (arguments.size() == 2) {
        if (arguments[1] == "species") {
            showSpeciesStats(world, arguments);
            return;
        }

        if (arguments[1] == "realm") {
            showRealmStats(world, arguments);
            return;
        }

        if (arguments[1] == "faction") {
            showFactionStats(world, arguments);
            return;
        }
    }

    std::cout << "Must specify 'faction', 'realm', or 'species'.\n\n";
}
