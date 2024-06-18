// Paired Header
#include "constants.h"

// define your own namespace to hold constants
// Universal Gas constant, J/(mol.K)
extern constexpr double R_gas_constant{8.314462618};

// Adiabatic Indices
extern constexpr double He_adiabatic_index{1.666};
extern constexpr double O2_adiabatic_index{1.394};
extern constexpr double N2_adiabatic_index{1.399};
extern constexpr double H2O_adiabatic_index{1.33};

// Molar Masses, kg/mol
extern constexpr double He_molar_mass{0.00400};
extern constexpr double O2_molar_mass{0.03200};
extern constexpr double N2_molar_mass{0.02802};
extern constexpr double H2O_molar_mass{0.01802};

// calibration values
double O2_cal_target = 0.209;
double O2_calibration = 0.0;
double dist_calibration_target = 0.0;
double distance_calibrated = 0.0517;