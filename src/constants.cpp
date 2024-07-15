// Paired Header
#include "constants.h"

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
double distance_calibrated;

// Save last sensor readings
double temperature_K_last = 0;
double O2_fraction_last = 0;
double He_fraction_last = 0;
double H2O_fraction_last = 0;

int eeprom_size = 512;
int eeprom_O2_address = 0;
int eeprom_dist_address = 256;

// Ultrasonic Sensor Setup
extern constexpr int trigPin{19}; // Single Trigger pin for both ultrasonic sensors
extern constexpr int echoPin0{18}; // Echo pin for first ultrasonic sensors
// extern constexpr int echoPin1{17}; // Echo pin for second ultrasonic sensors (not used in current version)

// Navigation Button pin configuration
extern constexpr int next_pin{25};
extern constexpr int select_pin{26};
extern constexpr int prev_pin{27};