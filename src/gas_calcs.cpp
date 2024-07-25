// Libraries
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <math.h>	    // Math Library

// Custom Headers
#include "constants.h"
#include "helium.h"

// Paired Header
#include "gas_calcs.h"

int MOD_O2_calculate(double O2_fraction, double O2_partial_pressure)
{
        return round(10 * (O2_partial_pressure / O2_fraction - 1));
}

int MOD_density_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double temperature, double density)
{
        double N2_fraction = 1 - (He_fraction + O2_fraction + H2O_fraction);
        double mix_molar_mass_g = 1000 * calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double pressure_kPa = (R_gas_constant * temperature * density) / mix_molar_mass_g;
        return round(0.1 * pressure_kPa - 10);
}

int END_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double depth)
{
        double N2_fraction = 1 - He_fraction - O2_fraction - H2O_fraction;
        return round((depth + 10) * N2_fraction - 10);
}

double min_gas_rec(double depth, double consumption_rate)
{
        double average_depth_bar = (0.1 * depth + 1.01325) / 2;
        double time = 3 * depth + 1;             
        double min_gas = consumption_rate * average_depth_bar * time;

        if (min_gas < 40.0)
        {
                return 40.0;
        }

        return min_gas;
}