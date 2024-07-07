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
extern double temperature_k_last;
extern double O2_fraction_last;
extern double He_fraction_last;
extern double H2O_fraction_last;

#endif