#ifndef GASCALCS_H
#define GASCALCS_H

enum gas_type {
        back_gas,
        deco_gas
};

int MOD_O2_calculate(double O2_fraction, double O2_partial_pressure);
int MOD_density_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double temperature, double density);
int END_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double depth);
double min_gas_rec(double depth, double consumption_rate);

#endif