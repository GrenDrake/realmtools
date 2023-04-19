#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

void makeSQL(World &world, const std::vector<std::string> &arguments) {
    std::ofstream sqlfile("realms.sql");

    sqlfile << "drop table if exists realms;\n";
    sqlfile << "drop table if exists links;\n";
    sqlfile << "drop table if exists species;\n\n";


    sqlfile << "create table realms (\n";
    sqlfile << "    ident INTEGER PRIMARY KEY,\n";
    sqlfile << "    name TEXT,\n";
    sqlfile << "    x INTEGER,\n";
    sqlfile << "    y INTEGER,\n";
    sqlfile << "    primarySpecies INTEGER,\n";
    sqlfile << "    biome TEXT,\n";
    sqlfile << "    diameter INTEGER,\n";
    sqlfile << "    popDensity INTEGER,\n";
    sqlfile << "    magicLevel INTEGER,\n";
    sqlfile << "    techLevel INTEGER,\n";

    sqlfile << "    speciesHome INTEGER,\n";
    sqlfile << "    faction INTEGER,\n";
    sqlfile << "    factionHome INTEGER\n";
    sqlfile << ");\n\n";

    sqlfile << "create table links (\n";
    sqlfile << "    fromIdent INTEGER,\n";
    sqlfile << "    toIdent INTEGER\n";
    sqlfile << ");\n\n";

    sqlfile << "create table species (\n";
    sqlfile << "    ident INTEGER PRIMARY KEY,\n";
    sqlfile << "    name TEXT,\n";
    sqlfile << "    abbrev TEXT,\n";
    sqlfile << "    stance TEXT,\n";
    sqlfile << "    wings TEXT,\n";
    sqlfile << "    heightCm INTEGER,\n";
    sqlfile << "    homeRealm INTEGER,\n";
    sqlfile << "    red INTEGER,\n";
    sqlfile << "    green INTEGER,\n";
    sqlfile << "    blue INTEGER\n";
    sqlfile << ");\n\n";


    for (const Realm *r : world.realms) {
        if (!r) continue;

        sqlfile << "INSERT INTO realms VALUES ( ";
        sqlfile << r->ident << ", ";
        sqlfile << '"' << r->name << "\", ";
        sqlfile << r->x << ", ";
        sqlfile << r->y << ", ";
        sqlfile << r->primarySpecies << ", ";
        sqlfile << '"' << r->biome << "\", ";
        sqlfile << r->diameter << ", ";
        sqlfile << r->populationDensity << ", ";
        // sqlfile << static_cast<int>(r->magicLevel) << ", ";
        // sqlfile << static_cast<int>(r->techLevel) << ", ";
        // sqlfile << r->speciesHome << ", ";
        sqlfile << r->faction << ", ";
        sqlfile << r->factionHome << ");\n";
    }
    sqlfile << '\n';

    for (const Species *s : world.species) {
        sqlfile << "INSERT INTO species VALUES ( ";
        sqlfile << s->ident << ", ";
        sqlfile << '"' << s->name << "\", ";
        sqlfile << '"' << s->abbrev << "\", ";
        sqlfile << '"' << s->stance << "\", ";
        sqlfile << '"' << s->wings << "\", ";
        sqlfile << s->height << ", ";
        // sqlfile << s->homeRealm << ", ";
        sqlfile << s->r << ", ";
        sqlfile << s->g << ", ";
        sqlfile << s->b << ");\n";

    }

    std::cout << "Wrote SQL file to realms.sql\n\n";
}
