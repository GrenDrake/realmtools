#include <iostream>
#include <string>

#include <SDL.h>

#include "viewer.h"

void RenderInfo::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void RenderInfo::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void RenderInfo::drawRect(int x, int y, int w, int h) {
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);
}

void RenderInfo::drawText(int x, int y, const std::string &text, int r, int g, int b) {
    SDL_SetTextureColorMod(font, r, g, b);
    drawText(x, y, text);
    SDL_SetTextureColorMod(font, 255, 255, 255);
}

void RenderInfo::drawText(int x, int y, const std::string &text) {
    SDL_Rect src = { 0, 0, fontWidth, fontHeight };
    SDL_Rect dest = { x, y, fontWidth, fontHeight };

    for (unsigned i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (c == 1) {
            int r = text[i + 1];
            int g = text[i + 2];
            int b = text[i + 3];
            i += 3;
            setColour(UIColour(r, g, b));
            fillRect(dest.x, dest.y, fontWidth, fontHeight);
        } else {
            src.x = c * fontWidth;
            SDL_RenderCopy(renderer, font, &src, &dest);
        }
        dest.x += fontWidth;
    }
}

void RenderInfo::fillRect(int x, int y, int w, int h) {
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

int RenderInfo::getHeight() {
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    return h;
}

int RenderInfo::getWidth() {
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    return w;
}

SDL_Texture* RenderInfo::loadTexture(const std::string filename) {
    std::cerr << "Loading texture from " << filename << ".\n";
    SDL_Surface *temp = SDL_LoadBMP(filename.c_str());
    if (!temp) {
        std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << '\n';
        return nullptr;
    }
    SDL_Texture *result = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    if (!result) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << '\n';
        return nullptr;
    }
    return result;
}

void RenderInfo::render() {
    SDL_RenderPresent(renderer);
}

void RenderInfo::setColour(const UIColour &c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
}




UIWidget::UIWidget()
: mX(0), mY(0), mWidth(0), mHeight(0), mShown(true)
{ }

UIWidget::~UIWidget() {
    for (UIWidget *child : mChildren) delete child;
}


void UIWidget::move(int x, int y) {
    mX = x;
    mY = y;
}

void UIWidget::resize(int w, int h) {
    mWidth = w;
    mHeight = h;
}

void UIWidget::addChild(UIWidget *child, int x, int y) {
    if (child) {
        mChildren.push_back(child);
        child->move(mX + x, mY + y);
    }
}

UILabel::UILabel(const std::string &text)
: mAlign(LEFT), mText(text)
{ ; }

void UILabel::setAlign(Alignment align) {
    mAlign = align;
}

void UILabel::setText(const std::string &text) {
    mText = text;
}

void UILabel::draw(RenderInfo &r) {
    int dX = mX;
    if (mAlign == RIGHT)        dX -= mText.length() * r.fontWidth;
    else if (mAlign == CENTRE)  dX -= mText.length() * r.fontWidth / 2;
    r.drawText(dX, mY, mText);
}

bool UILabel::handleClick(int x, int y) {
    return false;
}


UIList::UIList()
: selection(-1)
{ }

void UIList::draw(RenderInfo &r) {
    r.setColour(WHITE);
    r.fillRect(mX, mY, mWidth, mHeight);
    lineHeight = r.fontHeight * 1.2;
    const unsigned maxLines = (mHeight - 4) / lineHeight;
    SDL_Rect clip = { mX, mY, mWidth, mHeight };
    SDL_RenderSetClipRect(r.renderer, &clip);

    for (unsigned i = 0; i < items.size() && i < maxLines + 1; ++i) {
        if (i == selection) {
            r.setColour(BLACK);
            r.fillRect(mX, mY + 2 + i * lineHeight, mWidth, lineHeight);
            r.drawText(mX + 2, mY + 2 + i * lineHeight, items[i].text, 255, 255, 255);
        } else {
            r.drawText(mX + 2, mY + 2 + i * lineHeight, items[i].text, 0, 0, 0);
        }
    }

    SDL_RenderSetClipRect(r.renderer, nullptr);
    int scrollbarX = mX + mWidth - 16;
    r.setColour(WHITE);
    r.fillRect(scrollbarX, mY, 16, mHeight);
    r.setColour(BLACK);

    SDL_RenderDrawLine(r.renderer, scrollbarX, mY, scrollbarX, mY + mHeight);
    // up arrow
    SDL_RenderDrawLine(r.renderer, scrollbarX, mY + 16, scrollbarX + 8, mY);
    SDL_RenderDrawLine(r.renderer, scrollbarX + 8, mY, scrollbarX + 16, mY + 16);
    SDL_RenderDrawLine(r.renderer, scrollbarX, mY + 16, scrollbarX + 16, mY + 16);
    // down arrow
    SDL_RenderDrawLine(r.renderer, scrollbarX, mY + mHeight - 16, scrollbarX + 8, mY + mHeight);
    SDL_RenderDrawLine(r.renderer, scrollbarX + 8, mY + mHeight, scrollbarX + 16, mY + mHeight - 16);
    SDL_RenderDrawLine(r.renderer, scrollbarX, mY + mHeight - 16, scrollbarX + 16, mY + mHeight - 16);
}

bool UIList::handleClick(int x, int y) {
    int ry = y - mY;
    if (x > mX + mWidth - 16) {
        // scrollbar click
        return true;
    }

    // item selection
    int item = ry / lineHeight;
    if (item >= items.size()) {
        selection = -1;
    } else {
        selection = item;
    }
    return true;
}

ListRow BAD_ROW { "", -1 };
ListRow& UIList::getSelection() {
    if (selection >= items.size()) return BAD_ROW;
    return items[selection];
}

void UIList::setSelection(unsigned index) {
    if (index < items.size()) selection = index;
    else selection = -1;
}


void UIPanel::draw(RenderInfo &r) {
    if (!mShown) return;
    r.setColour(DARKGREY);
    r.fillRect(mX, mY, mWidth, mHeight);
    r.setColour(LIGHTGREY);
    r.drawRect(mX, mY, mWidth, mHeight);

    for (UIWidget *child : mChildren) {
        child->repaint(r);
    }
}

bool UIPanel::handleClick(int x, int y) {
    for (UIWidget *child : mChildren) {
        if (child->contains(x, y)) {
            bool r = child->click(x, y);
            if (r) return true;
        }
    }
    return true;
}

void UIRoot::draw(RenderInfo &r) {
    if (!mShown) return;
    for (UIWidget *child : mChildren) {
        child->repaint(r);
    }
}

bool UIRoot::handleClick(int x, int y) {
    for (UIWidget *child : mChildren) {
        if (child->contains(x, y)) {
            bool r = child->click(x, y);
            if (r) return true;
        }
    }
    return false;
}
