#ifndef OXYGEN_H
#define OXYGEN_H

void calibrate_oxygen();
double oxygen_measurement();
double oxygen_millivolts();
int O2_Initialise();
double MOD_calculate(double O2_fraction, double O2_partial_pressure);

#endif