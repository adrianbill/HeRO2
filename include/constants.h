#ifndef CONSTANTS_H
#define CONSTANTS_H

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
extern double distance_calibrated;

// Save last sensor readings
extern double temperature_K_last;
extern double O2_fraction_last;
extern double He_fraction_last;
extern double H2O_fraction_last;

extern int eeprom_size;
extern int eeprom_O2_address;
extern int eeprom_dist_address;

// Ultrasonic Sensor Setup
extern const int trigPin; // Single Trigger pin for both ultrasonic sensors
extern const int echoPin0; // Echo pin for first ultrasonic sensors
// extern const int echoPin1; // Echo pin for second ultrasonic sensors (not used in current version)

// Navigation Button pin configuration
extern const int next_pin;
extern const int select_pin;
extern const int prev_pin;

#endif