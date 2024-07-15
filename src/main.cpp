// Lbraries
#include <Arduino.h>        // Basic Library
#include <math.h>           // Math Library
#include <Wire.h>           // I2C Library
#include <U8g2lib.h>        // Display Library
#include <MUIU8g2.h>        // Menu LIbrary
#include <BME280I2C.h>      // Temp/Hum/Pres sensor Library
#include <ADS1X15.h>        // ADC / Amplifier Library
#include <RunningAverage.h> // Running Average Library

// Custom Headers
#include "constants.h"   // Global Constants
#include "oxygen.h"      // oxygen calculations
#include "environment.h" // environmenttal parameters: temperature, humidity, atmospheric pressure
#include "helium.h"      // ultrasonic measurement and helium calculations

//setup display and mui menus
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
MUIU8G2 mui;

// Menu element struct definition
struct menu_entry_type {
    const uint8_t *font;
    uint16_t icon0;
    uint16_t icon1;
    const char *name;
};

// Menu state struct definition
struct menu_state {
    int16_t menu_start;     /* in pixel */
    int16_t frame_position; /* in pixel */
    uint8_t position;       /* position, array index */
};

enum View {
        MAIN_MENU,
        SUB_MENU
};

// Menu Icon configuration
#define ICON_WIDTH 40
#define ICON_HEIGHT 38
#define ICON_GAP 4
#define ICON_BGAP 5
#define ICON_Y 32 + ICON_GAP - 4
#define ICON_Y_Text ICON_Y
#define ICON_W_2nd_Letter 27
#define ELEMENTS 6 // number of elements in menu + null element

// Navigation Button pin configuration
#define NEXT_PIN 25
#define SELECT_PIN 26
#define PREV_PIN 27

// menu initial states
struct menu_state current_state = {ICON_BGAP, ICON_BGAP, 0};
struct menu_state destination_state = {ICON_BGAP, ICON_BGAP, 0};

//button and menu tracking
int8_t button_event = 0; // set this to 0, once the event has been processed
uint8_t calib_page_exit_code = 0;
uint32_t submenu_selected = 0;
uint8_t is_redraw = 1;
View current_view = MAIN_MENU;

// mui O2 calibration inputs
uint8_t O2_calibration_target_ten = 2;
uint8_t O2_calibration_target_one = 0;
uint8_t O2_calibration_target_dec = 9;

// mui distance calibration inputs
uint8_t dist_calibration_target_ten = 0;
uint8_t dist_calibration_target_one = 0;
uint8_t dist_calibration_target_dec = 0;

// Main Menu items
// {font, icon character 1, icon character 2, Title} second icon character used for O2 & He elements
struct menu_entry_type menu_entry_list[ELEMENTS] = {
        {u8g2_font_helvR24_te, 79, 8322, "Oxygen"}, // 79 = "O", 8322 = "₂"
        {u8g2_font_helvR24_te, 72, 101, "Trimix"},  // 72 = "H", 101 = "e"
        {u8g2_font_open_iconic_weather_4x_t, 64, 0, "Climate"},
        {u8g2_font_open_iconic_embedded_4x_t, 72, 0, "Calibrate"},
        {u8g2_font_open_iconic_embedded_4x_t, 70, 0, "Raw Data"},
        {NULL, 0, 0, NULL}
};



muif_t muif_list[] = {
        MUIF_U8G2_FONT_STYLE(0, u8g2_font_helvR12_te), /* regular font */
        MUIF_U8G2_FONT_STYLE(1, u8g2_font_helvR10_te), /* bold font */

        MUIF_U8G2_LABEL(),

        MUIF_U8G2_U8_MIN_MAX("CA", &O2_calibration_target_ten, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),
        MUIF_U8G2_U8_MIN_MAX("CB", &O2_calibration_target_one, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),
        MUIF_U8G2_U8_MIN_MAX("CC", &O2_calibration_target_dec, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),
        MUIF_U8G2_U8_MIN_MAX("DA", &dist_calibration_target_ten, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),
        MUIF_U8G2_U8_MIN_MAX("DB", &dist_calibration_target_one, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),
        MUIF_U8G2_U8_MIN_MAX("DC", &dist_calibration_target_dec, 0, 9, mui_u8g2_u8_min_max_wm_mse_pi),

        MUIF_BUTTON("BO", mui_u8g2_btn_goto_wm_fi),
        MUIF_BUTTON("BH", mui_u8g2_btn_goto_wm_fi),
        MUIF_BUTTON("BA", mui_u8g2_btn_goto_wm_fi),
        MUIF_VARIABLE("EX", &calib_page_exit_code, mui_u8g2_btn_exit_wm_fi)
};

fds_t fds_data[] =

        MUI_FORM(3)
        MUI_STYLE(0)
        MUI_XYAT("BO", 32, 34, 1, " O₂ ")
        MUI_XYAT("BH", 96, 34, 2, " He ")
        MUI_STYLE(1)
        MUI_XYAT("EX", 64, 58, 3, " exit ")

        MUI_FORM(1)
        MUI_STYLE(0)
        MUI_LABEL(0, 34, "O₂ ")
        MUI_XY("CA", 65, 34)
        MUI_XY("CB", 75, 34)
        MUI_LABEL(85, 34, ".")
        MUI_XY("CC", 95, 34)
        MUI_LABEL(105, 34, " %")
        MUI_STYLE(1)
        MUI_XYAT("EX", 32, 58, 1, " cal ")
        MUI_XYAT("BA", 96, 58, 3, " back ")

        MUI_FORM(2)
        MUI_STYLE(0)
        MUI_LABEL(0, 34, "He ")
        MUI_XY("DA", 65, 34)
        MUI_XY("DB", 75, 34)
        MUI_LABEL(85, 34, ".")
        MUI_XY("DC", 95, 34)
        MUI_LABEL(105, 34, " %")
        MUI_STYLE(1)
        MUI_XYAT("EX", 32, 58, 2, " cal ")
        MUI_XYAT("BA", 96, 58, 3, " back ");

// Functions
int menu_initialise();
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
void O2_calibrate_run_display();
void dist_calibrate_run_display();
void splash_screen();
void splash_screen_cal();
void submenu_draw();
void submenu_cases();


void setup()
{
        // start serial connection
        Serial.begin(115200);
        delay(1000); // Delay to stabilize serial communication

        // Initialise I2C
        Wire.begin();

        // buttons and display Initialise
        if (menu_initialise()) {
                Serial.println("Display Connected");
                splash_screen();
                delay(500);
        }

        // Temperature, Relative Humidity, and pressure Sensor Initialise
        if (Environment_Initialise()) {
                Serial.println("Environment Connected");
                delay(500);
        }

        // Oxygen sensor Initialise
        if (O2_Initialise()) {
                Serial.println("ADC Connected");
                delay(500);
        }

        // helium initialization
        He_Initialise();

        // splash_screen_cal();
        run_menu();
        Serial.println("Ready");
}

void loop()
{
        check_button_event();

        if (mui.isFormActive())
                submenu_draw();
        else 
                submenu_cases();
}

// Functions

int menu_initialise()
{
        if (!u8g2.begin(SELECT_PIN, NEXT_PIN, PREV_PIN)) {
                Serial.println("Failed to initialize display.");
                while (1)
                        ;
        }

        u8g2.enableUTF8Print();

        mui.begin(u8g2, fds_data, muif_list, sizeof(muif_list) / sizeof(muif_t));

        return 1;
}

void check_button_event()
{
        if (button_event == 0) 
                button_event = u8g2.getMenuEvent();
}

void draw_main(struct menu_state *state)
{
        int16_t x_position = state->menu_start;
        uint8_t menu_element = 0;
        
        while (menu_entry_list[menu_element].icon0 > 0) {
                if (menu_element >= -ICON_WIDTH && menu_element < u8g2.getDisplayWidth()) {

                        u8g2.setFont(menu_entry_list[menu_element].font);

                        int16_t x0_position;
                        int16_t y0_position;
                        int16_t x1_position;
                        int16_t y1_position;

                        switch (menu_element) {
                        case 0:
                                x0_position = x_position + 2;
                                y0_position = ICON_Y;
                                x1_position = x_position + ICON_W_2nd_Letter;
                                y1_position = ICON_Y - 3;

                                u8g2.drawGlyph(x0_position, y0_position, menu_entry_list[menu_element].icon0);
                                u8g2.drawGlyph(x1_position, y1_position, menu_entry_list[menu_element].icon1);

                                break;
                        case 1:
                                x0_position = x_position;
                                y0_position = ICON_Y;
                                x1_position = x_position + ICON_W_2nd_Letter - 6;
                                y1_position = ICON_Y;

                                u8g2.drawGlyph(x0_position, y0_position, menu_entry_list[menu_element].icon0);
                                u8g2.drawGlyph(x1_position, y1_position, menu_entry_list[menu_element].icon1);

                                break;
                        default:
                                x0_position = x_position + 4;
                                y0_position = ICON_Y + 4;

                                u8g2.drawGlyph(x0_position, y0_position, menu_entry_list[menu_element].icon0);

                                break;
                        }
                }

                menu_element++;
                x_position += ICON_WIDTH + ICON_GAP;

                check_button_event();
        }

        u8g2.drawFrame(state->frame_position - 1, ICON_Y - ICON_HEIGHT + 7, ICON_WIDTH + 2, ICON_WIDTH);
        u8g2.drawFrame(state->frame_position - 2, ICON_Y - ICON_HEIGHT + 6, ICON_WIDTH + 4, ICON_WIDTH + 2);

        check_button_event();
}

// new run_menu
void run_menu()
{
        do {
                u8g2.clearBuffer();
                draw_main(&current_state);

                if (destination_state.position != 0) {
                        u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
                        u8g2.drawGlyph(0, u8g2.getDisplayHeight(), 65); // left arrow
                }

                u8g2.setFont(u8g2_font_helvR12_te);
                u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width(menu_entry_list[destination_state.position].name)) / 2, u8g2.getDisplayHeight() - 5);
                u8g2.print(menu_entry_list[destination_state.position].name);

                if (destination_state.position != (ELEMENTS - 2)) {
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
                submenu_selected = destination_state.position + 1;

        if (button_event > 0) // all known events are processed, clear event
                button_event = 0;
}

void navigate_submenu()
{
        if (button_event == U8X8_MSG_GPIO_MENU_SELECT && submenu_selected > 0)
                submenu_selected = submenu_selected * 10;
        if (button_event > 0)
                button_event = 0;
}

// new run_submenu
void run_submenu()
{
        check_button_event();

        int y_start = 10;
        int y_gap = 18;
        int x_gap = 0;

        double temperature_k;
        double relative_humidity;
        double local_pressure_kPa;

        double O2_fraction;
        double He_fraction;
        double H2O_fraction;

        double speed_of_sound_m_s;
        double duration_us;
        double O2_mV;

        check_button_event();
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvR10_te);
        u8g2.drawUTF8(0, y_start + 2, menu_entry_list[destination_state.position].name);
        u8g2.drawHLine(0, y_start + 5, u8g2.getDisplayWidth());

        switch (submenu_selected) {
        // Oxygen
        case 1:
                O2_fraction = oxygen_measurement();
                H2O_fraction = water_measurement(); 
                O2_mV = oxygen_millivolts();
                temperature_k = temperature_measurement();
                He_fraction = 0;

                check_button_event();

                u8g2.setFont(u8g2_font_helvR08_te);

                u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getUTF8Width("H₂O 0.00 %"), y_start + 2);
                u8g2.print("H₂O ");
                u8g2.print(H2O_fraction * 100, 2);
                u8g2.print(" %");

                u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("Cell 00.00 mV")) / 2, u8g2.getDisplayHeight() - 4);
                u8g2.print("Cell ");
                u8g2.print(O2_mV, 1);
                u8g2.print(" mV");

                u8g2.setFont(u8g2_font_helvR24_te);

                if (O2_fraction < 1)
                        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("00.0 %")) / 2 + 2, u8g2.getDisplayHeight() - 19);
                else
                        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("100.0 %")) / 2 + 2, u8g2.getDisplayHeight() - 19);

                u8g2.print(O2_fraction * 100, 1);
                u8g2.print(" %");

                temperature_k_last = temperature_k;
                O2_fraction_last = O2_fraction;
                He_fraction_last = He_fraction;
                H2O_fraction_last = H2O_fraction;

                break;
        // Trimix
        case 2:
                temperature_k = temperature_measurement();
                speed_of_sound_m_s = speed_measurement();
                O2_fraction = oxygen_measurement();
                H2O_fraction = water_measurement();

                // calculate helium fraction
                He_fraction = helium_measurement(He_fraction_last, O2_fraction, H2O_fraction, speed_of_sound_m_s, temperature_k);

                check_button_event();

                u8g2.setFont(u8g2_font_helvR08_te);

                u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getUTF8Width("H₂O 0.00 %"), y_start + 2);
                u8g2.print("H₂O ");
                u8g2.print(H2O_fraction * 100, 2);
                u8g2.print(" %");

                u8g2.setFont(u8g2_font_helvR18_te);

                x_gap = 46;

                u8g2.setCursor(4, 38);
                u8g2.print("O₂");
                u8g2.setCursor(x_gap, 38);
                u8g2.print(O2_fraction * 100, 1);
                u8g2.print(" %");

                u8g2.setCursor(4, u8g2.getDisplayHeight());
                u8g2.print("He");
                u8g2.setCursor(x_gap, u8g2.getDisplayHeight());
                u8g2.print(He_fraction * 100, 1);
                u8g2.print(" %");

                temperature_k_last = temperature_k;
                O2_fraction_last = O2_fraction;
                He_fraction_last = He_fraction;
                H2O_fraction_last = H2O_fraction;

                break;
        // Environment
        case 3:
                temperature_k = temperature_measurement();
                relative_humidity = humidity_measurement();
                local_pressure_kPa = atmpressure_measurement();

                u8g2.setFont(u8g2_font_helvR12_te);

                x_gap = u8g2.getStrWidth("R.Hum") + 3;

                u8g2.setCursor(0, 31);
                u8g2.print("Temp");
                u8g2.setCursor(x_gap, 31);
                u8g2.print(temperature_k - 273.15, 1);
                u8g2.print(" °C");

                u8g2.setCursor(0, 47);
                u8g2.print("R.Hum");
                u8g2.setCursor(x_gap, 47);
                u8g2.print(relative_humidity * 100, 1);
                u8g2.print(" %");

                u8g2.setCursor(0, u8g2.getDisplayHeight());
                u8g2.print("Press");
                u8g2.setCursor(x_gap, u8g2.getDisplayHeight());
                u8g2.print(local_pressure_kPa, 1);
                u8g2.print(" kPa");

                break;
        // Calibrate
        case 4:
                u8g2.setFont(u8g2_font_helvR12_te);
                mui.gotoForm(3, 0);
                break;
        // Raw Data
        case 5:
                temperature_k = temperature_measurement();
                relative_humidity = humidity_measurement();
                local_pressure_kPa = atmpressure_measurement();
                duration_us = measure_duration() * 1000000;
                O2_mV = oxygen_millivolts();

                check_button_event();

                y_gap = 16;

                u8g2.drawVLine(u8g2.getDisplayWidth() / 2 - 2, y_start + 4, y_gap * 2 + 1);
                u8g2.drawHLine(0, y_gap * 1 + y_start + 4, u8g2.getDisplayWidth());
                u8g2.drawHLine(0, y_gap * 2 + y_start + 4, u8g2.getDisplayWidth());
                u8g2.drawHLine(0, y_gap * 3 + y_start + 4, u8g2.getDisplayWidth());

                u8g2.setFont(u8g2_font_helvR08_te);
                u8g2.setCursor(0, y_gap * 1 + y_start);
                u8g2.print("O₂ ");
                u8g2.print(O2_mV, 2);
                u8g2.print(" mV");

                u8g2.setCursor(u8g2.getDisplayWidth() / 2 + 2, y_gap * 1 + y_start);
                u8g2.print("Dur ");
                u8g2.print(duration_us, 1);
                u8g2.print(" µs");

                u8g2.setCursor(0, y_gap * 2 + y_start);
                u8g2.print("Tem ");
                u8g2.print(temperature_k - 273.15, 1);
                u8g2.print(" °C");

                u8g2.setCursor(u8g2.getDisplayWidth() / 2 + 2, y_gap * 2 + y_start);
                u8g2.print("RH ");
                u8g2.print(relative_humidity * 100, 1);
                u8g2.print(" %");

                u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("Atm Press 100.0 kPa")) / 2, y_gap * 3 + y_start);
                u8g2.print("Atm Press ");
                u8g2.print(local_pressure_kPa, 1);
                u8g2.print(" kPa");

                break;
        // MOD PO2 Screen
        case 20:
        case 10:
                check_button_event();

                u8g2.clearBuffer();
                u8g2.setFont(u8g2_font_helvB08_te);
                u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getUTF8Width("00 / 00 "), y_start + 2);
                u8g2.print(O2_fraction_last * 100, 0);
                u8g2.print(" / ");
                u8g2.print(He_fraction_last * 100, 0);

                u8g2.setFont(u8g2_font_helvR10_te);
                u8g2.drawUTF8(0, y_start + 2, "MOD pO₂");
                u8g2.drawHLine(0, y_start + 4, u8g2.getDisplayWidth());

                x_gap = u8g2.getDisplayWidth() - u8g2.getStrWidth("000 m");

                u8g2.setCursor(20, 31);
                u8g2.print("pO₂ 1.2");
                u8g2.setCursor(x_gap, 31);
                u8g2.print(MOD_O2_calculate(O2_fraction_last, 1.2));
                u8g2.print(" m");

                u8g2.setCursor(20, 45);
                u8g2.print("pO₂ 1.4");
                u8g2.setCursor(x_gap, 45);
                u8g2.print(MOD_O2_calculate(O2_fraction_last, 1.4));
                u8g2.print(" m");

                u8g2.setCursor(20, 60);
                u8g2.print("pO₂ 1.6");
                u8g2.setCursor(x_gap, 60);
                u8g2.print(MOD_O2_calculate(O2_fraction_last, 1.6));
                u8g2.print(" m");

                break;
        // MOD Density Screen
        case 200:
        case 100:
                check_button_event();

                u8g2.clearBuffer();
                u8g2.setFont(u8g2_font_helvB08_te);
                u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getUTF8Width("00 / 00 "), y_start + 2);
                u8g2.print(O2_fraction_last * 100, 0);
                u8g2.print(" / ");
                u8g2.print(He_fraction_last * 100, 0);

                u8g2.setFont(u8g2_font_helvR10_te);
                u8g2.drawUTF8(0, y_start + 2, "MOD Den");
                u8g2.drawHLine(0, y_start + 4, u8g2.getDisplayWidth());

                x_gap = u8g2.getDisplayWidth() - u8g2.getStrWidth("000 m");

                u8g2.setCursor(20, 31);
                u8g2.print("5.20 g/l");
                u8g2.setCursor(x_gap, 31);
                u8g2.print(MOD_density_calculate(He_fraction_last, O2_fraction_last, H2O_fraction_last, temperature_k_last, 5.2));
                u8g2.print(" m");

                u8g2.setCursor(20, 45);
                u8g2.print("5.75 g/l");
                u8g2.setCursor(x_gap, 45);
                u8g2.print(MOD_density_calculate(He_fraction_last, O2_fraction_last, H2O_fraction_last, temperature_k_last, 5.75));
                u8g2.print(" m");

                u8g2.setCursor(20, 60);
                u8g2.print("6.30 g/l");
                u8g2.setCursor(x_gap, 60);
                u8g2.print(MOD_density_calculate(He_fraction_last, O2_fraction_last, H2O_fraction_last, temperature_k_last, 6.3));
                u8g2.print(" m");
                
                break;
        default:
                submenu_selected = 0;
                break;
        }

        u8g2.sendBuffer();

        check_button_event();
}

void O2_calibrate_run_display()
{
        u8g2.clearBuffer();

        u8g2.setFont(u8g2_font_helvB10_te);
        u8g2.drawUTF8(0, 12, menu_entry_list[3].name);
        u8g2.drawHLine(0, 14, u8g2.getDisplayWidth());
        u8g2.setCursor(0, 30);
        u8g2.print("Calibrating O₂");
        u8g2.setCursor(0, 55);
        u8g2.print("O₂ Target ");
        u8g2.print(O2_cal_target * 100, 1);
        u8g2.print(" %");
        u8g2.sendBuffer();

        calibrate_oxygen();

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_te);
        u8g2.drawUTF8(0, 12, menu_entry_list[3].name);
        u8g2.drawHLine(0, 14, u8g2.getDisplayWidth());
        u8g2.setCursor(0, 30);
        u8g2.print("Complete");
        u8g2.setCursor(0, 55);
        u8g2.print("O₂ ");
        u8g2.print(oxygen_measurement() * 100, 1);
        u8g2.print(" %");
        u8g2.sendBuffer();

        delay(1500);
}

void dist_calibrate_run_display()
{
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_te);
        u8g2.drawUTF8(0, 12, menu_entry_list[3].name);
        u8g2.drawHLine(0, 14, u8g2.getDisplayWidth());
        u8g2.setCursor(0, 30);
        u8g2.print("Calibrating Dist");
        u8g2.setCursor(0, 55);
        u8g2.print("He ");
        u8g2.print(dist_calibration_target * 100, 1);
        u8g2.print(" %");
        u8g2.sendBuffer();

        calibrate_distance(dist_calibration_target, oxygen_measurement(), water_measurement());

        delay(2000);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_te);
        u8g2.drawUTF8(0, 12, menu_entry_list[3].name);
        u8g2.drawHLine(0, 14, u8g2.getDisplayWidth());
        u8g2.setCursor(0, 30);
        u8g2.print("Complete");
        u8g2.setCursor(0, 55);
        u8g2.print("Dist ");
        u8g2.print(distance_calibrated * 1000, 1);
        u8g2.print(" mm");
        u8g2.sendBuffer();

        delay(1500);
}

void splash_screen()
{
        int y_start = 33;

        u8g2.setFont(u8g2_font_helvR24_te);

        u8g2.clearBuffer();

        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width("HeRO")) / 2, y_start);
        u8g2.print("HeRO₂");

        u8g2.setFont(u8g2_font_helvR10_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("gas analyser")) / 2, y_start + 11);
        u8g2.print("gas analyser");

        u8g2.setFont(u8g2_font_helvR08_te);
        u8g2.setCursor(u8g2.getDisplayWidth() - u8g2.getStrWidth("by a. bill"), u8g2.getDisplayHeight() - 2);
        u8g2.print("by a. bill");

        u8g2.sendBuffer();
}

void splash_screen_cal()
{
        int y_start = 33;

        Serial.println("Calibrating O₂");

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvR24_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width("HeRO")) / 2, y_start);
        u8g2.print("HeRO₂");

        u8g2.setFont(u8g2_font_helvR10_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("gas analyser")) / 2, y_start + 11);
        u8g2.print("gas analyser");
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width("Calibrating O")) / 2, u8g2.getDisplayHeight() - 4);
        u8g2.print("Calibrating O₂");

        u8g2.sendBuffer();

        calibrate_oxygen();

        Serial.println("Calibrating Distance");

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvR24_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width("HeRO")) / 2, y_start);
        u8g2.print("HeRO₂");

        u8g2.setFont(u8g2_font_helvR10_te);
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getStrWidth("gas analyser")) / 2, y_start + 11);
        u8g2.print("gas analyser");
        u8g2.setCursor((u8g2.getDisplayWidth() - u8g2.getUTF8Width("Calibrating Dist.")) / 2, u8g2.getDisplayHeight() - 4);
        u8g2.print("Calibrating Dist.");

        u8g2.sendBuffer();

        delay(1500);

        calibrate_distance(dist_calibration_target, oxygen_measurement(), water_measurement());
}

// Menu redraws
void submenu_draw()
{
        if (is_redraw) {
                u8g2.clearBuffer();
                u8g2.setFont(u8g2_font_helvB10_te);
                u8g2.drawUTF8(0, 12, menu_entry_list[destination_state.position].name);
                u8g2.drawHLine(0, 14, u8g2.getDisplayWidth());
                mui.draw();
                u8g2.sendBuffer();
                is_redraw = 0;
        }
        switch (u8g2.getMenuEvent()) {
        case U8X8_MSG_GPIO_MENU_SELECT:
                mui.sendSelect();
                is_redraw = 1;
                break;
        case U8X8_MSG_GPIO_MENU_NEXT:
                mui.nextField();
                is_redraw = 1;
                break;
        case U8X8_MSG_GPIO_MENU_PREV:
                mui.prevField();
                is_redraw = 1;
                break;
        } 
}

void submenu_cases()
{
        switch (calib_page_exit_code) {
        case 1:
                O2_cal_target = (O2_calibration_target_ten * 0.1) + (O2_calibration_target_one * 0.01) + (O2_calibration_target_dec * 0.001);
                O2_calibrate_run_display();
                submenu_selected = 0;
                calib_page_exit_code = 0;
                button_event = 0;
                break;
        case 2:
                dist_calibration_target = (dist_calibration_target_ten * 0.1) + (dist_calibration_target_one * 0.01) + (dist_calibration_target_dec * 0.001);
                dist_calibrate_run_display();
                submenu_selected = 0;
                calib_page_exit_code = 0;
                button_event = 0;
                break;
        case 3:
                submenu_selected = 0;
                calib_page_exit_code = 0;
                button_event = 0;
                break;
        default:
                break;
        }

        if (submenu_selected) {
                run_submenu();
                navigate_submenu();
        } else 
                run_menu();
}


// Main Menu Navigation helper functions

void to_right(struct menu_state *state)
{
        if (menu_entry_list[state->position + 1].font != NULL) {
                if ((int16_t)state->frame_position + 2 * (int16_t)ICON_WIDTH + (int16_t)ICON_BGAP < (int16_t)u8g2.getDisplayWidth()) {
                        state->position++;
                        state->frame_position += ICON_WIDTH + (int16_t)ICON_GAP;
                } else {
                        state->position++;
                        state->frame_position = (int16_t)u8g2.getDisplayWidth() - (int16_t)ICON_WIDTH - (int16_t)ICON_BGAP;
                        state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);
                }
        }
}

void to_left(struct menu_state *state)
{
        if (state->position > 0) {
                if ((int16_t)state->frame_position >= (int16_t)ICON_BGAP + (int16_t)ICON_WIDTH + (int16_t)ICON_GAP) {
                        state->position--;
                        state->frame_position -= ICON_WIDTH + (int16_t)ICON_GAP;
                } else {
                        state->position--;
                        state->frame_position = ICON_BGAP;
                        state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);
                }
        }
}

uint8_t towards_int16(int16_t *current, int16_t dest)
{
        if (*current < dest) {
                (*current)++;
                return 1;
        } else if (*current > dest) {
                (*current)--;
                return 1;
        }

        return 0;
}

// new towards function
uint8_t towards(struct menu_state *current, struct menu_state *destination)
{
        uint8_t towards_rate = 0;
        int8_t count = 4;

        for (int8_t i = 0; i < count; i++)
                towards_rate |= towards_int16(&(current->frame_position), destination->frame_position);

        for (int8_t i = 0; i < count; i++) 
                towards_rate |= towards_int16(&(current->menu_start), destination->menu_start);

        return towards_rate;
}