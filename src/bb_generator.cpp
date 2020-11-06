#include <string>
#include <vector>
#include "realms.h"

std::string makeNameCore(int depth);

std::vector<std::string> usedNames;

std::vector<Colour> colourList = {
    {240,163,255},
    {0,117,220},
    {153,63,0},
    {76,0,92},
    {25,25,25},
    {0,92,49},
    {43,206,72},
    {255,204,153},
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

std::vector<int> wordCount{
    1, 1, 1, 2, 2, 2
};

std::vector<std::string> syllableForms{
    "CV", "CV", "CV", "CVC", "CVC"
};

std::vector<std::string> C{
    "b", "d", "f", "g", "j",   "k", "l", "m", "n",
    "p", "r", "s", "t", "v",   "z"
};

std::vector<std::string> V{
    "a", "e", "i", "o", "u"
};

std::string makeName() {
    const int MAX_ITERATIONS = 100;
    const int MIN_SYLLABLES = 2;
    const int MAX_SYLLABLES = 5;

    int iterations = 0;
    while (iterations < MAX_ITERATIONS) {
        int words = rngVector(wordCount);
        std::string name;

        for (int i = 0; i < words; ++i) {
            if (i != 0) name += " ";
            int sylCount = MIN_SYLLABLES + rngNext(MAX_SYLLABLES - MIN_SYLLABLES);
            for (int j = 0; j < sylCount; ++j) {
                std::string form = rngVector(syllableForms);
                for (char c : form) {
                    if (c == 'C') name += rngVector(C);
                    if (c == 'V') name += rngVector(V);
                }
            }
        }

        name[0] = name[0] - ('a' - 'A');
        if (!contains(usedNames, name)) {
            usedNames.push_back(name);
            return name;
        } else ++iterations;
    }

    return "(namegen exceeded max iterations)";
}



std::vector<Stance> stanceList{
    Stance::Biped, Stance::Biped, Stance::Biped, Stance::Biped,
    Stance::Biped, Stance::Quad, Stance::Quad, Stance::Taur,
    Stance::Taur, Stance::Thero,
};

std::vector<Wings> wingList{
    Wings::None, Wings::None, Wings::None, Wings::None,
    Wings::Arm, Wings::Arm, Wings::Back,
};

Species* makeSpecies() {
    static int identCounter = 0;
    static unsigned nextColour = 0;
    Species *s = new Species;
    s->ident = identCounter;
    ++identCounter;
    s->name = makeName();
    s->abbrev = s->name.substr(0, 2);
    s->height = 50 + rngNext(150);
    s->stance = rngVector(stanceList);
    s->homeRealm = -1;
    s->colour = colourList[nextColour];
    ++nextColour;
    if (nextColour >= colourList.size()) nextColour = 0;
    if (s->stance == Stance::Taur)  s->wings = Wings::None;
    else                            s->wings = rngVector(wingList);
    return s;
}
