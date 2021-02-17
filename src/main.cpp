#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "Rotary.h"
#include "Menu.h"

#define COLS 20
#define ROWS 4

#define INPUT_CLK 4
#define INPUT_DT 5
#define INPUT_SW 2

void onButtonClick();

Rotary rotary = Rotary(INPUT_CLK, INPUT_DT);
LiquidCrystal_I2C lcd(0x27, COLS, ROWS);
MenuNavigator* navigator;
Menu* menu;
volatile bool clicked = false;

void setup() {
  delay(1000);

  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println("setup");

  pinMode(INPUT_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPUT_SW), onButtonClick, LOW);

  lcd.init();
  lcd.backlight();
  lcd.noAutoscroll();

  auto* builder = new MenuBuilder();

  menu = builder
    ->addMenuItem(0, "item 0")
      ->startSubMenu()
      ->addMenuItem(0, "item 0 a")
      ->addMenuItem(0, "item 0 b")
      ->addMenuItem(0, "item 0 c")
      ->addMenuItem(0, "item 0 d")
      ->addMenuItem(0, "item 0 e")
      ->addMenuItem(0, "item 0 f")
      ->addMenuItem(0, "item 0 g")
      ->endSubMenu()
    ->addMenuItem(0, "item 1")
      ->startSubMenu()
      ->addMenuItem(0, "item 1 a")
      ->addMenuItem(0, "item 1 b")
        ->startSubMenu()
        ->addMenuItem(0, "item 1 b 0")
        ->addMenuItem(0, "item 1 b 1")
        ->endSubMenu()
      ->endSubMenu()
    ->addMenuItem(0, "item 2")
    ->addMenuItem(0, "item 3")
    ->addMenuItem(0, "item 4")
    ->addMenuItem(0, "item 5")
    ->addMenuItem(0, "item 6")
    ->build();

  navigator = new MenuNavigator(menu, COLS, ROWS);
}

void loop() {

  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    navigator->goUp();
  } else if (result == DIR_CCW) {
    navigator->goDown();
  } else if (clicked) {
    navigator->selectItem();
    delay(100);
    clicked = false;
  }

  char** menuString = navigator->render();
  if (menuString != nullptr) {
    for (int i=0; i<ROWS; i++) {
      lcd.setCursor(0, i);
      lcd.printstr(menuString[i]);
    }
  }

}

void onButtonClick() {
    clicked = true;
}