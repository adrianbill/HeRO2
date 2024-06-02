// Libraries
#include <Arduino.h>        // Basic Library
#include <math.h>           // Math Library
#include <RunningAverage.h> // Running Average Library

// Custom Headers
#include "constants.h"

// Paired Header
#include "speed_of_sound.h"

// Function to calculate the molar mass of the mixture
double calculate_molar_mass(double x_He, double x_O2, double x_N2, double x_H2O)
{
    return x_He * M_He + x_O2 * M_O2 + x_N2 * M_N2 + x_H2O * M_H2O;
}

// Function to calculate the effective adiabatic index (G_i=(gamma_i−1)^-1
double calculate_effective_gamma(double x_He, double x_O2, double x_N2, double x_H2O)
{
    double G_He = 1 / (gamma_He - 1);
    double G_O2 = 1 / (gamma_O2 - 1);
    double G_N2 = 1 / (gamma_N2 - 1);
    double G_H2O = 1 / (gamma_H2O - 1);
    double G_mix = x_He * G_He + x_O2 * G_O2 + x_N2 * G_N2 + x_H2O * G_H2O;
    return (1 / G_mix) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double gamma_mix, double M_mix, double T)
{
    return sqrt(gamma_mix * R * T / M_mix);
}
