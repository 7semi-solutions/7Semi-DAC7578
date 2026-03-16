/**
 * 7Semi DAC7578 Arduino Library
 *
 *   DAC7578_Basic_Example
 *
 * Device
 *   Texas Instruments DAC7578
 *   8-Channel 12-Bit I2C Digital-to-Analog Converter
 *
 * Description
 *   This example demonstrates basic DAC7578 usage including:
 *
 *   - initializing the DAC
 *   - writing voltages to channels
 *   - writing raw DAC values
 *   - sweeping DAC outputs
 *   - reading cached DAC values
 *   - using LDAC register
 *   - using clear code register
 *
 *
 * Hardware Connections
 *
 *   DAC7578        MCU (Arduino / ESP32)
 *   -------------------------------------
 *   VCC      ->    3.3V or 5V
 *   GND      ->    GND
 *   SDA      ->    SDA
 *   SCL      ->    SCL
 *   VOUT0    ->    Analog measurement node
 *
 *
 * Example Node Connections
 *
 *   A  ---->  Analog input (A0)
 *
 *   Used to observe DAC output voltage.
 *
 *   Example uses address: 0x4C
 */

#include <7Semi_DAC7578.h>

DAC7578_7Semi dac;



void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("DAC7578 Example Starting...");

    /**
     * Initialize DAC
     *
     * - Starts I2C
     */

    if(!dac.begin(0x4C))
    {
        Serial.println("DAC7578 not detected!");
        while(1);
    }

    /**
     * Configure reference voltage
     *
     * - Used for voltage conversion
     */

    dac.setVref(3.3);

    Serial.println("DAC Initialized");

    delay(1000);


    /**
     * Sweep raw DAC values
     *
     * - Iterates through each channel
     * - Steps raw values across DAC range
     */

    for(uint8_t ch = CH0; ch <= CH7; ch++)
    {
        for(uint16_t val = 0; val <= 4095; val += 512)
        {
            dac.writeChannelRaw((Channel)ch, val);

            Serial.print("Channel ");
            Serial.print(ch);
            Serial.print(" raw value = ");
            Serial.println(val);

            delay(100);
        }
    }


    /**
     * Write voltages to individual channels
     */

    dac.writeChannel(CH0, 2.5);
    dac.writeChannel(CH1, 1.0);


    /**
     * Demonstrate register operations
     *
     * - LDAC register
     * - Clear code register
     */

    dac.writeLDAC(1.2);
    dac.writeClearCode(0.0);
}



void loop()
{
    /**
     * Sweep DAC voltage on channel 0
     */

    for(float v = 0.0; v <= 3.3; v += 0.3)
    {
        dac.writeChannel(CH0, v);

        Serial.print("Channel 0 voltage = ");
        Serial.print(v);

        Serial.print("  Analog voltage CH0 = ");
        Serial.println(analogRead(A0)*3.3/1024);

        delay(200);
    }

    uint16_t raw;

    delay(1000);

    /**
     * Write same voltage to all DAC channels
     */

    dac.writeAll(2.0);

    delay(2000);
}