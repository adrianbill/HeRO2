# Background & Approach

## Assumptions

- Electro-galvanic sensor is linear (i.e. single point calibration)
- Gas composition for speed of sound calculations includes only:
  - Helium ($He$)
  - Oxygen ($O_2$)
  - Nitrogen ($N_2$)
  - Water ($H_2$)
- Carbon Monoxide ($CO$) levels will only be used as a safety measure.

## Water Fraction and Relative Humidity

The BME280 sensor used provides a measurement of relative humidity, for use in the gas fraction calculations this needs to be converted to the fraction of water in the gas, this is done using partial pressures.

### Relative Humidity

Relative humidity ($RH$) is the water fraction in a gas relative to the maximum possible humidity (saturation vapour pressure) at the same temperature.

### Saturation Vapour Pressure

The saturation vapour pressure ($p\_{sat}$) is the pressure at which water vapour is in thermodynamic equilibrium with its condensed state. Simply, it is the maximum partial pressure that water achieve at a given atmospheric temperature.

To estimate the saturation vapour pressure the Tetens equation is used ([Ref: wikipedia](https://en.wikipedia.org/wiki/Tetens_equation)). For temperatures above $0\degree C$ the Tetens equation is:

$p_{sat} = 0.61078 \cdot \exp \Large (\frac{\small 17.27 \cdot T}{\small T + 237.3})$, where, $T$ is the temperature in $\degree C$.

### Water Fraction

The water fraction can then be calculated using the saturation vapour pressure, relative humidity and total pressure ($p_{total}$ , typically the atmospheric pressure) by:

$x_{H_2O} = \frac{\normalsize p_{sat} \cdot RH}{\normalsize p_{total}}$

## Helium Calculation

### The speed of sound in gases

The speed of sound $c$ in an ideal gas can be calculated using the following formula:

$c = \sqrt{\Large \frac{{\large \gamma} R T}{M}}$

where:

-   $\large \gamma$ is the adiabatic index (heat capacity ratio),
-   $R$ is the universal gas constant $(8.314\ ^J/_{K\cdot mol})$,
-   $T$ is the absolute temperature $(K)$,
-   $M$ is the molar mass of the gas mixture $(kg/mol)$.

[Ref: University Physics I (OpenStax)](https://phys.libretexts.org/Bookshelves/University_Physics/University_Physics_(OpenStax)/Book%3A_University_Physics_I_-_Mechanics_Sound_Oscillations_and_Waves_(OpenStax)/17%3A_Sound/17.03%3A_Speed_of_Sound)

### Individual Gas Properties

The molecular composition of the gas contributes both as the molar mass ($M$), and their heat capacity ratio ($\large \gamma$).

### Adiabatic index (Heat capacity ratio)

For this application ideal gas law applies and the following can be used,

$\Large \gamma = \frac{C_p}{C_v}$

where:

-   $C_p$ is specific heat capacity at constant pressure $(^J/_{K\cdot mol})$
-   $C_v$ is specific heat capacity at constant volume $(^J/_{K\cdot mol})$

and

$C_v = C_p - R$

Since the adiabatic index for a gas is the ratio of the specific heats as indicated above, it depends upon the effective number of degrees of freedom $(f)$ in the molecular motion.

#### Monatomic gasses

For a monatomic gas like helium, $f=3$ and ${\large \gamma} = 5/3 \approx 1.66$.

#### Diatomic molecules

For diatomic molecules like $N_2$ and $O_2$, you include two degrees of rotational freedom, so $f=5$ and ${\large \gamma} = 7/5 = 1.4$. Since almost all of the atmosphere is nitrogen and oxygen, ${\large \gamma} \approx 1.4$ can be used for dry air in such calculations as the speed of sound.

#### Polyatomic molecules

For polyatomic molecules, there is evidence of contribution of vibrational degrees of freedom that would reduce the heat capacity ratio from the expected ${\large \gamma} = 4/3$. This includes molecules such as $CO_2$ and $H_2O$, and therefore relative humidity can effect the calculation of the speed of sound. For this reason calibration of the device will be done using dry gasses.

#### Important Values

The heat capacities, adiabatic indices, and molar masses for relevant gasses.

| $Gas$  |                  $C_p$ |                  $C_v$ | $\Large \gamma$ |             $M$ |
| ------ | ---------------------: | ---------------------: | --------------: | --------------: |
| -      | $\frac{J}{K\cdot mol}$ | $\frac{J}{K\cdot mol}$ |               - | $\frac{g}{mol}$ |
| $He$   |                  20.79 |                  12.48 |           1.666 |           4.003 |
| $O_2$  |                  29.38 |                  21.07 |           1.394 |          31.999 |
| $N_2$  |                  29.12 |                  20.81 |           1.399 |          28.013 |
| $H_2O$ |                  33.58 |                  25.27 |           1.329 |          18.015 |
| $CO$   |                  29.15 |                  20.84 |           1.399 |          28.010 |
| $CO_2$ |                  37.12 |                  28.81 |           1.288 |          44.009 |

[Ref: NIST Chemistry WebBook](https://webbook.nist.gov/chemistry/):

### Gas Mixtures

The speed of sound for a mixed gas uses the same formula as define above. The adiabatic index and molar masses for the mixture are defined as follows ([Ref: Löfqvist et al](https://www.diva-portal.org/smash/get/diva2:1003574/FULLTEXT01.pdf)):

#### Adiabatic index

The heat capacities of the gas mixture can be calculated using the weighted sum of the individual components. For a gas mixture with $i$ gasses:

$C_{p,mix} = \sum_{i}x_i C_{p,i}$

$C_{v,mix} = \sum_{i}x_i C_{v,i}$

where:

-   $x$ is the molar fraction of the individual gasses

The effective adiabatic index of the gas mixture can then be calculated using the same formula as that of a single gas.

$\gamma_{mix} = \frac{C_{p,mix}}{C_{v,mix}}$

This can be reconfigured to use the $\gamma$ values of the individual components:

$\gamma_{mix} = \frac{\normalsize \sum_{i}x_i \frac{\gamma_i}{\gamma_i - 1}}{\normalsize \sum_{i}x_i \frac{1}{\gamma_i - 1}} = \frac{\normalsize 1}{\normalsize \sum_{i}x_i \frac{1}{\gamma_i - 1}} + 1$

#### Molar Mass

The average molar mass of the gas mixture can be calculated using the weighted sum of the individual components. For a gas mixture with $i$ gasses:

$M_{mix} = \sum_{i}x_i M_i$

where:

-   $x$ is the molar fraction of the individual gasses

#### Speed of Sounds and Helium fraction

Using the above formulas the speed of sound for the mixture is calculated:  

$c = \sqrt{\frac{{\large \gamma_{mix}} R T}{M_{mix}}}$

However, as both $\large \gamma_{mix}$ and $M_{mix}$ depend on  the helium fraction ($x_{He}$), isolating for $x_{He}$ results in a non-linear equation. Solving for $x_{He}$ can be accomplished either iteratively or via numerical methods (e.g. Newton–Raphson method). 

### Logic

Calculating the helium (He) fraction in a gas mixture comprising helium, oxygen (O₂), nitrogen (N₂), and water (H₂O) using a measurement of the speed of sound, temperature, oxygen fraction, and water fraction.

#### 1. Define the gas mixture composition

-   $x_{He}$ be the mole fraction of helium.
-   $x_{O_2}$ be the mole fraction of oxygen (given).
-   $x_{N_2} = 1 - x_{He} - x_{O_2}$ be the mole fraction of nitrogen.

#### 2. Calculate average molar mass of the mixture

$M_{mix} = x_{He} M_{He} + x_{O_2} M_{O_2} + (1 - x_{He} - x_{O_2}) M_{N_2}$

#### 3. Calculate the effective adiabatic index

For ease of manipulating the calculation,

Let $G_i =  ({\large \gamma}_i-1)^{-1}$

$\gamma_{mix} = (x_{He} G_{He} + x_{O_2} G_{O_2} + (1 - x_{He} - x_{O_2}) G_{N_2})^{-1} + 1$

#### 4. Solve for $x_{He}$ using the speed of sound formula

An iterative approach will be taken using a proportional control method for adjusting estimates.

$c = \sqrt{\frac{{\large \gamma_{mix}} R T}{M_{mix}}}$

1. Assume $x_{He(initial)} = 0$
2. calculate nitrogen fraction: $x_{N_2} = 1 - x_{O_2} - x_{H_2O} - x_{He}$
3. Calculate the speed of sound
4. Measure the speed of sound
5. Use proportional control method ([Ref: wikipedia](https://en.wikipedia.org/wiki/Proportional_control)): 
   - $x_{He(new)} =  K_p \cdot err + x_{He(last)}$
   - $error = c_{calc} - c_{meas}$ (the difference between calculated and measured speeds)
   - $K_p$ is the proportional gain, a constant used to scale the error for the desired response rate ( for this application are value of 0.001 is used)
6. Check if error is within acceptable $error$ range (an acceptable $error$ of ±1 m/s was chosen). 
   - **If $error$ is inside the error threshold:**
     - accept the $x_{He}$ value
     - exit the loop
   - **if $error$ is outside the error threshold:** 
     - adjust $x_{He}$ using proportional method
     - Return to step 2
