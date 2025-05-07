# V's World
Plugin for VCV Rack.

# Manual

## X-Limit JI Autotuner
This module tunes voltages into just intonation based on the voltage given in the first channel of a polyphonic cable.
For this it uses a precalculated list of voltages and picks the closest value to given input voltages.

The math needed to understand the module is the following:

> Let $f_0$ be the starting frequency from which an allowed frequency $f$ can be derived by:
> 
> $f = f_0 * 2^{x_0} * 3^{x_1} * 5^{x_2} * 7^{x_3} * 11^{x_4} * 13^{x_5} * 17^{x_6} * 19^{x_7}$
>    
> Getting from frequency to voltage:
>
> $v = log_2(f / 261.625)$
>
> Which leads to the final expression:
>
> $v = v_0 + {x_0} * log_2(2) + {x_1} * log_2(3) + {x_2} * log_2(5) + {x_3} * log_2(7) + {x_4} * log_2(11) + {x_5} * log_2(13) + {x_6} * log_2(17) + {x_7} * log_2(19) $
>
> $v_0$ and $v$ are given by the voltage in the first and $i$ th input channel.
> The vector $\vec{x} = (x_0, x_1, x_2, ...)$ is called monzo.

The module calculates a list of allowed voltages given by the "Monzo range" knob values in the module. 
Each "Monzo" knob specifies the range of values $[-n_x, +n_x]$ for its corresponding prime number.
The list will be recalculated when a knob is turned, which can lead to high CPU load for large lists.
The module will stop working and a red light will show up, if the list gets too large.

The circle in the middle is the tuning circle (see image).
The outer lines show the allowed tunings within an octave.
The inner lines show the output tunings over all octaves.

The module allows for dynamic and static just intonation (see patch examples).
Dynamic just intonation is done by default.
Static just intonation can be achieved by inserting the voltage of the basekey into the first channel of the polyphonic input.
This module needs polyphonic input to work properly, since the first channel is always passed through.
Only the other channels are just intonated based on the first channel.

For more information, check out this article from Felix Roos: https://loophole-letters.vercel.app/5limit-just-intonation
Many presets are built from the lists in the article.

## X-Limit JI Autotuner 2
The Autotuner 2 module is an extension of the first module with more freedom. 

### How to build a tuning system
Lets try to build the most simple tuning system: the pythagorean tuning system.
The pythagorean tuning system builds its frequencies with the following formula: $$ f = f_0 * \frac{3^p}{2^q} $$ , where $p,q$ are natural numbers.
This means we need the Harmonics 2 and 3, with bounds $0 <= p < \inf$ and $0 <= q < \inf$. Since we cant handle infinities let's choose 20 as upper bound.
This corresponds to the following parameters:

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 2       | 1  | -20  | 0 |
| 3 | 1 | 0  | 20  |

We have now built the pythagorean tuning, where the upper octaves of the root note are pitch shifted by the pythagorean comma. To fix this, we can allow going up octaves by changing the bounds for the second harmonic:

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 2       | 1  | -20  | 10 |
| 3 | 1 | 0  | 20  |

So we are able to build octaveted and non-octaveted tuning systems.
Insted of octaveting, we can also shift the starting point of the pythagorean comma up and down. The following examples shift it down by 1 octave, once by reassigning bounds and once by directly shifting:

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 2       | 1  | -21  | -1 |
| 3 | 1 | 0  | 20  |

OR

| $a_n$ | $b_n$ | $s_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|-----|-----| --|
| 2       | 1 | -1 | -20  | 0 |
| 3 | 1 | 0 | 0  | 20  |

Any tuning system also has a subharmonic side which can be acessed by mirroring the lower and upper bounds of every harmonic in the following way: $[x_{min}, x_{max}] \rightarrow [-x_{max}, -x_{min}]$. Instead of this, it is also possible to just change the harmonics: $[a_n, b_n] \rightarrow [b_n, a_n]$. For the subharmonic pythagorean tuning system we get the following parameters:

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 2       | 1  | 0  | 20 |
| 3 | 1 | -20  | 0  |

OR

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 1 | 2 | -20  | 0 |
| 1 | 3 | 0  | 20  |

The pythagorean comma now appears when going down octaves instead of up. How to fix this should be clear by now.
All of these special pyhtagorean tuning system can be combined into a unified tuning system, where all subsystem can be found:

| $a_n$ | $b_n$ | $x_{min}$ | $x_{max}$ |
|----------|----------|----------| --|
| 2 | 1 | -20  | 20 |
| 3 | 1 | -20  | 20 |


## X-Limit JI VCO
VCO for the X-Limit JI Autotuner modules with a wavetable VCO to reduce floating point inaccuracies for minimal beating.

# Images
Light | Dark
:-------------------------:|:-------------------------:
![image](https://github.com/user-attachments/assets/01bec857-770c-4400-bad2-4a0b1ce062de) | ![image](https://github.com/user-attachments/assets/eec7b422-13fc-4270-b9ed-3976062abaca)
![image](https://github.com/user-attachments/assets/4cffd61c-d0e5-4cb2-bbae-cd3f695b852a) | ![image](https://github.com/user-attachments/assets/975297ee-607d-40c6-9e3a-e69d0d4e643e)
![image](https://github.com/user-attachments/assets/a52b8812-b058-4591-9617-433d74923c83) | ![image](https://github.com/user-attachments/assets/e980ce36-bba0-4941-8215-fb5a7951c1b2)


# Patch examples
## Static just intonation (left channel) vs Dynamic just intonation (right channel)

![image](https://github.com/user-attachments/assets/40f583b1-b0d4-41f8-8336-584b8bd0d0a5)


