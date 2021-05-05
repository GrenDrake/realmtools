#include <cmath>
#include <string>
#include <sstream>
#include <map>
#include <SDL.h>

#include "../src/realms.h"
#include "viewer.h"

static std::map<int, SDL_Texture*> maps;

SDL_Texture *realmMap = nullptr;
UILabel *nameEdit = nullptr;
UILabel *coordEdit = nullptr;
UILabel *diameterEdit = nullptr;
UILabel *areaEdit = nullptr;
UILabel *popDensEdit = nullptr;
UILabel *populationEdit = nullptr;
UILabel *speciesEdit = nullptr;
UILabel *factionEdit = nullptr;

SDL_Texture* getMap(RenderInfo &r, int realmId) {
    auto iter = maps.find(realmId);
    if (iter == maps.end()) {
        const std::string filename = "gfx/map_" + std::to_string(realmId) + ".bmp";
        SDL_Texture *tex = r.loadTexture(filename);
        maps.insert(std::make_pair(realmId, tex));
        return tex;
    }
    return iter->second;
}

void selectRealm(World &world, RenderInfo &r, Realm *realm) {
    realmMap = getMap(r, realm->ident);
    nameEdit->setText(realm->name + " [" + std::to_string(realm->ident) + "]");
    coordEdit->setText(std::to_string(realm->x) + ", " + std::to_string(realm->y));
    diameterEdit->setText(intToString(realm->diameter));
    areaEdit->setText(intToString(realm->area()));
    popDensEdit->setText(std::to_string(realm->populationDensity));
    populationEdit->setText(intToString(realm->population()));
    Species *spc = world.speciesByIdent(realm->primarySpecies);
    Faction *fac = world.factionByIdent(realm->faction);
    if (spc) speciesEdit->setText(spc->name);
    else     speciesEdit->setText("Bad Ident #" + std::to_string(realm->primarySpecies) );
    if (spc) factionEdit->setText(fac->name);
    else     factionEdit->setText("Bad Ident #" + std::to_string(realm->faction) );
}

void realm_list(World &world, RenderInfo &r, Realm *startingRealm) {
    const int lineHeight = r.fontHeight * 1.2;
    const unsigned maxLines = r.getHeight() / lineHeight;
    const int screenWidth = r.getWidth();
    const int midLine = screenWidth / 3;
    const int column2 = midLine + 20 * r.fontWidth;
    const int scrollX = midLine - 20;
    const int mapSize = midLine;// * 0.5;
    const int mapX = midLine + (screenWidth - midLine) / 2 - mapSize / 2;
    const int mapY = r.getHeight() - 4 - mapSize;
    const unsigned lastRow = world.realms.size() - maxLines;
    unsigned topRow = 0;

    UIRoot *root = new UIRoot;
    UILabel *nameLabel = new UILabel("Name:");
    root->addChild(nameLabel, midLine + 4, 4);
    nameEdit = new UILabel("");
    root->addChild(nameEdit, midLine + 4 + r.fontWidth * 6, 4);

    UILabel *coordLabel = new UILabel("Map Coord:");
    root->addChild(coordLabel, midLine + 4, 4 + lineHeight * 2);
    coordEdit = new UILabel("");
    root->addChild(coordEdit, midLine + 4 + r.fontWidth * 11, 4 + lineHeight * 2);

    UILabel *diameterLabel = new UILabel("Avg. Diameter:");
    root->addChild(diameterLabel, midLine + 4, 4 + lineHeight * 3);
    diameterEdit = new UILabel("");
    root->addChild(diameterEdit, midLine + 4 + r.fontWidth * 15, 4 + lineHeight * 3);
    UILabel *areaLabel = new UILabel("Area:");
    root->addChild(areaLabel, column2 + 4, 4 + lineHeight * 3);
    areaEdit = new UILabel("");
    root->addChild(areaEdit, column2 + 4 + r.fontWidth * 6, 4 + lineHeight * 3);

    UILabel *popDensLabel = new UILabel("Pop. Density:");
    root->addChild(popDensLabel, midLine + 4, 4 + lineHeight * 4);
    popDensEdit = new UILabel("");
    root->addChild(popDensEdit, midLine + 4 + r.fontWidth * 14, 4 + lineHeight * 4);
    UILabel *populationLabel = new UILabel("Total Population:");
    root->addChild(populationLabel, column2 + 4, 4 + lineHeight * 4);
    populationEdit = new UILabel("");
    root->addChild(populationEdit, column2 + 4 + r.fontWidth * 18, 4 + lineHeight * 4);

    UILabel *speciesLabel = new UILabel("Species:");
    root->addChild(speciesLabel, midLine + 4, 4 + lineHeight * 6);
    speciesEdit = new UILabel("");
    root->addChild(speciesEdit, midLine + 4 + r.fontWidth * 9, 4 + lineHeight * 6);

    UILabel *factionLabel = new UILabel("Faction:");
    root->addChild(factionLabel, midLine + 4, 4 + lineHeight * 7);
    factionEdit = new UILabel("");
    root->addChild(factionEdit, midLine + 4 + r.fontWidth * 9, 4 + lineHeight * 7);

    Realm *realm = startingRealm;
    if (startingRealm) {
        for (unsigned i = 0; i < world.realms.size(); ++i) {
            if (world.realms[i] == startingRealm) {
                topRow = i - maxLines / 2;
                break;
            }
        }
    }
    if (realm) selectRealm(world, r, realm);

    while (1) {
        r.clear();

        for (unsigned i = 0; i < maxLines; ++i) {
            unsigned index = i + topRow;
            if (index >= world.realms.size()) continue;
            const Realm *rlm = world.realms[index];
            UIColour color = {255, 255, 255};
            if (rlm == realm)   color.b = 0;
            else                color.b = 255;
            r.drawText(0, i * lineHeight, rlm->name + " [" + std::to_string(rlm->ident) + "]", color.r, color.g, color.b);
        }
        r.setColour(LIGHTGREY);
        // r.drawLine(midLine, 0, midLine, r.getHeight());

        const int maxScroll = r.getHeight() - 10;
        r.fillRect(scrollX, 0, 20, r.getHeight());
        double percent = topRow * 100.0 / lastRow;
        const int scrollY = percent * maxScroll / 100;
        r.setColour(DARKGREY);
        r.fillRect(scrollX, scrollY, 20, 10);

        if (realm) {
            SDL_Rect mapDest = { mapX, mapY, mapSize, mapSize };
            SDL_RenderCopy(r.renderer, realmMap, nullptr, &mapDest);
            r.setColour(RED);
            r.drawLine(mapX + mapSize / 2, mapY, mapX + mapSize / 2, mapY + mapSize);
            r.drawLine(mapX, mapY + mapSize / 2, mapX + mapSize, mapY + mapSize / 2);
            for (const Link &l : realm->links) {
                int lx, ly;
                double radius = 200.0 * (l.distance / 100.0);
                const double PI = 3.14159265;
                double bearing = (l.bearing - 90) * PI / 180.0;
                lx = radius * cos(bearing);
                ly = radius * sin(bearing);
                r.setColour(PINK);
                r.fillRect(mapX + lx + mapSize / 2 - 1, mapY + ly + mapSize / 2 - 1, 3, 3);
                r.drawText(mapX + lx + mapSize / 2 - 1, mapY + ly + mapSize / 2 - 1, std::to_string(l.linkTo));
            }
        }

        root->repaint(r);
        r.render();


        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_HOME) topRow = 0;
                if (event.key.keysym.sym == SDLK_END) topRow = lastRow;
                if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                    if (topRow + maxLines / 3 <= lastRow) topRow += maxLines / 3;
                    else topRow = lastRow;
                }
                if (event.key.keysym.sym == SDLK_PAGEUP) {
                    if (topRow >= maxLines / 3) topRow -= maxLines / 3;
                    else if (topRow > 0) topRow = 0;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) return;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x, my = event.button.y;
                if (mx < scrollX) {
                    int index = my / lineHeight + topRow;
                    if (index >= 0 && index < static_cast<int>(world.realms.size())) {
                        if (realm != world.realms[index]) {
                            realm = world.realms[index];
                            // realmMap = getMap(r, realm->ident);
                            // nameEdit->setText(realm->name + " [" + std::to_string(realm->ident) + "]");
                            // coordEdit->setText(std::to_string(realm->x) + ", " + std::to_string(realm->y));
                            // diameterEdit->setText(intToString(realm->diameter));
                            // areaEdit->setText(intToString(realm->area()));
                            // popDensEdit->setText(std::to_string(realm->populationDensity));
                            // populationEdit->setText(intToString(realm->population()));
                            // Species *spc = world.speciesByIdent(realm->primarySpecies);
                            // Faction *fac = world.factionByIdent(realm->faction);
                            // if (spc) speciesEdit->setText(spc->name);
                            // else     speciesEdit->setText("Bad Ident #" + std::to_string(realm->primarySpecies) );
                            // if (spc) factionEdit->setText(fac->name);
                            // else     factionEdit->setText("Bad Ident #" + std::to_string(realm->faction) );
                            selectRealm(world, r, realm);
                        }
                    }
                } else if (mx < midLine) {
                    double percent = 1.0 * my / maxScroll;
                    topRow = lastRow * percent;
                }
            }

            if (event.type == SDL_MOUSEWHEEL) {
                int shift = -event.wheel.y * 2;
                if (shift < 0) {
                    if (static_cast<int>(topRow) > -shift) topRow += shift;
                    else topRow = 0;
                } else if (topRow + shift <= lastRow) topRow += shift;
            }
        }

    }


}

