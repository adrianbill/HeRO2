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


// function to trigger helium reading
double helium_measurement(double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature)
{

    double N2_fraction = 1.0 - O2_fraction - H2O_fraction; // Assume N2 makes up the rest
    double He_fraction = 0.0;
    double maHe_fraction = 1 - O2_fraction - H2O_fraction;

    // Iterate to solve for He fraction

    for (int i = 0; i < 1000; i++)
    {
        double N2_modified = N2_fraction - He_fraction;

        // Ensure He fraction stays within valid range
        // if (N2_modified < 0)
        // {
        //     He_fraction = N2_fraction; // Ensure the fraction does not go negative
        //     break;
        // }
        // else
        if (He_fraction > maHe_fraction)
        {
            He_fraction = maHe_fraction;
            break;
        }
        else if (He_fraction < 0)
        {
            He_fraction = 0;
            break;
        }

        double molar_mass_mix = calculate_molar_mass(He_fraction, O2_fraction, N2_modified, H2O_fraction);
        double adiabatic_index_mix = calculate_effective_gamma(He_fraction, O2_fraction, N2_modified, H2O_fraction);

        double speed_of_sound_calculated = calculate_speed_of_sound(adiabatic_index_mix, molar_mass_mix, temperature);

        double error = speed_of_sound_measured - speed_of_sound_calculated;

        if (abs(error) < 0.001)
        {
            break; // Converged to solution within tolerance
        }

        // Adjust He fraction for next iteration
        He_fraction += 0.0001 * error; // Simple proportional adjustment

        N2_fraction = 1.0 - He_fraction - O2_fraction - H2O_fraction;
    }

    return He_fraction;
}

// //  New function to trigger helium reading under development
// double helium_measurement(double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature)
// {

//     double N2_fraction = 1.0 - O2_fraction - H2O_fraction; // Assume N2 makes up the rest
//     double He_fraction = 0.0;
//     double maHe_fraction = 1 - O2_fraction - H2O_fraction;

//     // Iterate to solve for He fraction

//     do
//     {
        
//         double molar_mass_mix = calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
//         double adiabatic_index_mix = calculate_effective_gamma(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
//         double speed_of_sound_calculated = calculate_speed_of_sound(adiabatic_index_mix, molar_mass_mix, temperature);

//         double error = speed_of_sound_measured - speed_of_sound_calculated;

//     } while (abs(error) < 0.01);

//     for (int i = 0; i < 1000; i++)
//     {
//         double N2_modified = N2_fraction - He_fraction;

//         // Ensure He fraction stays within valid range
//         // if (N2_modified < 0)
//         // {
//         //     He_fraction = N2_fraction; // Ensure the fraction does not go negative
//         //     break;
//         // }
//         // else
//         if (He_fraction > maHe_fraction)
//         {
//             He_fraction = maHe_fraction;
//             break;
//         }
//         else if (He_fraction < 0)
//         {
//             He_fraction = 0;
//             break;
//         }

//         double speed_of_sound_calculated = calculate_speed_of_sound(adiabatic_index_mix, molar_mass_mix, temperature);

//         double error = speed_of_sound_measured - speed_of_sound_calculated;

//         if (abs(error) < 0.001)
//         {
//             break; // Converged to solution within tolerance
//         }

//         // Adjust He fraction for next iteration
//         He_fraction += 0.0001 * error; // Simple proportional adjustment

//         N2_fraction = 1.0 - He_fraction - O2_fraction - H2O_fraction;
//     }

//     return He_fraction;
// }