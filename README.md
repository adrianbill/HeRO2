# HeRO<sub>2</sub>
Gas analyser with other extravagant features to complicated and simply divering

## Introduction

### Objectives

 1. To measure the oxygen and helium fractions in a gas mixture for scuba diving.
 2. Detect Carbon monoxide levels as a safety precaution

### Basic Methods
 - Oxygen fraction will be measured using an electro-galvanic sensor.
 - Helium fraction will be measured using an ultrasonic sensor via speed of sound calculations.

### Assumptions
 - Gas composition for speed of sound calculations includes only:
    - Helium ($He$)
    - Oxygen ($O_2$)
    - Nitrogen ($N_2$)
 - relative humidity will be measured but it assumed that all gasses are dry.
 - Carbon Monoxide ($CO$) levels will only be used as a safety measure. 

## Helium Calculation

### The speed of sound in gases

The speed of sound $c$ in a gas is:

$c = \sqrt{\Large \frac{{\large \gamma} R T}{M}}$

where:
- $\large \gamma$ is the adiabatic index (heat capacity ratio),
- $R$ is the universal gas constant $(8.314\ ^J/_{K\cdot mol})$,
- $T$ is the absolute temperature $(K)$,
- $M$ is the molar mass of the gas mixture $(K)$.

### Individual Gas Properties

The molecular composition of the gas contributes both as the molar mass ($M$), and their heat capacity ratio ($\large \gamma$).

### Adiabatic index (Heat capacity ratio)

For this application ideal gas law applies and the following can be used,

$\Large \gamma = \frac{C_p}{C_v}$

where:
- $C_p$ is specific heat capacity at constant pressure $(^J/_{K\cdot mol})$
- $C_v$ is specific heat capacity at constant volume $(^J/_{K\cdot mol})$

and

$C_v = C_p - R$

Since the adiabatic index for a gas is the ratio of the specific heats as indicated above, it depends upon the effective number of degrees of freedom $(f)$ in the molecular motion.

#### Monoatomic gasses
For a monoatomic gas like helium, $f=3\ $ and $\ {\large \gamma} = 5/3 \approx 1.66$. 

#### Diatomic molecules
For diatomic molecules like $N_2\ $ and $O_2$, you include two degrees of rotational freedom, so $f=5\ $ and $\ {\large \gamma} = 7/5 = 1.4$. Since almost all of the atmosphere is nitrogen and oxygen, $\ {\large \gamma} approx 1.4\ $ can be used for dry air in such calculations as the speed of sound. 

#### Polyatomic molecules
For polyatomic molecules, there is evidence of contribution of vibrational degrees of freedom that would reduce the heat capacity ratio from the expected $\ {\large \gamma} = 4/3$. This includes molecules such as $\ CO_2\ $ and $\ H_2O$, and therefore relative humidity can effect the calculation of the speed of sound. For this reason calibration of the device will be done using dry gasses.

#### Important Values
The heat capacities, adiabatic indices, and molar masses for relevant gasses.

| $Gas$  | $C_p$                | $C_v$                | $\Large \gamma$ | $M$ |
|--------|---------------------:|---------------------:|----------------:|----:|
| -      |$\frac{J}{K\cdot mol}$|$\frac{J}{K\cdot mol}$|     -           |$\frac{g}{mol}$|
| $He$   | 20.79                | 12.48                |     1.666       |     4.003     |
| $O_2$  | 29.38                | 21.07                |     1.394       |    31.999     |
| $N_2$  | 29.12                | 20.81                |     1.399       |    28.013     |
| $H_2O$ | 33.58                | 25.27                |     1.329       |    18.015     |
| $CO$   | 29.15                | 20.84                |     1.399       |    28.010     |
| $CO_2$ | 37.12                | 28.81                |     1.288       |    44.009     |

[Ref: NIST Chemistry WebBook](https://webbook.nist.gov/chemistry/):


### Gas Mixtures [Ref](https://physics.stackexchange.com/questions/173976/speed-of-sound-of-a-gas-mixture)

The speed of sound for a mized gas uses the same formula as define above. The adiabatic index and molar masses for the mixture are defined as follows:

#### Adiabatic index 

The heat capacities of the gas mixture can be calculated using the weighted sum of the individual components. For a gas mixture with $i$ gasses:

$C_{p,mix} = \sum_{i}x_i C_{p,i}$

$C_{v,mix} = \sum_{i}x_i C_{v,i}$

where:

- $x$ is the molar fraction of the individual gasses

The effective adiabatic index of the gas mixture can then be calculated using the same formula as that of a single gas.

${\Large \gamma}_{mix} = \frac{C_{p,mix}}{C_{v,mix}}$

This can be reconfigured to use the $\gamma$ values of the individual components:

${\Large \gamma}_{mix} = \frac{\normalsize \sum_{i}x_i \frac{\gamma_i}{\gamma_i - 1}}{\normalsize \sum_{i}x_i \frac{1}{\gamma_i - 1}} = \frac{\normalsize1}{\normalsize \sum_{i}x_i \frac{1}{\gamma_i - 1}} + 1$

#### Molar Mass

The average molar mass of the gas mixture can be calculated using the weighted sum of the individual components. For a gas mixture with $i$ gasses:

$M_{mix} = \sum_{i}x_i M_i$

where:

- $x$ is the molar fraction of the individual gasses

### Logic

To calculate the helium (He) fraction in a gas mixture comprising helium, oxygen (O₂), and nitrogen (N₂) using the speed of sound and temperature, given the oxygen fraction, follow these steps:

#### 1. Define the gas mixture composition

- $x_{He}$ be the mole fraction of helium.
- $x_{O_2}$ be the mole fraction of oxygen (given).
- $x_{N_2} = 1 - x_{He} - x_{O_2}$ be the mole fraction of nitrogen.

#### 2. Calculate average molar mass of the mixture

$M_{mix} = x_{He} M_{He} + x_{O_2} M_{O_2} + (1 - x_{He} - x_{O_2}) M_{N_2}$

#### 3. Calculate the effective adiabatic index

Let $G_i =  ({\large \gamma}_i-1)^{-1}$

${\Large \gamma}_{mix} = \frac{\normalsize1}{\normalsize x_{He} G_{He} + x_{O_2} G_{O_2} + (1 - x_{He} - x_{O_2}) G_{N_2}} + 1$

#### 4. Use the speed of sound formula

Rearrange the speed of sound formula to solve for the molar mass of the mixture:

$M = {\Large \frac{{\large \gamma} R T}{c^2}}$

#### 5. Set up the equation to solve for $x_{He}$

By equating the expression for $M$ from the speed of sound with the expression for the average molar mass of the mixture, we get:

${\Large \frac{{\large\gamma}_{\text{mix}} R T}{v^2}} = x_{He} M_{He} + x_{O_2} M_{O_2} + (1 - x_{He} - x_{O_2}) M_{N_2}$

This equation contains the unknown $x_{He}$. Since $\gamma_{\text{mix}}$ also depends on $x_{He}$, this is a non-linear equation. It can be solved iteratively or using numerical methods to find the value of $x_{He}$.

