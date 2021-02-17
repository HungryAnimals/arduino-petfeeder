
#ifndef Menu_h
#define Menu_h

struct Menu;
struct MenuItem;
struct MenuDef;
struct MenuItemDef;
class MenuNavigator;

typedef int (MenuItemCallback)(MenuNavigator* navigator, Menu*, MenuItem*);
typedef void (RenderMenuItemCallback)(char* dest, int maxLen, MenuItem*);

struct Menu {
    MenuItem** items;
    MenuItem* parentMenuItem;
    int itemCount;
    int selectedItemIndex;
};

struct MenuItem {
    int id;
    char* text;
    void* data;

    MenuItemCallback* selectCallback;
    RenderMenuItemCallback* renderMenuCallback;

    Menu* subMenu;
    Menu* menu;
};

class MenuBuilder {
  public:
    MenuBuilder();
    MenuBuilder* addMenuItem(int id, const char* text, void* data = nullptr,  MenuItemCallback* selectCallback = nullptr, RenderMenuItemCallback* renderMenuCallback = nullptr);
    MenuBuilder* startSubMenu();
    MenuBuilder* endSubMenu();
    Menu* build();
  private:
    Menu* currentMenu;
    MenuItem* currentMenuItem;
};

class MenuNavigator {
  public:
    MenuNavigator(Menu* root, int cols, int rows);
    ~MenuNavigator();

    void goTo(Menu* menu, int menuItemIndex = 0);
    void goToRoot();
    void goBack();
    void goDown();
    void goUp();

    Menu* selectedMenu();
    MenuItem* selectedItem();

    void selectItem(int menuItemIndex = -1);

    char** render();

    static void renderMenuItem(char* dest, int maxLen, MenuItem* menuItem);

  private:
    Menu* root;
    Menu* current;
    bool dirty;
    int cols;
    int rows;
    char* renderedMenu[];

};

#endif
