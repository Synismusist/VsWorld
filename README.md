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

The module calculates a list of allowed voltages given by the "max power" knob values in the module. 
Each "max power" knob specifies the range of values $[-x, +x]$ for its corresponding prime number.
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

The module initializes with 5-limit just intonation. The corresponding $(x_1,x_2,...)$ knob values for 5-limit are the following: $(6,2,1,0,0,...)$. The module uses $(10,2,1,0,0,...)$ in case voltages over multiple octaves are inserted. For 3-limit just intionation $(14,6,0,0,...)$ can be used.

For more information, check out this article from Felix Roos: https://loophole-letters.vercel.app/5limit-just-intonation

## X-Limit JI VCO
VCO for the X-Limit JI Autotuner modules with a wavetable VCO to reduce floating point inaccuracies for minimal beating.

# Images
Light | Dark
:-------------------------:|:-------------------------:
<img src="https://github.com/user-attachments/assets/01bec857-770c-4400-bad2-4a0b1ce062de" style="height: 100%; max-height: 500px; width: auto;"> | <img src="https://github.com/user-attachments/assets/eec7b422-13fc-4270-b9ed-3976062abaca" style="height: 100%; max-height: 500px; width: auto;">
<img src="https://github.com/user-attachments/assets/4cffd61c-d0e5-4cb2-bbae-cd3f695b852a" style="height: 100%; max-height: 500px; width: auto;"> | <img src="https://github.com/user-attachments/assets/975297ee-607d-40c6-9e3a-e69d0d4e643e" style="height: 100%; max-height: 500px; width: auto;">
<img src="https://github.com/user-attachments/assets/a52b8812-b058-4591-9617-433d74923c83" style="height: 100%; max-height: 500px; width: auto;"> | <img src="https://github.com/user-attachments/assets/e980ce36-bba0-4941-8215-fb5a7951c1b2" style="height: 100%; max-height: 500px; width: auto;">

<!-- Images -->
<div class="image-row">
  <img id="first-image" src="https://github.com/user-attachments/assets/01bec857-770c-4400-bad2-4a0b1ce062de">
  <img class="other-images" src="https://github.com/user-attachments/assets/eec7b422-13fc-4270-b9ed-3976062abaca">
</div>
<div class="image-row">
  <img class="other-images" src="https://github.com/user-attachments/assets/4cffd61c-d0e5-4cb2-bbae-cd3f695b852a">
  <img class="other-images" src="https://github.com/user-attachments/assets/975297ee-607d-40c6-9e3a-e69d0d4e643e">
</div>
<div class="image-row">
  <img class="other-images" src="https://github.com/user-attachments/assets/a52b8812-b058-4591-9617-433d74923c83">
  <img class="other-images" src="https://github.com/user-attachments/assets/e980ce36-bba0-4941-8215-fb5a7951c1b2">
</div>

<script>
  // Get the height of the first image
  const firstImage = document.getElementById('first-image');
  const firstImageHeight = firstImage.height;

  // Apply the height dynamically to other images
  const otherImages = document.querySelectorAll('.other-images');
  otherImages.forEach(img => {
    img.style.maxHeight = `${firstImageHeight}px`;
  });
</script>

<style>
  /* Optional styling for layout */
  .image-row {
    display: flex;
    gap: 10px;
  }
</style>




# Parameter Examples

## X-Limit JI Autotuner

### 3-Limit: 
| Knob 1   | Knob 2   | 
|----------|----------|
| 14        | 6  |  

### 5-Limit: 
| Knob 1   | Knob 2   | Knob 3   |
|----------|----------|----------|
| 10       | 2  | 1  | 

## X-Limit JI Autotuner 2

### 3 - Limit (congruent to Autotuner):
| Knob 1   | Knob 2   | Knob 3   | Knob 4 |
|----------|----------|----------| --|
| 2        | 1  | -14  | 14 |
| 3 | 1 | -6  | 6  |

### 5 - Limit (congruent to Autotuner):
| Knob 1 | Knob 2 | Knob 3 | Knob 4 |
|----------|----------|----------|--|
| 2 | 1 | -10 | 10 |
| 3 | 1 | -2  | 2  |
| 5 | 1 | -1  | 1  |

### Aloboi's Just Scale:
https://www.patreon.com/Aloboi/shop/alobend-only-available-for-logic-pro-x-748315
| Knob 1 | Knob 2 | Knob 3 | Knob 4 |
|----------|----------|----------|--|
| 2 | 1 | -1 | 1 |
| 17 | 16 | 0  | 1  |
| 9 | 8 | 0  | 1  |
| 19 | 16 | 0  | 1  |
| 5 | 4 | 0  | 1  |
| 21 | 16 | 0  | 1  |

# Patch examples
## Static just intonation (left channel) vs Dynamic just intonation (right channel)

![image](https://github.com/user-attachments/assets/40f583b1-b0d4-41f8-8336-584b8bd0d0a5)


