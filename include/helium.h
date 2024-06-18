#ifndef HELIUM_H
#define HELIUM_H

void He_Initialise();
double calculate_molar_mass(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction);
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction);
double calculate_speed_of_sound(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction, double temperature);
double helium_measurement(double He_fraction, double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature);

#endif