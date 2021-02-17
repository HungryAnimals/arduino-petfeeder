
#include "Arduino.h"
#include "Menu.h"

MenuNavigator::MenuNavigator(Menu* root, int cols, int rows) :
    root(root),
    current(root),
    dirty(true),
    cols(cols),
    rows(rows) {


    *this->renderedMenu = new char[rows];
    for (int i=0; i<rows; i++) {
        this->renderedMenu[i] = new char[cols + 1];
        memset(this->renderedMenu[i], '\0', cols + 1);
    }
    
    this->goTo(root, 0);

}

MenuNavigator::~MenuNavigator() {

    for (int i=0; i<rows; i++) {
        delete[] this->renderedMenu[i];
    }

    this->root          = nullptr;
    this->current       = nullptr;
    this->dirty         = false;
    this->cols          = 0;
    this->rows          = 0;
}

void MenuNavigator::goTo(Menu* menu, int menuItemIndex) {
    if (menuItemIndex < 0) {
        menuItemIndex = menu->itemCount - (abs(menuItemIndex) % menu->itemCount);
    }
    this->current = menu;
    this->current->selectedItemIndex = abs(menuItemIndex % menu->itemCount);
    this->dirty = true;
}

void MenuNavigator::goToRoot() {
    this->goTo(this->root);
}

void MenuNavigator::goBack() {
    auto* item = this->selectedItem();
    auto* parentMenuItem = item->menu->parentMenuItem;
    if (parentMenuItem == nullptr) {
        return;
    }
    auto* newMenu = parentMenuItem->menu;
    if (newMenu == nullptr) {
        return;
    }
    this->goTo(newMenu, newMenu->selectedItemIndex);
}

void MenuNavigator::goDown() {
    auto* menu = this->selectedMenu();
    this->goTo(menu, menu->selectedItemIndex + 1);
}

void MenuNavigator::goUp() {
    auto* menu = this->selectedMenu();
    this->goTo(menu, menu->selectedItemIndex - 1);
}

Menu* MenuNavigator::selectedMenu() {
    return this->current;
}

MenuItem* MenuNavigator::selectedItem() {
    auto* menu = this->selectedMenu();
    return menu->items[menu->selectedItemIndex];
}

void MenuNavigator::selectItem(int menuItemIndex) {

    auto* menu              = this->selectedMenu();
    auto* item              = this->selectedItem();
    auto* parentMenuItem    = item->menu->parentMenuItem;

    if (menuItemIndex == 0 && parentMenuItem != nullptr) {
        this->goTo(parentMenuItem->menu, parentMenuItem->menu->selectedItemIndex);
        return;
    }

    if (item->selectCallback != nullptr) {
        item->selectCallback(this, menu, item);
    }

    if (item->subMenu != nullptr) {
        this->goTo(item->subMenu);
    }
}

void MenuNavigator::renderMenuItem(char* dest, int maxLen, MenuItem* menuItem) {
    memcpy(dest, menuItem->text, min(strlen(menuItem->text), maxLen));
}

char** MenuNavigator::render() {

    if (!this->dirty) {
        return nullptr;
    }

    auto* menu              = this->selectedMenu();
    auto* item              = this->selectedItem();
    auto* parentMenuItem    = item->menu->parentMenuItem;
    int menuItemCount       = menu->itemCount;

    int windowStart = max(0, min(menu->selectedItemIndex, menu->itemCount - this->rows));

    for (int i=0, j=windowStart; i<this->rows; i++, j++) {
        memset(this->renderedMenu[i], ' ', cols);
        if (j >= menu->itemCount) {
            continue;
        }

        auto* menuItem = menu->items[j];
        if (j == 0 && parentMenuItem != nullptr) {
            if (j == menu->selectedItemIndex) {
                memcpy(this->renderedMenu[i], "<-", 2);
            } else {
                memcpy(this->renderedMenu[i], "< ", 2);
            }

        } else {
            if (j == menu->selectedItemIndex) {
                memcpy(this->renderedMenu[i], "->", 2);
            } else {
                memcpy(this->renderedMenu[i], "  ", 2);
            }
        }
        
        menuItem->renderMenuCallback(this->renderedMenu[i] + 2, cols - 2, menuItem);

    }

    this->dirty = false;
    return this->renderedMenu;
}
