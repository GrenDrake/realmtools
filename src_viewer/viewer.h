#ifndef VIEWER_H_489302
#define VIEWER_H_489302

#include <string>
#include <vector>

struct RenderInfo;

struct UIColour {
    explicit UIColour(int s) : r(s), g(s), b(s) { }
    UIColour(int r, int g, int b) : r(r), g(g), b(b) { }

    int r, g, b;
};

class UIWidget {
public:
    UIWidget();
    virtual ~UIWidget();

    bool contains(int x, int y) const { return x >= mX && y >= mY && x < mX + mWidth && y < mY + mHeight; }
    void move(int x, int y);
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth()  const { return mWidth; }
    int getHeight() const { return mHeight; }
    bool isShown() { return mShown; }
    void resize(int w, int h);
    void setShow(bool show) { mShown = show; }
    void addChild(UIWidget *child, int x, int y);

    void repaint(RenderInfo &r) { if (!mShown) return; draw(r); }
    bool click(int x, int y)    { if (!mShown) return false; return handleClick(x, y); }

    virtual void draw(RenderInfo &r) = 0;
    virtual bool handleClick(int x, int y) = 0;
protected:
    int mX, mY, mWidth, mHeight;
    std::vector<UIWidget*> mChildren;
    bool mShown;
};

class UILabel : public UIWidget {
public:
    enum Alignment { LEFT, CENTRE, RIGHT };

    UILabel(const std::string &text);
    void setAlign(Alignment align);
    void setText(const std::string &text);
    virtual void draw(RenderInfo &r) override;
    virtual bool handleClick(int x, int y) override;
private:
    Alignment mAlign;
    std::string mText;
};

struct ListRow {
    std::string text;
    int ident;
};
class UIList : public UIWidget {
public:
    UIList();
    virtual void draw(RenderInfo &r) override;
    virtual bool handleClick(int x, int y) override;

    ListRow& getSelection();
    void setSelection(unsigned index);

    std::vector<ListRow> items;
    unsigned selection;
private:
    int lineHeight;
};

class UIPanel : public UIWidget {
public:
    virtual void draw(RenderInfo &r) override;
    virtual bool handleClick(int x, int y) override;
};

class UIRoot : public UIWidget {
public:
    virtual void draw(RenderInfo &r) override;
    virtual bool handleClick(int x, int y) override;
};

struct RenderInfo {
    void clear();
    void drawLine(int x1, int x2, int y1, int y2);
    void drawRect(int x, int y, int w, int h);
    void drawText(int x, int y, const std::string &text, int r, int g, int b);
    void drawText(int x, int y, const std::string &text);
    void fillRect(int x, int y, int w, int h);
    int getHeight();
    int getWidth();
    SDL_Texture* loadTexture(const std::string filename);
    void render();
    void setColour(const UIColour &c);

    SDL_Window *window;
    SDL_Renderer *renderer;
    int fontWidth, fontHeight;
    SDL_Texture *font;
};

const unsigned NO_SELECTION = -1;

const UIColour WHITE(255);
const UIColour BLACK(0);
const UIColour INVALID(255, 0, 255);
const UIColour HIGHLIGHT(255, 127, 127);
const UIColour LIGHTGREY(191);
const UIColour MIDGREY(127);
const UIColour DARKGREY(63);
const UIColour VERYDARKGREY(31);
const UIColour RED(255, 0, 0);
const UIColour BLUE(0, 0, 255);
const UIColour GREEN(0, 255, 0);
const UIColour PINK(255, 0, 255);

#endif
