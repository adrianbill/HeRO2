// Libraries
#include <Arduino.h>        // Basic Library
#include <math.h>           // Math Library
#include <RunningAverage.h> // Running Average Library

// Custom Headers
#include "constants.h"
#include "speed_of_sound.h"

// Paired Header
#include "helium.h"

// Running Average Setup
RunningAverage RA_He(10);

// helium initialization
void He_Initialise()
{
    RA_He.clear();
}

// function to trigger helium reading
double measure_helium(double x_O2, double x_H2O, double c_mea, double T)
{

    double x_N2 = 1.0 - x_O2 - x_H2O; // Assume N2 makes up the rest
    double x_He = 0.0;
    double max_He = 1 - x_O2 - x_H2O;

    // Iterate to solve for He fraction

    for (int i = 0; i < 1000; i++)
    {
        double N2_mod = x_N2 - x_He;

        // Ensure He fraction stays within valid range
        if (N2_mod < 0)
        {
            x_He = x_N2; // Ensure the fraction does not go negative
            break;
        }
        else if (x_He > max_He)
        {
            x_He = max_He;
            break;
        }
        else if (x_He < 0)
        {
            x_He = 0;
            break;
        }

        double M_mix = calculate_molar_mass(x_He, x_O2, N2_mod, x_H2O);
        double gamma_mix = calculate_effective_gamma(x_He, x_O2, N2_mod, x_H2O);

        double c_calc = calculate_speed_of_sound(gamma_mix, M_mix, T);

        double error = c_mea - c_calc;

        if (abs(error) < 0.001)
        {
            break; // Converged to solution within tolerance
        }

        // Adjust He fraction for next iteration
        x_He += 0.0001 * error; // Simple proportional adjustment

        x_N2 = 1.0 - x_He - x_O2 - x_H2O;
    }

    return x_He;
}