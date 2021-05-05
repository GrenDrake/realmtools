#include <sstream>
#include <string>
#include <iostream>

#include <SDL.h>

#include "../src/realms.h"
#include "viewer.h"

void innerMain(RenderInfo &r);
void realm_list(World &world, RenderInfo &r, Realm *startingRealm);
void species_list(World &world, RenderInfo &r, Species *startingSpecies);

int main(int argc, char *argv[]) {
    bool wantFullscreen = false;
    bool wantMaximized = false;
    bool wantVsync = true;

    for (int i = 1; i < argc; ++i) {
        const std::string &arg = argv[i];
        if (arg == "-fullscreen")           wantFullscreen = true;
        else if (arg == "-no-fullscreen")   wantFullscreen = true;
        else if (arg == "-maximized")       wantMaximized = true;
        else if (arg == "-no-maximized")    wantMaximized = true;
        else if (arg == "-vsync")           wantVsync = true;
        else if (arg == "-no-vsync")        wantVsync = true;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
        return 1;
    }

    const int displayNum = 0;
    int defaultScreenWidth = 1920;
    int defaultScreenHeight = 1024;

    SDL_Rect bounds;
    SDL_GetDisplayUsableBounds(0, &bounds);
    if (bounds.w < defaultScreenWidth)  defaultScreenWidth = bounds.w * 0.9;
    if (bounds.h < defaultScreenHeight) defaultScreenHeight = bounds.h * 0.9;
    unsigned windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (wantFullscreen) windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (wantMaximized)  windowFlags |= SDL_WINDOW_MAXIMIZED;
    SDL_Window *win = SDL_CreateWindow("Realms Viewer",
                                       SDL_WINDOWPOS_CENTERED_DISPLAY(displayNum),
                                       SDL_WINDOWPOS_CENTERED_DISPLAY(displayNum),
                                       defaultScreenWidth, defaultScreenHeight,
                                       windowFlags);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    unsigned rendererFlags = SDL_RENDERER_ACCELERATED;
    if (wantVsync) {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, rendererFlags);
    if (renderer == nullptr){
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    SDL_RendererInfo renderInfo;
    if (SDL_GetRendererInfo(renderer, &renderInfo) != 0) {
        std::cerr << "SDL_GetRenderInfo Error: " << SDL_GetError() << '\n';
    } else {
        std::cerr << "Renderer is " << renderInfo.name << ".\n";
    }

    RenderInfo rInfo;
    rInfo.window = win;
    rInfo.renderer = renderer;
    rInfo.fontWidth = 9;
    rInfo.fontHeight = 18;
    rInfo.font = rInfo.loadTexture("gfx/font.bmp");

    innerMain(rInfo);

    SDL_DestroyTexture(rInfo.font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

enum class Mode { Faction, Species, Realm };
enum class Task { None, Move };

void buildRealmList(UIList *list, World &world) {
    list->items.clear();
    for (unsigned i = 0; i < world.realms.size(); ++i) {
        const Realm *rlm = world.realms[i];
        std::stringstream line;
        line << rlm->name << " [" + std::to_string(rlm->ident) << "]";
        list->items.push_back(ListRow{line.str(), rlm->ident});
    }
}
void buildSpeciesList(UIList *list, World &world) {
    list->items.clear();
    for (unsigned i = 0; i < world.species.size(); ++i) {
        const Species *spc = world.species[i];
        std::stringstream line;
        line << "\x1" << static_cast<char>(spc->r) << static_cast<char>(spc->g) << static_cast<char>(spc->b);
        line << ' ' << spc->name << " [" + std::to_string(spc->ident) << "]";
        list->items.push_back(ListRow{line.str(), spc->ident});
    }
}
void buildFactionList(UIList *list, World &world) {
    list->items.clear();
    for (unsigned i = 0; i < world.factions.size(); ++i) {
        const Faction *fac = world.factions[i];
        std::stringstream line;
        line << "\x1" << static_cast<char>(fac->r) << static_cast<char>(fac->g) << static_cast<char>(fac->b);
        line << ' ' << fac->name << " [" << std::to_string(fac->ident) << "]";
        list->items.push_back(ListRow{line.str(), fac->ident});
    }
}

void innerMain(RenderInfo &r) {
    World world;
    if (!world.readFromFile("realms.txt")) {
        std::cerr << "Failed to read realms data.\n";
        return;
    }

    const std::string versionString = "Realms Viewer Alpha1";
    std::stringstream defaultMessage;
    defaultMessage << "Data loaded. " << world.realms.size() << " realms; ";
    defaultMessage << world.factions.size() << " factions; ";
    defaultMessage << world.species.size() << " species";

    const int lineHeight = r.fontHeight + 2;
    const int screenWidth = r.getWidth();
    const int screenHeight = r.getHeight();
    const int infoHeight = lineHeight * 3 + 4;
    const int infoTop = screenHeight - infoHeight;
    const int mapAreaHeight = screenHeight - infoHeight;
    const int listWidth = 300;
    const int listLeft = screenWidth - listWidth;

    int scaleX = (listLeft - 20) / world.maxX;
    int scaleY = (mapAreaHeight - 20) / world.maxY;
    int scale = scaleX;
    if (scaleY < scaleX) scale = scaleY;
    int x = 10, y = 10;
    // std::string statusMessage = defaultMessage.str();
    Mode colourMode = Mode::Faction;
    Task task = Task::None;
    Realm *taskRealm;

    UIRoot *root = new UIRoot;
    UIPanel *panel = new UIPanel;
    root->addChild(panel, 0, infoTop);
    panel->resize(screenWidth, infoHeight);

    UILabel *info1 = new UILabel("");
    panel->addChild(info1, 4, 4);
    UILabel *info2 = new UILabel("");
    panel->addChild(info2, 4, 4 + lineHeight);
    UILabel *info3 = new UILabel("");
    panel->addChild(info3, 600, 4);
    UILabel *info4 = new UILabel("");
    panel->addChild(info4, 600, 4 + lineHeight);

    UILabel *statusMessage = new UILabel(defaultMessage.str());
    panel->addChild(statusMessage, 4, 4 + lineHeight * 2);
    UILabel *versionLabel = new UILabel(versionString);
    panel->addChild(versionLabel, screenWidth - 4, 4 + lineHeight * 2);
    versionLabel->setAlign(UILabel::RIGHT);

    panel = new UIPanel;
    root->addChild(panel, listLeft, 0);
    panel->resize(listWidth, screenHeight - infoHeight);

    UIList *realmList = new UIList;
    panel->addChild(realmList, 4, 4);
    realmList->resize(listWidth - 8, screenHeight - infoHeight - 8);
    buildRealmList(realmList, world);

    UIList *speciesList = new UIList;
    panel->addChild(speciesList, 4, 4);
    speciesList->resize(listWidth - 8, screenHeight - infoHeight - 8);
    speciesList->setShow(false);
    buildSpeciesList(speciesList, world);

    UIList *factionList = new UIList;
    panel->addChild(factionList, 4, 4);
    factionList->resize(listWidth - 8, screenHeight - infoHeight - 8);
    factionList->setShow(false);
    buildFactionList(factionList, world);

    while (1) {
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);
        int mapX = (mx - x) / scale;
        int mapY = (my - y) / scale;
        Realm *hoverRealm = nullptr;
        if (mx <= listLeft && my <= infoTop) {
            hoverRealm = world.getNearest(mapX, mapY, std::vector<int>{ }, 1);
        }

        if (hoverRealm) {
            std::stringstream l;
            l << hoverRealm->name << " [" << hoverRealm->ident << ']';
            info1->setText(l.str());

            std::stringstream l2;
            Species *s = world.speciesByIdent(hoverRealm->primarySpecies);
            Faction *f = world.factionByIdent(hoverRealm->faction);
            l2 << "Species: ";
            if (s) l2 << s->name << " [" << s->ident << ']';
            else    l2 << "none";
            l2 << "   Faction: ";
            if (f)  l2 << f->name << " [" << f->ident << ']';
            else    l2 << "none";
            info2->setText(l2.str());
        } else {
            info1->setText("");
            info2->setText("");
        }
        if (realmList->isShown()) {
            if (realmList->selection != NO_SELECTION) {
                Realm *selRealm = world.realmByIdent(realmList->items[realmList->selection].ident);
                std::stringstream l;
                l << selRealm->name << " [" << selRealm->ident << ']';
                info3->setText(l.str());

                std::stringstream l2;
                Species *s = world.speciesByIdent(selRealm->primarySpecies);
                Faction *f = world.factionByIdent(selRealm->faction);
                l2 << "Species: ";
                if (s) l2 << s->name << " [" << s->ident << ']';
                else    l2 << "none";
                l2 << "   Faction: ";
                if (f)  l2 << f->name << " [" << f->ident << ']';
                else    l2 << "none";
                info4->setText(l2.str());
            } else {
                info3->setText("");
                info4->setText("");
            }
        } else if (speciesList->isShown()) {
            if (speciesList->selection != NO_SELECTION) {
                Species *selSpecies = world.speciesByIdent(speciesList->items[speciesList->selection].ident);
                std::stringstream l;
                l << selSpecies->name << " [" << selSpecies->ident << ']';
                info3->setText(l.str());

                std::stringstream l2;
                l2 << "stance:" << selSpecies->stance << "  " << selSpecies->wings << "  height:" << selSpecies->height << " cm";
                info4->setText(l2.str());
            } else {
                info3->setText("");
                info4->setText("");
            }
        } else if (factionList->isShown()) {
            if (factionList->selection != NO_SELECTION) {
                Faction *selFaction = world.factionByIdent(factionList->items[factionList->selection].ident);
                std::stringstream l;
                l << selFaction->name << " [" << selFaction->ident << ']';
                info3->setText(l.str());
                info4->setText("");
            } else {
                info3->setText("");
                info4->setText("");
            }
        }

        r.clear();
        // DRAW CONNECTIONS
        r.setColour(WHITE);
        for (const Realm *realm : world.realms) {
            for (const Link &l : realm->links) {
                if (l.linkTo <= realm->ident) continue;
                const Realm *target = world.realmByIdent(l.linkTo);
                int rx = x + realm->x * scale + scale / 2;
                int ry = y + realm->y * scale + scale / 2;
                int tx = x + target->x * scale + scale / 2;
                int ty = y + target->y * scale + scale / 2;
                r.drawLine(rx, ry, tx, ty);
            }
        }

        // DRAW MAP
        for (const Realm *realm : world.realms) {
            int rx = x + realm->x * scale;
            int ry = y + realm->y * scale;
            r.setColour(INVALID);
            if (colourMode == Mode::Faction) {
                Faction *f = world.factionByIdent(realm->faction);
                if (f)  r.setColour(UIColour(f->r, f->g, f->b));
            } else if (colourMode == Mode::Species) {
                Species *s = world.speciesByIdent(realm->primarySpecies);
                if (s)  r.setColour(UIColour(s->r, s->g, s->b));
            }
            r.fillRect(rx, ry, scale, scale);
            if (realm == hoverRealm) {
                r.setColour(HIGHLIGHT);
            } else if (realmList->selection != NO_SELECTION && realmList->getSelection().ident == realm->ident) {
                r.setColour(RED);
            } else if (speciesList->selection != NO_SELECTION && speciesList->getSelection().ident == realm->primarySpecies) {
                r.setColour(BLUE);
            } else if (factionList->selection != NO_SELECTION && factionList->getSelection().ident == realm->faction) {
                r.setColour(GREEN);
            } else {
                r.setColour(MIDGREY);
            }
            r.drawRect(rx-1, ry-1, scale+2, scale+2);
            r.drawRect(rx, ry, scale, scale);
        }

        root->repaint(r);
        r.render();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                statusMessage->setText("");
                if (root->click(event.button.x, event.button.y)) break;
                switch(task) {
                    case Task::Move: {
                        task = Task::None;
                        Realm *existing = world.getNearest(mapX, mapY, std::vector<int>{ }, 1);
                        if (existing) {
                            statusMessage->setText("Space already occupied.");
                        } else {
                            taskRealm->x = mapX;
                            taskRealm->y = mapY;
                            taskRealm = nullptr;
                        }
                        break; }
                    case Task::None:
                        if (hoverRealm) {
                            if (realmList->isShown()) {
                                for (unsigned i = 0; i < realmList->items.size(); ++i) {
                                    if (realmList->items[i].ident == hoverRealm->ident) {
                                        realmList->selection = i;
                                        break;
                                    }
                                }
                            } else if (speciesList->isShown()) {
                                for (unsigned i = 0; i < speciesList->items.size(); ++i) {
                                    if (speciesList->items[i].ident == hoverRealm->primarySpecies) {
                                        speciesList->selection = i;
                                        break;
                                    }
                                }
                            } else if (factionList->isShown()) {
                                for (unsigned i = 0; i < factionList->items.size(); ++i) {
                                    if (factionList->items[i].ident == hoverRealm->faction) {
                                        factionList->setSelection(i);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (task == Task::None) statusMessage->setText("");
                switch(event.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        return;
                    case SDLK_1:
                        realmList->setShow(true);
                        speciesList->setShow(false);
                        factionList->setShow(false);
                        break;
                    case SDLK_2:
                        realmList->setShow(false);
                        speciesList->setShow(true);
                        factionList->setShow(false);
                        break;
                    case SDLK_3:
                        realmList->setShow(false);
                        speciesList->setShow(false);
                        factionList->setShow(true);
                        break;
                    case SDLK_6:
                        realm_list(world, r, nullptr);
                        break;
                    case SDLK_7:
                        species_list(world, r, nullptr);
                        break;
                    case SDLK_e:
                        if (hoverRealm) realm_list(world, r, hoverRealm);
                        break;
                    case SDLK_f:
                        colourMode = Mode::Faction;
                        break;
                    case SDLK_s:
                        colourMode = Mode::Species;
                        break;
                    case SDLK_m:
                        if (hoverRealm) {
                            taskRealm = hoverRealm;
                            task = Task::Move;
                            statusMessage->setText("Select destination.");
                        }
                        break;
                    case SDLK_TAB:
                        if (root->isShown()) root->setShow(false);
                        else root->setShow(true);
                        break;
                    // case SDLK_LEFTBRACKET:
                    //     if (scale > 1) --scale;
                    //     break;
                    // case SDLK_RIGHTBRACKET:
                    //     if (scale < 40) ++scale;
                    //     break;
                }
            }
        }
    }
}