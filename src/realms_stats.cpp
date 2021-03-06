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

void showSpeciesStats(World &world, const std::vector<std::string> &arguments) {
    int speciesCount = world.species.size();

    std::map<int, int> counts;
    std::map<Stance, int> stances;
    std::map<Wings, int> wings;

    int minHeight = 9999;
    int maxHeight = 0;
    int totalHeight = 0;

    int longestName = 0;

    for (const Realm *r : world.realms) {
        ++counts[r->primarySpecies];
    }
    for (const Species *s : world.species) {
        if (s->name.size() > longestName) longestName = s->name.size();
        ++stances[s->stance];
        ++wings[s->wings];

        if (s->height < minHeight) minHeight = s->height;
        if (s->height > maxHeight) maxHeight = s->height;
        totalHeight += s->height;
    }

    std::cout << std::left << std::setw(longestName) << "Name" << "  Count\n";
    for (int i = 0; i < longestName; ++i) std::cout << '-';
    std::cout << "-------\n";
    for (auto iter : counts) {
        std::cout << std::setw(longestName);
        const Species *s = world.speciesByIdent(iter.first);
        if (s)  std::cout << s->name;
        else    std::cout << "(bad ident)";
        std::cout << "  " << iter.second << "\n";
    }
    std::cout << "\n\n";

    std::cout << "Biped: " << stances[Stance::Biped]  << " (" << percent(stances[Stance::Biped], speciesCount) << "%)\n";
    std::cout << "Taur:  " << stances[Stance::Taur]   << " (" << percent(stances[Stance::Taur],  speciesCount) << "%)\n";
    std::cout << "Quad:  " << stances[Stance::Quad]   << " (" << percent(stances[Stance::Quad],  speciesCount) << "%)\n";
    std::cout << "Thero: " << stances[Stance::Thero]  << " (" << percent(stances[Stance::Thero], speciesCount) << "%)\n";
    std::cout << '\n';
    std::cout << "No Wings: " << wings[Wings::None]   << " (" << percent(wings[Wings::None],     speciesCount) << "%)\n";
    std::cout << "Back Wings: " << wings[Wings::Back] << " (" << percent(wings[Wings::Back],     speciesCount) << "%)\n";
    std::cout << "Arm Wings: " << wings[Wings::Arm]   << " (" << percent(wings[Wings::Arm],      speciesCount) << "%)\n";
    std::cout << '\n';
    std::cout << "Max Height: " << maxHeight << " cm\n";
    std::cout << "Min Height: " << minHeight << " cm\n";
    std::cout << "Average Height: " << (totalHeight / speciesCount) << " cm\n";
    std::cout << '\n';
}

void showRealmStats(World &world, const std::vector<std::string> &arguments) {
    int realmCount = world.realms.size();

    std::map<Biome, int> biomes;

    int maxDiameter = 0, maxDiameterId = -1;
    int minDiameter = 999999, minDiameterId = -1;
    int totalDiameter = 0;
    unsigned long long totalArea = 0;

    int maxPopDensity = 0, maxPopDensityId = -1;
    int minPopDensity = 999999, minPopDensityId = -1;
    int totalPopDensity = 0;

    int maxPopulation = 0, maxPopulationId = -1;
    int minPopulation = 999999999, minPopulationId = -1;

    unsigned long totalPopulation = 0;

    unsigned maxLinks = 0, maxLinksId = -1;
    int totalLinks = 0;

    for (const Realm *r : world.realms) {
        ++biomes[r->biome];

        if (r->links.size() > maxLinks) { maxLinks = r->links.size(); maxLinksId = r->ident; }
        totalLinks += r->links.size();

        if (r->diameter > maxDiameter) { maxDiameter = r->diameter; maxDiameterId = r->ident; }
        if (r->diameter < minDiameter) { minDiameter = r->diameter; minDiameterId = r->ident; }
        totalDiameter += r->diameter;
        totalArea += r->area();

        if (r->populationDensity > maxPopDensity) { maxPopDensity = r->populationDensity; maxPopDensityId = r->ident; }
        if (r->populationDensity < minPopDensity) { minPopDensity = r->populationDensity; minPopDensityId = r->ident; }
        totalPopulation += r->population();

        if (r->population() > maxPopulation) { maxPopulation = r->population(); maxPopulationId = r->ident; }
        if (r->population() < minPopulation) { minPopulation = r->population(); minPopulationId = r->ident; }
        totalPopDensity += r->populationDensity;
    }

    std::cout << '\n';
    std::cout << "Biome         #      %\n";
    std::cout << "-----------------------\n";
    for (int i = 0; i < static_cast<int>(Biome::BiomeCount); ++i) {
        Biome b = static_cast<Biome>(i);
        std::cout << std::setw(10) << std::left << b << "   " << std::right;
        std::cout << std::setw(2) << biomes[b] << "   " << std::setw(3) << percent(biomes[b], realmCount) << "%\n";
    }

    std::cout << "\nMost Links: " << maxLinks;
    if (maxLinksId >= 0) std::cout << " (" << world.realmByIdent(maxLinksId)->name << " [" << maxLinksId << "])";
    std::cout << '\n';
    std::cout << "Average Links: " << totalLinks / realmCount << "\n";

    std::cout << "\nLargest Diameter: " << maxDiameter << " km [area: " << intToString(calcArea(maxDiameter/2.0)) << " sq.km]";
    if (maxDiameterId >= 0) std::cout << " (" << world.realmByIdent(maxDiameterId)->name << " [" << maxDiameterId << "])";
    std::cout << '\n';
    std::cout << "Average Diameter: " << (totalDiameter / realmCount) << " km [area: " << intToString(calcArea((totalDiameter/realmCount)/2.0)) << " sq.km]\n";
    std::cout << "Smallest Diameter: " << minDiameter << " km [area: " << intToString(calcArea(minDiameter/2.0)) << " sq.km]";
    if (minDiameterId >= 0) std::cout << " (" << world.realmByIdent(minDiameterId)->name << " [" << minDiameterId << "])";
    std::cout << '\n';

    std::cout << "\nLargest Population: " << intToString(maxPopulation);
    if (maxPopulationId >= 0) std::cout << " (" << world.realmByIdent(maxPopulationId)->name << " [" << maxPopulationId << "])";
    std::cout << '\n';
    std::cout << "Average Population: " << intToString(totalPopulation / realmCount) << "\n";
    std::cout << "Smallest Population: " << intToString(minPopulation);
    if (minPopulationId >= 0) std::cout << " (" << world.realmByIdent(minPopulationId)->name << " [" << minPopulationId << "])";
    std::cout << '\n';

    std::cout << "\nLargest Pop. Density: " << maxPopDensity;
    if (maxPopDensityId >= 0) std::cout << " (" << world.realmByIdent(maxPopDensityId)->name << " [" << maxPopDensityId << "])";
    std::cout << '\n';
    std::cout << "Average Pop. Density: " << (totalPopDensity / realmCount) << "\n";
    std::cout << "Smallest Pop. Density: " << minPopDensity;
    if (minPopDensityId >= 0) std::cout << " (" << world.realmByIdent(minPopDensityId)->name << " [" << minPopDensityId << "])";
    std::cout << "\n\n";

    std::cout << "Total Realms: " << world.realms.size() << "\n";
    std::cout << "Total Area: " << intToString(totalArea) << " sq.km\n";
    std::cout << "Total Population: " << intToString(totalPopulation) << "\n";
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
