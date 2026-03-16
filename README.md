# 7Semi DAC7578 Arduino Library

Arduino driver for the **DAC7578 8-channel 12-bit digital-to-analog converter (DAC)**.

The **DAC7578** from Texas Instruments is an **8-channel 12-bit DAC with an I²C interface** designed for applications such as signal generation, analog control systems, industrial automation, and embedded instrumentation.

This library provides a simple interface for writing **analog voltages or raw DAC values** to each channel and supports advanced device features such as **synchronous updates, LDAC control, clear code register configuration, and power-down modes**.

---

# Features

* 8-channel 12-bit DAC output
* Voltage output control
* Raw DAC value control
* Simultaneous multi-channel update
* Channel cache readback
* Clear code register control
* LDAC register configuration
* Channel power-down modes
* Soft reset support

---

# Supported Platforms

* Arduino UNO / Mega
* ESP32
* Any board supporting the **Wire (I²C) library**

---

# Hardware

Supported device:

**7Semi 8-Channel 12-bit I²C DAC Breakout – DAC7578**

---

# Connection

The **DAC7578 communicates using I²C**.

## I²C Connection

| DAC7578 Pin | MCU Pin   | Description  |
| ----------- | --------- | ------------ |
| VDD         | 3.3V / 5V | Device power |
| GND         | GND       | Ground       |
| SCL         | SCL       | I²C clock    |
| SDA         | SDA       | I²C data     |

### I²C Notes

Typical DAC address:

```cpp
0x4C
```

Possible address range:

```cpp
0x48 – 0x4F
```

Recommended I²C speed:

```cpp
100 kHz – 400 kHz
```

---

# Installation

## Arduino Library Manager

1. Open **Arduino IDE**
2. Go to **Library Manager**
3. Search for **7Semi DAC7578**
4. Click **Install**

---

## Manual Installation

1. Download this repository as ZIP
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

# Example

```cpp
#include <Wire.h>
#include "7Semi_DAC7578.h"

DAC7578 dac(0x4C);

void setup()
{
  Serial.begin(115200);

  if(!dac.begin(Wire))
  {
    Serial.println("DAC7578 not detected");
    while(1);
  }

  dac.setVref(3.3);

  dac.writeChannel(CH0, 2.0);
  dac.writeChannel(CH1, 1.0);
}

void loop()
{
  static float v = 0;

  dac.writeChannel(CH0, v);

  Serial.print("Voltage: ");
  Serial.println(v);

  v += 0.2;

  if(v > 3.3)
    v = 0;

  delay(300);
}
```

---

# Library Overview

## Writing Voltage to a Channel

```cpp
dac.writeChannel(CH0, 2.5);
```

Writes **2.5 V to DAC channel 0**.

---

## Writing Raw DAC Value

```cpp
dac.writeChannelRaw(CH0, 2048);
```

Writes **mid-scale output** to channel 0.

Raw DAC range:

```cpp
0 – 4095
```

---

## Writing Same Voltage to All Channels

```cpp
dac.writeAll(1.65);
```

Sets **all DAC channels to 1.65 V**.

---

## Updating a Single Channel

```cpp
dac.updateChannel(CH2);
```

Updates a **specific channel output** after writing to the input register.

---

## Updating All Channels

```cpp
dac.updateAll();
```

Updates **all DAC outputs simultaneously**.

Useful for **synchronous multi-channel signals**.

---

# Device Control

## Power-Down Mode

Each channel can be placed in **low-power mode**.

```cpp
dac.powerDown(CH0, 2);
```

Power-down modes:

```cpp
0 → 1kΩ to GND
1 → 100kΩ to GND
2 → High-Z output
```

---

## LDAC Register

LDAC allows **synchronous DAC output updates**.

```cpp
dac.writeLDAC(1.2);
```

---

## Clear Code Register

Defines the output value used during **clear operations**.

```cpp
dac.writeClearCode(0.0);
```

---

## Soft Reset

Resets DAC registers to default state.

```cpp
dac.softReset();
```

---

## Example Applications

Typical applications include:

* Analog signal generation
* Programmable voltage outputs
* Industrial control systems
* DAC waveform generation
* Sensor calibration systems
* Multi-channel analog control

---

## License

### MIT License

---

## Author

### 7Semi
