#include <fstream>
#include <iomanip>
#include <iostream>
#include "realms.h"

struct Colour { int r; int g; int b; };
std::vector<Colour> colourList = {
    {43,206,72},    // forest
    {255,204,153},  // desert
    {240,163,255},  // tundra
    {76,0,92},      // plains
    {25,25,25},     // savanna
    {0,92,49},      // jungle
    {0,117,220},    // aquatic
    {153,63,0},     // swamp
    {128,128,128},
    {148,255,181},
    {143,124,0},
    {157,204,0},
    {194,0,136},
    {0,51,128},
    {255,164,5},
    {255,168,187},
    {66,102,0},
    {255,0,16},
    {94,241,242},
    {0,153,143},
    {224,255,102},
    {116,10,255},
    {153,0,0},
    {255,255,128},
    {255,255,0},
    {255,80,5}
};


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
    svgMap << "\t<g inkscape:label=\"Grid\" inkscape:groupmode=\"layer\" id=\"layer_grid\">\n";
    for (int x = 0; x <= maxX; x += 5) {
        int realX = (x + xOffset * 2) * scale;
        if (x > 0) {
            svgMap << "\t\t<text x=\"" << realX << "\" y=\"" << mapTop - 10;
            svgMap << "\">" << x << "</text>\n";
        }

        svgMap << "\t\t<line x1=\"" << realX << "\" x2=\"" << realX;
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
    svgMap << "\t</g>\n";

    // make link lines
    svgMap << "\t<g inkscape:label=\"Realm Links\" inkscape:groupmode=\"layer\" id=\"layer_realm_links\">\n";
    for (Realm *r : world.realms) {
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        for (const Link &l : r->links) {
            Realm *t = world.realmByIdent(l.linkTo);
            if (!t) continue;
            int targetX = (t->x + xOffset * 2) * scale;
            int targetY = (t->y + yOffset) * scale;
            svgMap << "\t\t<line x1=\"" << realX << "\" x2=\"" << targetX;
            svgMap << "\" y1=\"" << realY << "\" y2=\"" << targetY;
            svgMap << "\" stroke=\"orange\" stroke-width=\"2\"/>\n";
        }
    }
    svgMap << "\t</g>\n";

    svgMap << "\t<g inkscape:label=\"Realms\" inkscape:groupmode=\"layer\" id=\"layer_realms\">\n";
    // draw realm dots
    for (Realm *r : world.realms) {
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        svgMap << "\t\t<circle cx=\"" << realX << "\" cy=\"" << realY;
        svgMap << "\" r=\"5\" ";
        svgMap << "fill=\"#" << std::hex << std::setfill('0');
        svgMap << std::setw(2) << colourList[static_cast<int>(r->biome)].r;
        svgMap << std::setw(2) << colourList[static_cast<int>(r->biome)].g;
        svgMap << std::setw(2) << colourList[static_cast<int>(r->biome)].b;
        svgMap << "\" />\n" << std::dec;
    }
    svgMap << "\t</g>\n";

    svgMap << "\t<g inkscape:label=\"Realm Labels\" inkscape:groupmode=\"layer\" id=\"layer_realm_labels\">\n";
    // draw realm labels
    for (Realm *r : world.realms) {
        int realX = (r->x + xOffset * 2) * scale;
        int realY = (r->y + yOffset) * scale;
        svgMap << "\t\t<text x=\"" << realX << "\" y=\"" << realY - 7;
        svgMap << "\" text-anchor=\"middle\" font-size=\"smaller\">";
        svgMap << r->name << "</text>\n";
        svgMap << "\t\t<text x=\"" << realX << "\" y=\"" << realY + 7;
        svgMap << "\" text-anchor=\"middle\" dominant-baseline=\"hanging\" font-size=\"smaller\">[";
        svgMap << r->ident << "]</text>\n";
    }
    svgMap << "\t</g>\n";

    // draw biome legend
    svgMap << "\t<g inkscape:label=\"Legend (Biome)\" inkscape:groupmode=\"layer\" id=\"layer_biome\">\n";
    svgMap << "\t\t<text x=\"45\" y=\"" << 20 + yOffset * scale;
    svgMap << "\" font-size=\"smaller\" font-weight=\"bold\">";
    svgMap << "BIOMES</text>\n";

    int counter = 0;
    for (int i = 0; i < static_cast<int>(Biome::BiomeCount); ++i) {
        svgMap << "\t\t<rect x=\"25\" y=\"" << (28 + 20 * counter) + yOffset * scale;
        svgMap << "\" width=\"15\" height=\"15\" fill=\"#";
        svgMap << std::hex << std::setfill('0');
        svgMap << std::setw(2) << colourList[i].r << std::setw(2) << colourList[i].g;
        svgMap << std::setw(2) << colourList[i].b << "\"/>\n" << std::dec;

        svgMap << "\t\t<text x=\"" << 45 << "\" y=\"" << (40 + 20 * counter) + yOffset * scale;
        svgMap << "\" font-size=\"smaller\">";
        svgMap << static_cast<Biome>(i) << "</text>\n";

        ++counter;
    }
    svgMap << "\t</g>\n";


    svgMap << "</svg>\n";
    std::cout << "Wrote SVG map to realms.svg\n\n";
}
