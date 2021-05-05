#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

void makeGViz(World &world, const std::vector<std::string> &arguments) {
    std::ofstream dotfile("realms.dot");
    int showWhat = 0;

    dotfile << "graph G {\noverlap=false\n\tnode [style=filled,shape=circle];\n";
    for (const Realm *r : world.realms) {
        if (!r) continue;
        for (const Link &link : r->links) {
            if (link.linkTo > r->ident) {
                dotfile << '\t' << r->ident << " -- " << link.linkTo << ";\n";
            }
        }
    }
    dotfile << '\n';

    int scale = 20;
    for (const Realm *r : world.realms) {
        dotfile << std::setfill('0');
        dotfile << '\t' << r->ident;
        dotfile << std::hex;
        dotfile << " [fillcolor=\"#";
        if (showWhat == 0) {
            const Faction *f = world.factionByIdent(r->faction);
            dotfile << std::setw(2) << f->r;
            dotfile << std::setw(2) << f->g;
            dotfile << std::setw(2) << f->b;
        } else if (showWhat == 1) {
            const Species *s = world.speciesByIdent(r->primarySpecies);
            dotfile << std::setw(2) << s->r;
            dotfile << std::setw(2) << s->g;
            dotfile << std::setw(2) << s->b;
        }
        dotfile << "\", pos=\"";
        dotfile << std::dec;
        dotfile << r->x * scale << ',' << r->y * scale;
        dotfile << "!\"";
        if (showWhat == 0) {
            if (r->factionHome) dotfile << ",shape=square";
        }
        dotfile << "];\n";
    }
    dotfile << "}\n";

    std::cout << "Wrote dot file to realms.sql\n\n";
}
