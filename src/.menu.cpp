#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//// Paired Header
#include "menu.h"

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define menu items
const char *menuItems[] =
    {
        "Calibrate O2",
        "Nitrox",
        "Trimix",
        "Raw Data"};
        
const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

// Button pins
const int buttonUpPin = 27;
const int buttonDownPin = 25;
const int buttonSelectPin = 26;

// Variables to keep track of menu state
int selectedItem = 0;
bool inSubMenu = false;

void menu_initialise()
{
    pinMode(buttonUpPin, INPUT_PULLUP);
    pinMode(buttonDownPin, INPUT_PULLUP);
    pinMode(buttonSelectPin, INPUT_PULLUP);

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.cp437(true);
    display.display();
    delay(2000);
    display.clearDisplay();

    drawMenu();
}

void main_menu_navigation()
{
    // Check if the select button is pressed
    if (digitalRead(buttonSelectPin) == LOW)
    {
        if (!inSubMenu)
        {
            inSubMenu = true;
            drawSubMenu();
        }
        delay(200); // debounce delay
    }

    if (!inSubMenu)
    {
        // Check if the up button is pressed
        if (digitalRead(buttonUpPin) == LOW)
        {
            selectedItem--;
            if (selectedItem < 0)
            {
                selectedItem = menuLength - 1;
            }
            drawMenu();
            delay(200); // debounce delay
        }

        // Check if the down button is pressed
        if (digitalRead(buttonDownPin) == LOW)
        {
            selectedItem++;
            if (selectedItem >= menuLength)
            {
                selectedItem = 0;
            }
            drawMenu();
            delay(200); // debounce delay
        }
    }
    else
    {
        // Check if the down button is pressed to return to menu
        if (digitalRead(buttonDownPin) == LOW)
        {
            inSubMenu = false;
            drawMenu();
            delay(200); // debounce delay
        }
    }
}

void drawMenu()
{
    display.clearDisplay();

    int middleIndex = 1; // The middle item position in the 3-item display (0, 1, 2)
    int displayOffset = selectedItem - middleIndex;

    // Adjust displayOffset for looping
    if (displayOffset < 0)
    {
        displayOffset += menuLength;
    }

    for (int i = 0; i < 3; i++)
    {
        int itemIndex = (displayOffset + i) % menuLength;
        display.setTextColor(SSD1306_WHITE);
        if (itemIndex < menuLength)
        {
            if (itemIndex == selectedItem)
            {
                // Draw a box around the selected item
                display.drawRect(0, i * 20 + 4, SCREEN_WIDTH, 16, SSD1306_WHITE);
                display.setCursor(SCREEN_WIDTH - 10, i * 20 + 4 + 4);
                // display.print(" ");
                display.write(17);
                // display.print(" ");

                display.setCursor(5, i * 20 + 4 + 4);
                display.write(16);
                display.print(" ");
            }
            else
            {
                display.setCursor(30, i * 20 + 4 + 4);
                // display.print("   ");
            }

            display.print(menuItems[itemIndex]);
        }
    }
}

void drawSubMenu()
{
    display.clearDisplay();

    // Display the selected item name
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(menuItems[selectedItem]);

    // Matsh Case to display & run the correct functions

    // Display the "Return to Menu" option
    display.setCursor(0, SCREEN_HEIGHT - 8);
    display.print("Menu");

    display.display();
}