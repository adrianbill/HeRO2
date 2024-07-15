#ifndef OXYGEN_H
#define OXYGEN_H

void calibrate_oxygen(void);
double oxygen_measurement(void);
double oxygen_millivolts(void);
int O2_Initialise(void);
int MOD_O2_calculate(double O2_fraction, double O2_partial_pressure);
int MOD_density_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double temperature, double density);

#endif