#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

void makeSVG(World &world, const std::vector<std::string> &arguments) {
    const int xOffset = 6;
    const int yOffset = 2;
    const int scale = 20;
    int maxX = world.maxX;
    int maxY = world.maxY;
    while (maxX % 5 != 0) ++maxX;
    while (maxY % 5 != 0) ++maxY;
    const int mapWidth = (maxX + xOffset * 3) * scale;
    const int mapHeight = (maxY + yOffset * 2) * scale;
    const int mapLeft = xOffset * 2 * scale;
    const int mapTop = yOffset * scale;
    const int mapRight = mapWidth - xOffset * scale;
    const int mapBottom = mapHeight - yOffset * scale;

    std::ofstream svgMap("realms.svg");
    svgMap << "<svg version=\"1.1\"\n";
    svgMap << "\tbaseProfile=\"full\"\n";
    svgMap << "\twidth=\"" << mapWidth << "\" height=\"";
    svgMap << mapHeight << "\"\n";
    svgMap << "\txmlns=\"http://www.w3.org/2000/svg\">\n";

    // draw grid
    for (int x = 0; x <= maxX; x += 5) {
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
    for (int y = 0; y <= maxY; y += 5) {
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
        const Species *s = world.speciesByIdent(r->primarySpecies);
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        if (r->speciesHome) {
            svgMap << "\t<rect x=\"" << realX - 5 << "\" y=\"" << realY - 5;
            svgMap << "\" width=\"10\" height=\"10\" ";
        } else {
            svgMap << "\t<circle cx=\"" << realX << "\" cy=\"" << realY;
            svgMap << "\" r=\"5\" ";
        }
        svgMap << "fill=\"#" << std::hex << std::setfill('0');
        if (s) {
            svgMap << std::setw(2) << s->colour.r;
            svgMap << std::setw(2) << s->colour.g;
            svgMap << std::setw(2) << s->colour.b;
            svgMap << "\" />\n" << std::dec;
        } else {
            svgMap << "777777\" />\n" << std::dec;
        }
        svgMap << "\t<text x=\"" << realX << "\" y=\"" << realY - 7;
        svgMap << "\" text-anchor=\"middle\" font-size=\"smaller\">";
        svgMap << r->name << " [" << r->ident << "]</text>\n";
        if (s) {
            svgMap << "\t<text x=\"" << realX << "\" y=\"" << realY + 7;
            svgMap << "\" text-anchor=\"middle\" dominant-baseline=\"hanging\" font-size=\"smaller\">";
            svgMap << s->abbrev << " [" << s->ident << "]</text>\n";
        }
    }

    // draw species legend
    svgMap << "\t<text x=\"45\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "SPECIES</text>\n";

    int counter = 0;
    for (Species *s : world.species) {
        svgMap << "\t<rect x=\"25\" y=\"" << (28 + 20 * counter) + yOffset * scale;
        svgMap << "\" width=\"15\" height=\"15\" fill=\"#";
        svgMap << std::hex << std::setfill('0');
        svgMap << std::setw(2) << s->colour.r << std::setw(2) << s->colour.g;
        svgMap << std::setw(2) << s->colour.b << "\"/>\n" << std::dec;

        svgMap << "\t<text x=\"" << 45 << "\" y=\"" << (40 + 20 * counter) + yOffset * scale;
        svgMap << "\" font-size=\"smaller\">";
        svgMap << s->name << " [" << s->ident << "]</text>\n";

        ++counter;
    }


    svgMap << "</svg>\n";
    std::cout << "Wrote SVG map to realms.svg\n\n";
}
