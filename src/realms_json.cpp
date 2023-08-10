#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

void makeJSON(World &world, const std::vector<std::string> &arguments) {
    std::ofstream jsonFile("realms.js");

    jsonFile << "const realmsDB = {\n\t\"realms\": [\n";
    for (const Realm *r : world.realms) {
        if (!r) continue;
    
        jsonFile << "\t\t{\n";
        jsonFile << "\t\t\t\"ident\": " << r->ident << ",\n";
        jsonFile << "\t\t\t\"name\": \"" << r->name << "\",\n";
        jsonFile << "\t\t\t\"x\": " << r->x << ",\n";
        jsonFile << "\t\t\t\"y\": " << r->y << ",\n";
        jsonFile << "\t\t\t\"diameter\": " << r->diameter << ",\n";
        jsonFile << "\t\t\t\"populationDensity\": " << r->populationDensity << ",\n";
        jsonFile << "\t\t\t\"biome\": \"" << r->biome << "\",\n";
        jsonFile << "\t\t\t\"faction\": " << r->faction << ",\n";
        jsonFile << "\t\t\t\"factionHome\": " << (r->factionHome ? "true" : "false") << ",\n";
        jsonFile << "\t\t\t\"diameter\": " << r->diameter << ",\n";
        jsonFile << "\t\t\t\"primarySpecies\": " << r->primarySpecies << ",\n";
        jsonFile << "\t\t\t\"links\": [ ";
        for (const Link &l : r->links) {
            jsonFile << l.linkTo << ", ";
        }
        jsonFile << "],\n";
        jsonFile << "\t\t},\n";
        
        

    // for (const Realm *r : world.realms) {
        // if (!r) continue;
    // }

        //// jsonFile << static_cast<int>(r->magicLevel) << ", ";
        //// jsonFile << static_cast<int>(r->techLevel) << ", ";
    }
    jsonFile << "\t],\n";

    jsonFile << "\t\"factions\": [\n";
    for (const Faction *f : world.factions) {
        jsonFile << "\t\t{\n";
        jsonFile << "\t\t\t\"ident\": " << f->ident << ",\n";
        jsonFile << "\t\t\t\"name\": \"" << f->name << "\",\n";
        jsonFile << "\t\t\t\"color\": \"0x";
        jsonFile << std::hex << std::setfill('0');
        jsonFile << std::setw(2) << f->r;
        jsonFile << std::setw(2) << f->g;
        jsonFile << std::setw(2) << f->b;
        jsonFile << std::dec << "\",\n";;
        jsonFile << "\t\t\t\"homeRealm\": " << f->home << ",\n";
    }
    jsonFile << "\t],\n";

    jsonFile << "\t\"species\": [\n";
    for (const Species *s : world.species) {
        jsonFile << "\t\t{\n";
        jsonFile << "\t\t\t\"ident\": " << s->ident << ",\n";
        jsonFile << "\t\t\t\"name\": \"" << s->name << "\",\n";
        jsonFile << "\t\t\t\"abbrev\": \"" << s->abbrev << "\",\n";
        jsonFile << "\t\t\t\"color\": \"0x";
        jsonFile << std::hex << std::setfill('0');
        jsonFile << std::setw(2) << s->r;
        jsonFile << std::setw(2) << s->g;
        jsonFile << std::setw(2) << s->b;
        jsonFile << std::dec << "\",\n";
    }
    jsonFile << "\t],\n";
    jsonFile << "}\n";

    std::cout << "Wrote JSON file to realms.js\n\n";
}
