// Libraries
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <math.h>	    // Math Library

// Custom Headers
#include "constants.h"

// Paired Header
#include "helium.h"

// Running Average Setup
RunningAverage RA_He(100);

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

// Function to calculate the effective adiabatic index (tmp=(γ−1)^-1
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
        double He_tmp = (1 / (He_adiabatic_index - 1));
        double O2_tmp = (1 / (O2_adiabatic_index - 1));
        double N2_tmp = (1 / (N2_adiabatic_index - 1));
        double H2O_tmp = (1 / (H2O_adiabatic_index - 1));

        return (1 / (He_fraction * He_tmp + O2_fraction * O2_tmp + N2_fraction * N2_tmp + H2O_fraction * H2O_tmp)) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction, double temperature)
{
        double mix_molar_mass = calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double mix_adiabatic_index = calculate_adiabatic_index(He_fraction, O2_fraction, N2_fraction, H2O_fraction);

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

        do {
                double speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature);

                error = speed_of_sound_measured - speed_of_sound_calculated;

                He_fraction = gain * error + He_fraction;

                if (He_fraction > He_fraction_max) {
                        He_fraction = He_fraction_max;
                        N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;
                        break;
                } else if (He_fraction < 0) {
                        He_fraction = 0.0;
                        N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;
                        break;
                }

        } while (abs(error) > threshold);

        RA_He.addValue(He_fraction);

        return RA_He.getAverage();
}