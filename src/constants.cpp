// Custom Headers
#include <EEPROM.h>

// Paired Header
#include "constants.h"

extern constexpr double code_version{0.93};

// Universal Gas constant, J/(mol.K)
extern constexpr double R_gas_constant{8.314462618};

// Adiabatic Indices
extern constexpr double He_adiabatic_index{1.666};
extern constexpr double O2_adiabatic_index{1.394};
extern constexpr double N2_adiabatic_index{1.399};
extern constexpr double H2O_adiabatic_index{1.33};

// Molar Masses, kg/mol
extern constexpr double He_molar_mass{0.00400};
extern constexpr double O2_molar_mass{0.03200};
extern constexpr double N2_molar_mass{0.02802};
extern constexpr double H2O_molar_mass{0.01802};

// calibration values
double O2_cal_target = 0.209;
double O2_calibration;
double dist_calibration_target = 0.0;
double distance_calibration;
extern constexpr double dist_actual{553.0};
double temperature_calibration = EEPROM.readDouble(eeprom_temp_address);
void load_calibration_values(void)
{      
        O2_calibration = EEPROM.readDouble(eeprom_O2_address);
        distance_calibration = EEPROM.readDouble(eeprom_dist_address);
        temperature_calibration = EEPROM.readDouble(eeprom_temp_address);
}

// Save last sensor readings
double temperature_K_last = 0;
double O2_fraction_last = 0;
double He_fraction_last = 0;
double H2O_fraction_last = 0;
double He_spd;
double He_error;

//eeprom parameters for saving values across restarts
int eeprom_size = 1024;
int eeprom_O2_address = 0;
int eeprom_dist_address = 512;
int eeprom_temp_address = 768;

// I2C Pins
// O2 Proto for John
extern constexpr int sda_pin{9};
extern constexpr int scl_pin{8};
// He Prototype
// extern constexpr int sda_pin{8};
// extern constexpr int scl_pin{9};

// Ultrasonic Sensor Setup
extern constexpr int trigPin{11}; // Single Trigger pin for both ultrasonic sensors
extern constexpr int echoPin0{10}; // Echo pin for first ultrasonic sensors
// extern constexpr int echoPin1{16}; // Echo pin for second ultrasonic sensors (not used in current version)

// Navigation Button pin configuration
// O2 Proto for John
extern constexpr int next_pin{5};
extern constexpr int select_pin{6};
extern constexpr int prev_pin{7};
// He Prototype
// extern constexpr int next_pin{1};
// extern constexpr int select_pin{4};
// extern constexpr int prev_pin{6};

//time checks
// unsigned long currentMillis;
unsigned long previousMillis_dur = 0;
unsigned long deltaMillis_dur;
unsigned long previousMillis_dur_raw = 0;
unsigned long deltaMillis_dur_raw;
extern constexpr int ping_delay{125};

//He Toggle
extern constexpr int toggle_He{1};

