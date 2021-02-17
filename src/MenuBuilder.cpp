
#include "Menu.h"
#include <Arduino.h>

static void handleBackMenu(MenuNavigator* navigator, Menu* menu, MenuItem* item) {
    Serial.println("handleBackMenu");
    navigator->goBack();
}

MenuBuilder::MenuBuilder() {
    this->currentMenu                       = new Menu();
    this->currentMenu->items                = nullptr;
    this->currentMenu->parentMenuItem       = nullptr;
    this->currentMenu->itemCount            = 0;
    this->currentMenu->selectedItemIndex    = 0;
    this->currentMenuItem                   = nullptr;
}

MenuBuilder* MenuBuilder::addMenuItem(int id, const char* text, void* data,  MenuItemCallback* selectCallback, RenderMenuItemCallback* renderMenuCallback) {

    auto* item = new MenuItem();
    item->id = id;
    item->text = text;
    item->data = data;
    item->selectCallback = selectCallback;
    item->renderMenuCallback = renderMenuCallback;
    item->subMenu = nullptr;
    item->menu = this->currentMenu;

    if (item->renderMenuCallback == nullptr) {
        item->renderMenuCallback = &MenuNavigator::renderMenuItem;
    }

    MenuItem** items = malloc(sizeof(MenuItem) * (this->currentMenu->itemCount + 1));
    items[this->currentMenu->itemCount] = item;
    for (int i=0; i<this->currentMenu->itemCount; i++) {
        items[i] = this->currentMenu->items[i];
    }
    this->currentMenu->itemCount += 1;
    this->currentMenu->items = items;
    this->currentMenuItem = item;

    return this;
}

MenuBuilder* MenuBuilder::startSubMenu() {
    
    if (this->currentMenuItem == nullptr) {
        Serial.println("can't startSubMenu, no menu item");
        return nullptr;
    } else if (this->currentMenuItem->subMenu != nullptr) {
        Serial.println("can't startSubMenu, already has one");
        return nullptr;
    }

    auto* parentMenuItem = this->currentMenuItem;
    
    auto* subMenu = new Menu();
    subMenu->items = nullptr;
    subMenu->parentMenuItem = parentMenuItem;
    subMenu->itemCount = 0;
    subMenu->selectedItemIndex = 0;

    this->currentMenuItem->subMenu = subMenu;
    this->currentMenu = subMenu;
    this->currentMenuItem = nullptr;

    return this->addMenuItem(-1, parentMenuItem->text, nullptr, &handleBackMenu, parentMenuItem->renderMenuCallback);
}

MenuBuilder* MenuBuilder::endSubMenu() {
    
    if (this->currentMenu->parentMenuItem == nullptr) {
        Serial.println("can't endSubMenu, at root");
        return nullptr;
    }
    this->currentMenuItem = this->currentMenu->parentMenuItem;
    this->currentMenu = this->currentMenuItem->menu;

    return this;
}

Menu* MenuBuilder::build() {
    
    if (this->currentMenu->parentMenuItem != nullptr) {
        Serial.println("can't build, not at root");
        return nullptr;
    }
    return this->currentMenu;
}