#include <string>
#include <sstream>
#include <map>
#include <SDL.h>

#include "../src/realms.h"
#include "viewer.h"

static std::map<int, SDL_Texture*> maps;

void species_list(World &world, RenderInfo &r, Species *startingSpecies) {
    const int lineHeight = r.fontHeight * 1.2;
    const unsigned maxLines = r.getHeight() / lineHeight;
    const int midLine = r.getWidth() / 3;
    const int scrollX = midLine - 20;
    const unsigned lastRow = world.species.size() < maxLines ? maxLines : world.species.size() - maxLines;
    unsigned topRow = 0;


    Species *species = startingSpecies;
    if (startingSpecies) {
        for (unsigned i = 0; i < world.species.size(); ++i) {
            if (world.species[i] == startingSpecies) {
                topRow = i - maxLines / 2;
                break;
            }
        }
    }
    while (1) {
        r.clear();

        for (unsigned i = 0; i < maxLines; ++i) {
            unsigned index = i + topRow;
            if (index >= world.species.size()) continue;
            const Species *rlm = world.species[index];
            UIColour color = {255, 255, 255};
            if (rlm == species)   color.b = 0;
            else                color.b = 255;
            r.drawText(0, i * lineHeight, rlm->name + " [" + std::to_string(rlm->ident) + "]", color.r, color.g, color.b);
        }
        r.setColour(LIGHTGREY);

        const int maxScroll = r.getHeight() - 10;
        r.fillRect(scrollX, 0, 20, r.getHeight());
        double percent = topRow * 100.0 / lastRow;
        const int scrollY = percent * maxScroll / 100;
        r.setColour(DARKGREY);
        r.fillRect(scrollX, scrollY, 20, 10);

        if (species) {
            int yPos = 0;
            r.drawText(midLine + 4, yPos, species->name + " (" + species->abbrev + ") [" + std::to_string(species->ident) + "]");
            yPos += lineHeight * 2;

            std::stringstream stanceLine;
            stanceLine << "Stance: " << species->stance;
            r.drawText(midLine + 4, yPos, stanceLine.str());
            yPos += lineHeight;
            r.drawText(midLine + 4, yPos, "Avg. Height: " + intToString(species->height) + " cm");
            yPos += lineHeight;
            std::stringstream wingLine;
            wingLine << "Wings: " << 0;
            r.drawText(midLine + 4, yPos, wingLine.str());
            yPos += lineHeight * 2;
        }

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
                    if (index >= 0 && index < static_cast<int>(world.species.size())) {
                        if (species != world.species[index]) {
                            species = world.species[index];
                        }
                    }
                } else if (mx < midLine) {
                    double percent = 1.0 * my / maxScroll;
                    topRow = lastRow * percent;
                }
            }
            if (event.type == SDL_MOUSEWHEEL) {
                int shift = -event.wheel.y;
                if (shift < 0) {
                    if (static_cast<int>(topRow) > -shift) topRow += shift;
                    else topRow = 0;
                } else if (topRow + shift <= lastRow) topRow += shift;
            }
        }

    }


}

