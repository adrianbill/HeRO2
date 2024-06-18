// Libraries
#include <Arduino.h>        // Basic Library
#include <math.h>           // Math Library
#include <RunningAverage.h> // Running Average Library

// Custom Headers
#include "constants.h"

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
double calculate_molar_mass(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
    return He_fraction * He_molar_mass + O2_fraction * O2_molar_mass + N2_fraction * N2_molar_mass + H2O_fraction * H2O_molar_mass;
}

// Function to calculate the effective adiabatic index (G_i=(gamma_i−1)^-1
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
    
    
    
    
    
    return (1 / (He_fraction * (1 / (He_adiabatic_index - 1)) + O2_fraction * (1 / (O2_adiabatic_index - 1)) + N2_fraction * (1 / (N2_adiabatic_index - 1)) + H2O_fraction * (1 / (H2O_adiabatic_index - 1)))) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double mix_adiabatic_index, double mix_molar_mass, double temperature)
{
    return sqrt(mix_adiabatic_index * R_gas_constant * temperature / mix_molar_mass);
}

//  New function to trigger helium reading under development
double helium_measurement(double He_fraction, double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature)
{

    double gain = 0.001;
    double threshold = 1.0;
    double error = 0.0;
    double N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;
    double He_fraction_max = 1.0 - O2_fraction - H2O_fraction;

    // Iterate to solve for He fraction

    do
    {
        double mix_molar_mass = calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double mix_adiabatic_index = calculate_adiabatic_index(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double speed_of_sound_calculated = calculate_speed_of_sound(mix_adiabatic_index, mix_molar_mass, temperature);

        error = speed_of_sound_measured - speed_of_sound_calculated;

        He_fraction = gain * error + He_fraction;

        // //Debudding serial monitor text
        // Serial.print("Error: ");
        // Serial.print(error, 2);
        // Serial.print(" | He: ");
        // Serial.print(He_fraction, 2);
        // Serial.print(" | meas: ");
        // Serial.print(speed_of_sound_measured, 1);
        // Serial.print(" | calc: ");
        // Serial.println(speed_of_sound_calculated, 1);

        // Serial.print("MM: ");
        // Serial.print(mix_molar_mass * 1000, 2);
        // Serial.print(" | gamma: ");
        // Serial.print(mix_adiabatic_index, 2);
        // Serial.print(" | calc: ");
        // Serial.println(speed_of_sound_calculated, 1);

        // delay(1000);
        // //end of debugging section

        if (He_fraction > He_fraction_max)
        {
            He_fraction = He_fraction_max;
            break;
        }
        else if (He_fraction < 0)
        {
            He_fraction = 0.0;
            break;
        }

        N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;

    } while (abs(error) > threshold);

    return He_fraction;
}