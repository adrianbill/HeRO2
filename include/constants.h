#ifndef CONSTANTS_H
#define CONSTANTS_H

extern const double code_version;

// Universal Gas constant, J/(mol.K)
extern const double R_gas_constant;

// Adiabatic Indices
extern const double He_adiabatic_index;
extern const double O2_adiabatic_index;
extern const double N2_adiabatic_index;
extern const double H2O_adiabatic_index;

// Molar Masses, kg/mol
extern const double He_molar_mass;
extern const double O2_molar_mass;
extern const double N2_molar_mass;
extern const double H2O_molar_mass;

// calibration values
extern double O2_cal_target;
extern double O2_calibration;
extern double dist_calibration_target;
extern double distance_calibration;
extern const double dist_actual;
extern double temperature_calibration;
void load_calibration_values(void);

// Save last sensor readings
extern double temperature_K_last;
extern double O2_fraction_last;
extern double He_fraction_last;
extern double H2O_fraction_last;
extern double He_spd;
extern double He_error;

//eeprom parameters for saving values across restarts
extern int eeprom_size;
extern int eeprom_O2_address;
extern int eeprom_dist_address;
extern int eeprom_temp_address;

// I2C Pins
extern const int sda_pin;
extern const int scl_pin;

// Ultrasonic Sensor Setup
extern const int trigPin; // Single Trigger pin for both ultrasonic sensors
extern const int echoPin0; // Echo pin for first ultrasonic sensors
// extern const int echoPin1; // Echo pin for second ultrasonic sensors (not used in current version)

// Navigation Button pin configuration
extern const int next_pin;
extern const int select_pin;
extern const int prev_pin;

//time checks
// extern unsigned long currentMillis;
extern unsigned long previousMillis_dur;
extern unsigned long deltaMillis_dur;
extern unsigned long previousMillis_dur_raw;
extern unsigned long deltaMillis_dur_raw;
extern const int ping_delay;

//He Toggle
extern const int toggle_He;

#endif