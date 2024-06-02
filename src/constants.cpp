// Paired Header
#include "constants.h"

// define your own namespace to hold constants
      // Universal Gas constant, J/(mol.K)
    extern constexpr double R{8.314462618};

    // Atmospheric Pressure (kPa)
    extern constexpr double pres_atm{101.325};

    // Adiabatic Indices
    extern constexpr double gamma_He{1.666};
    extern constexpr double gamma_O2{1.394};
    extern constexpr double gamma_N2{1.399};
    extern constexpr double gamma_H2O{1.33};

    // Molar Masses, kg/mol
    extern constexpr double M_He{0.00400};
    extern constexpr double M_O2{0.03200};
    extern constexpr double M_N2{0.02802};
    extern constexpr double M_H2O{0.01802};
