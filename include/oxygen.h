#ifndef OXYGEN_H
#define OXYGEN_H

void calibrate_oxygen(double O2_Fraction);
double oxygen_measurement(void);
double oxygen_millivolts(void);
int O2_Initialise(void);
double oxygen_stddev(void);

#endif