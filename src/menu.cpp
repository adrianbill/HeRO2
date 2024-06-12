#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Menu element struct definition
struct menu_entry_type
{
    const uint8_t *font;
    uint16_t icon0;
    uint16_t icon1;
    const char *name;
};

// Menu state struct definition
struct menu_state
{
    int16_t menu_start;     /* in pixel */
    int16_t frame_position; /* in pixel */
    uint8_t position;       /* position, array index */
};

// Icon configuration
#define ICON_WIDTH 40
#define ICON_HEIGHT 38
#define ICON_GAP 4
#define ICON_BGAP 5
#define ICON_Y 32 + ICON_GAP - 4
#define ICON_Y_Text ICON_Y
#define ICON_W_2nd_Letter 27
#define ELEMENTS 6 // number of elements in menu + null element

// Button pin configuration
#define NEXT_PIN 25
#define SELECT_PIN 26
#define PREV_PIN 27

// Main Menu items
// {font, icon character 1, icon character 2, Title} second icon character used for O2 & He lemements
struct menu_entry_type menu_entry_list[ELEMENTS] =
    {
        {u8g2_font_helvR24_te, 79, 8322, "Nitrox"}, // 79 = "O", 8322 = "₂"
        {u8g2_font_helvR24_te, 72, 101, "Trimix"},  // 72 = "H", 101 = "e"
        {u8g2_font_open_iconic_embedded_4x_t, 72, 0, "Calibration"},
        {u8g2_font_open_iconic_weather_4x_t, 64, 0, "Climate"},
        {u8g2_font_open_iconic_embedded_4x_t, 70, 0, "Raw Data"},
        {NULL, 0, 0, NULL}};

int8_t button_event = 0; // set this to 0, once the event has been processed

struct menu_state current_state = {ICON_BGAP, ICON_BGAP, 0};
struct menu_state destination_state = {ICON_BGAP, ICON_BGAP, 0};

enum View
{
    MAIN_MENU,
    SUB_MENU
};

View current_view = MAIN_MENU;

// Functions
void display_initialise();
void check_button_event();
void draw_main(struct menu_state *state);
void to_right(struct menu_state *state);
void to_left(struct menu_state *state);
uint8_t towards_int16(int16_t *current, int16_t dest);
uint8_t towards(struct menu_state *current, struct menu_state *destination);
void run_menu();
void navigate_menu();
void run_submenu();
void navigate_submenu();
void draw_raw_data();

void setup()
{
    display_initialise();
}

void loop()
{
    check_button_event();

    if (current_view == MAIN_MENU)
    {
        run_menu();
    }
    else if (current_view == SUB_MENU)
    {
        run_submenu();
        navigate_submenu();
    }
    // run_menu();
}

// Functions

void display_initialise()
{
    u8g2.begin(SELECT_PIN, NEXT_PIN, PREV_PIN);
    u8g2.enableUTF8Print();
    // u8g2.setFont(u8g2_font_unifont_te);
    u8g2.setFont(u8g2_font_helvR10_te);
}

void check_button_event()
{
    if (button_event == 0)
        button_event = u8g2.getMenuEvent();
}

void draw_main(struct menu_state *state)
{
    int16_t x;
    uint8_t i;
    x = state->menu_start;
    i = 0;
    while (menu_entry_list[i].icon0 > 0)
    {
        if (x >= -ICON_WIDTH && x < u8g2.getDisplayWidth())
        {
            u8g2.setFont(menu_entry_list[i].font);
            int16_t x0;
            int16_t y0;
            int16_t x1;
            int16_t y1;

            switch (i)
            {
            case 0:
                x0 = x + 2;
                y0 = ICON_Y;
                x1 = x + ICON_W_2nd_Letter;
                y1 = ICON_Y - 3;

                u8g2.drawGlyph(x0, y0, menu_entry_list[i].icon0);
                u8g2.drawGlyph(x1, y1, menu_entry_list[i].icon1);

                break;
            case 1:
                x0 = x;
                y0 = ICON_Y;
                x1 = x + ICON_W_2nd_Letter - 6;
                y1 = ICON_Y;

                u8g2.drawGlyph(x0, y0, menu_entry_list[i].icon0);
                u8g2.drawGlyph(x1, y1, menu_entry_list[i].icon1);

                break;
            default:
                x0 = x + 4;
                y0 = ICON_Y + 4;

                u8g2.drawGlyph(x0, y0, menu_entry_list[i].icon0);

                break;
            }
        }
        i++;
        x += ICON_WIDTH + ICON_GAP;
        check_button_event();
    }
    u8g2.drawFrame(state->frame_position - 1, ICON_Y - ICON_HEIGHT + 7, ICON_WIDTH + 2, ICON_WIDTH);
    u8g2.drawFrame(state->frame_position - 2, ICON_Y - ICON_HEIGHT + 6, ICON_WIDTH + 4, ICON_WIDTH + 2);

    check_button_event();
}

void to_right(struct menu_state *state)
{
    if (menu_entry_list[state->position + 1].font != NULL)
    {
        if ((int16_t)state->frame_position + 2 * (int16_t)ICON_WIDTH + (int16_t)ICON_BGAP < (int16_t)u8g2.getDisplayWidth())
        {
            state->position++;
            state->frame_position += ICON_WIDTH + (int16_t)ICON_GAP;
        }
        else
        {
            state->position++;
            state->frame_position = (int16_t)u8g2.getDisplayWidth() - (int16_t)ICON_WIDTH - (int16_t)ICON_BGAP;
            state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);
        }
    }
}

void to_left(struct menu_state *state)
{
    if (state->position > 0)
    {
        if ((int16_t)state->frame_position >= (int16_t)ICON_BGAP + (int16_t)ICON_WIDTH + (int16_t)ICON_GAP)
        {
            state->position--;
            state->frame_position -= ICON_WIDTH + (int16_t)ICON_GAP;
        }
        else
        {
            state->position--;
            state->frame_position = ICON_BGAP;
            state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);
        }
    }
}

uint8_t towards_int16(int16_t *current, int16_t dest)
{
    if (*current < dest)
    {
        (*current)++;
        return 1;
    }
    else if (*current > dest)
    {
        (*current)--;
        return 1;
    }
    return 0;
}

// new towards function
uint8_t towards(struct menu_state *current, struct menu_state *destination)
{
    uint8_t r = 0;
    r |= towards_int16(&(current->frame_position), destination->frame_position);
    r |= towards_int16(&(current->frame_position), destination->frame_position);
    r |= towards_int16(&(current->menu_start), destination->menu_start);
    r |= towards_int16(&(current->menu_start), destination->menu_start);
    return r;
}

// new run_menu
void run_menu()
{
    do
    {
        u8g2.clearBuffer();
        draw_main(&current_state);

        if (destination_state.position != 0)
        {
            u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
            u8g2.drawGlyph(0, u8g2.getDisplayHeight(), 65); // left arrow
        }

        // u8g2.setFont(u8g2_font_unifont_te);
        u8g2.setFont(u8g2_font_helvR12_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth(menu_entry_list[destination_state.position].name)) / 2, u8g2.getDisplayHeight() - 5);
        u8g2.print(menu_entry_list[destination_state.position].name);

        if (destination_state.position != (ELEMENTS - 2))
        {
            u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
            u8g2.drawGlyph(u8g2.getDisplayWidth() - 16, u8g2.getDisplayHeight(), 66); // right arrow
        }

        u8g2.sendBuffer();

        check_button_event();

        navigate_menu();

    } while (towards(&current_state, &destination_state));
}

void navigate_menu()
{
    if (button_event == U8X8_MSG_GPIO_MENU_NEXT)
        to_right(&destination_state);
    if (button_event == U8X8_MSG_GPIO_MENU_PREV)
        to_left(&destination_state);
    if (button_event == U8X8_MSG_GPIO_MENU_SELECT)
    {
        current_view = SUB_MENU;
        // u8g2.userInterfaceMessage("Selection:", menu_entry_list[destination_state.position].name, "", " Ok ");
    }
    if (button_event > 0) // all known events are processed, clear event
        button_event = 0;
}

// new run_submenu
void run_submenu()
{
    int y_start = 10;
    int y_gap = 18;
    int x_gap = 0;

    double O2_fraction = 20.004532;
    double He_fraction = 35.004532;
    double H2O_fraction = 2.561562;

    double temperature = 24.55532;
    double relative_humidity = 35.004532;
    double local_pressure = 15.5431236;

    double duration = 467.004532;
    double O2_millivolts = 10.12532;

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_helvR08_te);

    u8g2.drawStr(0, y_start, menu_entry_list[destination_state.position].name);

    u8g2.drawHLine(0, y_start + 3, u8g2.getDisplayWidth());

    // u8g2.drawStr((u8g2.getDisplayWidth() - u8g2.getStrWidth("main menu")) / 2, u8g2.getDisplayHeight(), "main menu");

    switch (destination_state.position)
    {
    case 0: // Nitrox
        u8g2.setFont(u8g2_font_helvR08_te);

        u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getStrWidth("H₂O : 0.00%"), y_start);
        u8g2.print("H₂O ");
        u8g2.print(H2O_fraction,1);
        u8g2.print("%");

        u8g2.setFont(u8g2_font_helvR10_tr);
        x_gap = (u8g2.getDisplayWidth() - u8g2.getStrWidth("Oxygen")) / 2;
        u8g2.drawUTF8(x_gap, 14 + y_start, "Oxygen");
        u8g2.drawHLine(x_gap - 1, 18 + y_start + 2, u8g2.getStrWidth("Oxygen") + 1);

        u8g2.setFont(u8g2_font_helvR24_te);

        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("00.00%")) / 2, u8g2.getDisplayHeight() - 14);
        u8g2.print(O2_fraction, 2);
        u8g2.print("%");

        u8g2.setFont(u8g2_font_helvR08_te);

        u8g2.setCursor(0, u8g2.getDisplayHeight() - 3);
        u8g2.print("O₂ Cell: ");
        u8g2.print(O2_millivolts, 1);
        u8g2.print(" mV");

        u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getStrWidth("H₂O : 0.00%"), u8g2.getDisplayHeight() - 3);
        u8g2.print("H₂O ");
        u8g2.print(H2O_fraction);
        u8g2.print("%");

        break;
    case 1: // Trimix
        u8g2.setFont(u8g2_font_helvR08_te);

        u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getStrWidth("H₂O : 0.00%"), y_start);
        u8g2.print("H₂O ");
        u8g2.print(H2O_fraction,2);
        u8g2.print("%");

        u8g2.setFont(u8g2_font_helvR18_te);

        y_gap = 26;
        x_gap = 46;

        u8g2.setCursor(0, y_gap * 1 + y_start);
        u8g2.print("O₂");
        u8g2.setCursor(x_gap, y_gap * 1 + y_start);
        u8g2.print(O2_fraction, 2);
        u8g2.print("%");

        u8g2.setCursor(0, y_gap * 2 + y_start);
        u8g2.print("He");
        u8g2.setCursor(x_gap, y_gap * 2 + y_start);
        u8g2.print(He_fraction, 2);
        u8g2.print("%");

        break;
    case 2: // Calibrate
        u8g2.setFont(u8g2_font_helvR12_te);

        y_gap = 18;

        break;
    case 3: // Environment
        u8g2.setFont(u8g2_font_helvR12_te);

        y_gap = 18;
        x_gap = 60;

        u8g2.setCursor(0, y_gap * 1 + y_start);
        u8g2.print("Temp");
        u8g2.setCursor(x_gap, y_gap * 1 + y_start);
        u8g2.print(temperature, 1);
        u8g2.print("°C");

        u8g2.setCursor(0, y_gap * 2 + y_start);
        u8g2.print("R.Hum");
        u8g2.setCursor(x_gap, y_gap * 2 + y_start);
        u8g2.print(relative_humidity, 1);
        u8g2.print("%");

        u8g2.setCursor(0, y_gap * 3 + y_start);
        u8g2.print("Press");
        u8g2.setCursor(x_gap, y_gap * 3 + y_start);
        u8g2.print(local_pressure, 1);
        u8g2.print(" hPa");

        // u8g2.drawUTF8(0, y_gap * 1 + y_start, "Temp: 21.7°C");
        // u8g2.drawUTF8(0, y_gap * 2 + y_start, "R.Hum: 51.1%");
        // u8g2.drawUTF8(0, y_gap * 3 + y_start, "Press: 10.1 hPa");

        break;
    case 4: // Raw Data
        u8g2.setFont(u8g2_font_helvR08_te);

        y_gap = 16;

        u8g2.drawVLine(u8g2.getDisplayWidth() / 2 - 2, y_start + 3, y_gap * 2);

        // u8g2.drawUTF8(0, y_gap * 1 + y_start, "O₂ : 12.2 mV");
        u8g2.setCursor(0, y_gap * 1 + y_start);
        u8g2.print("O₂ : ");
        u8g2.print(O2_millivolts, 1);
        u8g2.print(" mV");

        // u8g2.drawUTF8(u8g2.getDisplayWidth() / 2 + 2, y_gap * 1 + y_start, "Dur: 24.7 µs");
        u8g2.setCursor(u8g2.getDisplayWidth() / 2 + 2, y_gap * 1 + y_start);
        u8g2.print("Dur: ");
        u8g2.print(duration, 1);
        u8g2.print(" µs");

        u8g2.drawHLine(0, y_gap * 1 + y_start + 4, u8g2.getDisplayWidth());

        // u8g2.drawUTF8(0, y_gap * 2 + y_start, "T: 21.7°C");
        u8g2.setCursor(0, y_gap * 2 + y_start);
        u8g2.print("Tem:");
        u8g2.print(temperature, 1);
        u8g2.print("°C");

        // u8g2.drawUTF8(u8g2.getDisplayWidth() / 2 + 4, y_gap * 2 + y_start, "RH: 51.1%");
        u8g2.setCursor(u8g2.getDisplayWidth() / 2 + 2, y_gap * 2 + y_start);
        u8g2.print("RHu: ");
        u8g2.print(relative_humidity, 1);
        u8g2.print("%");

        u8g2.drawHLine(0, y_gap * 2 + y_start + 2, u8g2.getDisplayWidth());

        // u8g2.drawUTF8(0, y_gap * 3 + y_start, "Atm Press: 10.1 hPa");
        u8g2.setCursor(0, y_gap * 3 + y_start);
        u8g2.print("Atm Press: ");
        u8g2.print(local_pressure, 1);
        u8g2.print(" hPa");

        u8g2.drawHLine(0, y_gap * 3 + y_start + 4, u8g2.getDisplayWidth());

        break;
    }

    u8g2.sendBuffer();

    check_button_event();
}

void navigate_submenu()
{
    if (button_event == U8X8_MSG_GPIO_MENU_SELECT)
    {
        current_view = MAIN_MENU;
    }
    if (button_event > 0)
    {
        button_event = 0;
    }
}