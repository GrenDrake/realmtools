#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

void makeSVG(World &world, const std::vector<std::string> &arguments) {
    const int xOffset = 6;
    const int yOffset = 2;
    const int scale = 20;
    const int mapWidth = (world.maxX + xOffset * 5) * scale;
    const int mapHeight = (world.maxY + yOffset * 2) * scale;
    const int mapLeft = xOffset * 2 * scale;
    const int mapTop = yOffset * scale;
    const int mapRight = mapWidth - xOffset * scale * 3;
    const int mapBottom = mapHeight - yOffset * scale;

    std::ofstream svgMap("realms.svg");
    svgMap << "<svg version=\"1.1\"\n";
    svgMap << "\tbaseProfile=\"full\"\n";
    svgMap << "\twidth=\"" << mapWidth << "\" height=\"";
    svgMap << mapHeight << "\"\n";
    svgMap << "\txmlns=\"http://www.w3.org/2000/svg\">\n";

    // draw grid
    for (int x = 0; x <= world.maxX; x += 5) {
        int realX = (x + xOffset * 2) * scale;
        if (x > 0) {
            svgMap << "\t<text x=\"" << realX << "\" y=\"" << mapTop - 10;
            svgMap << "\">" << x << "</text>\n";
        }

        svgMap << "\t<line x1=\"" << realX << "\" x2=\"" << realX;
        svgMap << "\" y1=\"" << mapTop << "\" y2=\"" << mapBottom;
        svgMap << "\" stroke=\"grey";
        svgMap << "\" stroke-width=\"1\"/>\n";
    }
    for (int y = 0; y <= world.maxY; y += 5) {
        int realY = (y + yOffset) * scale;
        svgMap << "\t<text x=\"" << mapLeft - 10 << "\" y=\"" << realY;
        svgMap << "\">" << y << "</text>\n";

        svgMap << "\t<line x1=\"" << mapLeft << "\" x2=\"" << mapRight;
        svgMap << "\" y1=\"" << realY << "\" y2=\"" << realY;
        svgMap << "\" stroke=\"grey";
        svgMap << "\" stroke-width=\"1\"/>\n";
    }

    // make link lines
    for (Realm *r : world.realms) {
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        for (const Link &l : r->links) {
            Realm *t = world.realmByIdent(l.linkTo);
            if (!t) continue;
            int targetX = (t->x + xOffset * 2) * scale;
            int targetY = (t->y + yOffset) * scale;
            svgMap << "\t<line x1=\"" << realX << "\" x2=\"" << targetX;
            svgMap << "\" y1=\"" << realY << "\" y2=\"" << targetY;
            svgMap << "\" stroke=\"orange\" stroke-width=\"2\"/>\n";
        }
    }

    // draw realm dots
    for (Realm *r : world.realms) {
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        if (r->species >= 0) {
            svgMap << "\t<circle cx=\"" << realX << "\" cy=\"" << realY;
            svgMap << "\" r=\"" << (r->factionHome ? 10 : 7);
            svgMap << "\" fill=\"#FF00FF\" />\n" << std::dec;
        }
        svgMap << "\t<circle cx=\"" << realX << "\" cy=\"" << realY;
        svgMap << "\" r=\"" << (r->factionHome ? 8 : 5);
        svgMap << "\" fill=\"#" << std::hex << std::setfill('0');
        if (r->faction >= 0) {
            const Faction *c = world.factions[r->faction];
            svgMap << std::setw(2) << c->r << std::setw(2) << c->g;
            svgMap << std::setw(2) << c->b << "\" />\n" << std::dec;
        } else {
            svgMap << "FF00FF\" />\n" << std::dec;
        }
        svgMap << "\t<text x=\"" << realX << "\" y=\"" << realY - 7;
        svgMap << "\" text-anchor=\"middle\" font-size=\"smaller\">";
        svgMap << r->name << " [" << r->ident << "]</text>\n";
        if (r->species >= 0) {
            Species *species = world.speciesByIdent(r->species);
            if (species) {
                svgMap << "\t<text x=\"" << realX << "\" y=\"" << realY + 7;
                svgMap << "\" text-anchor=\"middle\" dominant-baseline=\"hanging\" font-size=\"smaller\">";
                svgMap << species->name << "[" << species->ident << "]</text>\n";
            }
        }
    }

    // draw faction legend
    svgMap << "\t<text x=\"25\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "FACTION</text>\n";

    svgMap << "\t<text x=\"170\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "SIZE</text>\n";

    int counter = 0;
    for (Faction *f : world.factions) {
        svgMap << "\t<rect x=\"5\" y=\"" << (28 + 20 * counter) + yOffset * scale;
        svgMap << "\" width=\"15\" height=\"15\" fill=\"#";
        svgMap << std::hex << std::setfill('0');
        svgMap << std::setw(2) << f->r << std::setw(2) << f->g;
        svgMap << std::setw(2) << f->b << "\"/>\n" << std::dec;

        svgMap << "\t<text x=\"25\" y=\"" << (40 + 20 * counter) + yOffset * scale;
        svgMap << "\" font-size=\"smaller\">";
        svgMap << f->name << "</text>\n";

        svgMap << "\t<text x=\"170\" y=\"" << (40 + 20 * counter) + yOffset * scale;
        svgMap << "\" font-size=\"smaller\">";
        svgMap << world.factionSize(f->ident) << "</text>\n";
        ++counter;
    }

    // draw species legend
    svgMap << "\t<text x=\"" << mapRight + 20 << "\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "SPECIES</text>\n";

    svgMap << "\t<text x=\"" << mapRight + 165 << "\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "HOME REALM</text>\n";

    counter = 0;
    for (Species *s : world.species) {
        svgMap << "\t<text x=\"" << mapRight + 20 << "\" y=\"" << (40 + 20 * counter) + yOffset * scale;
        svgMap << "\" font-size=\"smaller\">";
        svgMap << s->name << " [" << s->ident << "]</text>\n";

        Realm *r = world.realmByIdent(s->homeRealm);
        if (r) {
            svgMap << "\t<text x=\"" << mapRight + 165 << "\" y=\"" << (40 + 20 * counter) + yOffset * scale;
            svgMap << "\" font-size=\"smaller\">";
            svgMap << r->name << " [" << r->ident << "]  (" << r->x << ',' << r->y << ")</text>\n";
        }
        ++counter;
    }


    svgMap << "</svg>\n";
    std::cout << "Wrote SVG map to realms.svg\n\n";
}
