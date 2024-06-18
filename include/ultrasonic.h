#ifndef ULTRASONIC_H
#define ULTRASONIC_H

double measure_duration();
void calibrate_distance(double He_fraction, double O2_fraction, double H2O_fraction);
double speed_measurement();
int ultrasonic_Initialise();

#endif