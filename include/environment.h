#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

void Environment_Initialise();
double temperature_measurement();
double humidity_measurement();
double atmpressure_measurement();
double water_measurement(double, double, double);

#endif