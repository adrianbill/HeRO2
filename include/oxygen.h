#ifndef OXYGEN_H
#define OXYGEN_H

void calibrate_oxygen();
double oxygen_measurement();
double oxygen_millivolts();
int O2_Initialise();
int MOD_O2_calculate(double O2_fraction, double O2_partial_pressure);
int MOD_density_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double temperature, double density);

#endif