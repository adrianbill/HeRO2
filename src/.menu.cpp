#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

struct menu_entry_type
{
    const uint8_t *font;
    uint16_t icon0;
    uint16_t icon1;
    const char *name;
};

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
#define Elements 7

struct menu_entry_type menu_entry_list[Elements] =
    {
        {u8g2_font_helvR24_te, 79, 8322, "Nitrox"},
        {u8g2_font_helvR24_te, 72, 101, "Trimix"},
        {u8g2_font_open_iconic_embedded_4x_t, 72, 0, "Calibration"},
        {u8g2_font_open_iconic_weather_4x_t, 64, 0, "Environment"},
        {u8g2_font_open_iconic_embedded_4x_t, 70, 0, "Raw Data"},
        {u8g2_font_open_iconic_embedded_4x_t, 66, 0, "Settings"},
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
void draw(struct menu_state *state);
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
    u8g2.begin(/*Select=*/26, /*Right/Next=*/25, /*Left/Prev=*/27);
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_helvR12_te);
}

void check_button_event()
{
    if (button_event == 0)
        button_event = u8g2.getMenuEvent();
}

void draw(struct menu_state *state)
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

            int16_t x0_adjusted = x;
            int16_t y0_adjusted = ICON_Y;
            int16_t x1_adjusted = 0;
            int16_t y1_adjusted = 0;

            switch (i)
            {
            case 0:
                x0_adjusted = x + 2;
                x1_adjusted = x + ICON_W_2nd_Letter;
                y1_adjusted = ICON_Y - 3;
                break;
            case 1:
                x1_adjusted = x + ICON_W_2nd_Letter - 6;
                y1_adjusted = ICON_Y;
                break;
            default:
                x0_adjusted = x + 4;
                y0_adjusted = ICON_Y + 4;
                break;
            }

            u8g2.drawGlyph(x0_adjusted, y0_adjusted, menu_entry_list[i].icon0);
            u8g2.drawGlyph(x1_adjusted, y1_adjusted, menu_entry_list[i].icon1);
        }
        i++;
        x += ICON_WIDTH + ICON_GAP;
        check_button_event();
    }
    u8g2.drawFrame(state->frame_position - 1, ICON_Y - ICON_HEIGHT + 7, ICON_WIDTH + 2, ICON_WIDTH);
    u8g2.drawFrame(state->frame_position - 2, ICON_Y - ICON_HEIGHT + 6, ICON_WIDTH + 4, ICON_WIDTH + 2);
    // u8g2.drawFrame(state->frame_position - 2, ICON_Y - ICON_HEIGHT - 2, ICON_WIDTH + 4, ICON_WIDTH + 4);
    // u8g2.drawFrame(state->frame_position - 3, ICON_Y - ICON_HEIGHT - 3, ICON_WIDTH + 6, ICON_WIDTH + 6);
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

uint8_t towards(struct menu_state *current, struct menu_state *destination)
{
    uint8_t r = 0;
    uint8_t i;
    for (i = 0; i < 6; i++)
    {
        r |= towards_int16(&(current->frame_position), destination->frame_position);
        r |= towards_int16(&(current->menu_start), destination->menu_start);
    }
    return r;
}

void run_menu()
{
    do
    {
        u8g2.firstPage();
        do
        {
            draw(&current_state);
            if (destination_state.position != 0)
            {
                u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
                u8g2.drawGlyph(0, u8g2.getDisplayHeight(), 65); // left arrow
            }
            u8g2.setFont(u8g2_font_helvB10_te);
            u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth(menu_entry_list[destination_state.position].name)) / 2, u8g2.getDisplayHeight() - 5);
            u8g2.print(menu_entry_list[destination_state.position].name);
            if (destination_state.position != (Elements - 2))
            {
                u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
                u8g2.drawGlyph(u8g2.getDisplayWidth() - 16, u8g2.getDisplayHeight(), 66); // right arrow
            }
            check_button_event();
            delay(10);
        } while (u8g2.nextPage());
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

void run_submenu()
{
    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_helvR08_te);
        u8g2.drawStr(0, 8, menu_entry_list[destination_state.position].name);

        draw_raw_data();

        u8g2.drawStr(0, 64, "Back");
        check_button_event();
    } while (u8g2.nextPage());
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

void draw_raw_data()
{
    u8g2.setFont(u8g2_font_unifont_te);
    u8g2.firstPage();
    do
    {

    // u8g2.setCursor(0, 24);
    u8g2.drawUTF8(0, 24, "Dur: 24.7 µs");	
    // u8g2.print("O2: 12.2 mV");
    // u8g2.print("Dur: 24.7 µs");
    u8g2.drawUTF8(0, 44, "Tem: 21.7°C");
    u8g2.drawStr(0, 54, "R.Hum: 51.1%");

    u8g2.drawStr(0, 64, "Back");
}