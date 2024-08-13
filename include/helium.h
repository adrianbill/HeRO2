#ifndef HELIUM_H
#define HELIUM_H

int He_Initialise(void);
double calculate_molar_mass(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction);
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction);
double calculate_speed_of_sound(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction, double temperature);
double measure_duration(void);
void calibrate_distance(double He_fraction);
double speed_measurement(void);
double helium_measurement(double He_fraction, double O2_fraction_, double H2O_fraction_, double speed_of_sound_measured_, double temperature_k);
double helium_stddev(void);

#endif