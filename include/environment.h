#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

int Environment_Initialise(void);
double temperature_measurement(void);
double temperature_measurement_raw(void);
void calibrate_temperature(int sign, double offset);
double humidity_measurement(void);
double humidity_measurement_raw(void);
double atmpressure_measurement(void);
double atmpressure_measurement_raw(void);
double water_measurement(void);

#endif