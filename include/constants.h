#ifndef CONSTANTS_H
#define CONSTANTS_H

// define your own namespace to hold constants
namespace constants
{
    // Universal Gas constant, J/(mol.K)
    extern const double R;

    // Atmospheric Pressure (kPa)
    extern const double pres_atm;

    // Adiabatic Indices
    extern const double gamma_He;
    extern const double gamma_O2;
    extern const double gamma_N2;
    extern const double gamma_H2O;

    // Molar Masses, kg/mol
    extern const double M_He;
    extern const double M_O2;
    extern const double M_N2;
    extern const double M_H2O;
}

#endif