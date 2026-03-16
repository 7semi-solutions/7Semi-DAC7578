#ifndef _7SEMI_DAC7578_H_
#define _7SEMI_DAC7578_H_

#include <Arduino.h>
#include <Wire.h>

/**
 * 7Semi DAC7578 Driver
 *
 * - Driver for Texas Instruments DAC7578
 * - 8-channel 12-bit I2C Digital-to-Analog Converter
 *
 * Device capabilities
 * - 8 independent DAC outputs
 * - 12-bit resolution
 * - I2C communication interface
 * - programmable power-down modes
 * - synchronous update using LDAC
 *
 * Library features
 * - voltage based output control
 * - raw DAC value control
 * - individual or simultaneous channel update
 * - power-down control per channel
 * - clear code register support
 * - LDAC register access
 * - internal cache for fast readback
 *
 * Designed for
 * - Arduino
 * - ESP32
 * - other Wire-compatible platforms
 */

 /**
 * DAC7578 Command Codes
 *
 * - Defines all device commands
 * - Each command controls a specific DAC register
 */

#define CMD_WRITE_INPUT          0x00
#define CMD_UPDATE_DAC           0x10
#define CMD_WRITE_UPDATE_ALL     0x20
#define CMD_WRITE_UPDATE         0x30
#define CMD_POWERDOWN            0x40
#define CMD_CLEAR_CODE           0x50
#define CMD_WRITE_LDAC           0x60
#define CMD_SOFT_RESET           0x70


/**
 * DAC Channel Selection
 *
 * - CH0–CH7 represent the eight DAC outputs
 * - CH_ALL is used for broadcast commands
 * - Broadcast commands update all channels simultaneously
 */
enum Channel
{
    CH0 = 0,
    CH1,
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7,

    /**
     * Broadcast channel selector
     *
     * - Used by device commands that operate on all channels
     */
    CH_ALL = 0x0F
};



/**
 * DAC Power-Down Modes
 *
 * - Controls how DAC output behaves when powered down
 */
enum PowerMode
{
    /**
     * Output connected to GND through 1kΩ
     *
     * - Fast discharge
     * - Useful when output must quickly return to zero
     */
    POWER_1K = 0,

    /**
     * Output connected to GND through 100kΩ
     *
     * - Weak pull-down
     * - Lower current consumption
     */
    POWER_100K,

    /**
     * Output becomes high impedance
     *
     * - Output amplifier disabled
     * - Output pin floats
     */
    POWER_HIGHZ
};



/**
 * DAC7578 Driver Class
 *
 * - Provides high-level control of DAC7578 device
 * - Handles I2C communication and DAC register formatting
 * - Includes internal caching for fast readback
 */
class DAC7578_7Semi
{
public:

    DAC7578_7Semi();



    /**
     * Initialize DAC device
     *
     * - Initializes I2C communication
     * - Configures I2C clock speed
     * - Verifies device presence using I2C acknowledge
     *
     * Parameters
     *
     * - i2cAddress : DAC I2C address
     * - i2cPort    : Wire interface instance
     * - i2cClock   : I2C clock speed
     * - sda / scl  : custom I2C pins (ESP32 only)
     *
     * Return
     *
     * - true  → device detected
     * - false → device not responding
     */
    bool begin(uint8_t i2cAddress = 0x4C,
               TwoWire &i2cPort = Wire,
               uint32_t i2cClock = 400000,
               uint8_t sda = 255,
               uint8_t scl = 255);



    /* =========================================================
       Reference Voltage Control
       ========================================================= */

    /**
     * Set DAC reference voltage
     *
     * - Used for voltage to DAC conversion
     * - Should match actual DAC reference voltage
     */
    void setVref(float v);

    /**
     * Get configured reference voltage
     */
    float getVref();



    /* =========================================================
       Channel Write Operations
       ========================================================= */

    /**
     * Write voltage to DAC channel
     *
     * - Converts voltage to raw DAC value
     * - Updates DAC output immediately
     */
    bool writeChannel(Channel ch, float voltage);

    /**
     * Write raw value to DAC channel
     *
     * - Raw value range : 0 – 4095
     * - Updates DAC output immediately
     */
    bool writeChannelRaw(Channel ch, uint16_t value);

    /**
     * Write voltage to DAC channel
     *
     * - Helper function using voltage conversion
     */
    bool writeVoltage(Channel ch, float voltage);

    /**
     * Set DAC input register without updating output
     *
     * - Output is updated later using update command
     */
    bool setChannel(Channel ch, float voltage);

    /**
     * Set raw value in DAC input register
     *
     * - Output not updated until update command
     */
    bool setChannelRaw(Channel ch, uint16_t value);

    /**
     * Read cached voltage of channel
     */
    bool getChannel(Channel ch, float &voltage);

    /**
     * Read cached raw DAC value
     *
     * Note
     *
     * - DAC7578 does not reliably support register readback
     * - Library returns cached value instead
     */
    bool getChannelRaw(Channel ch, uint16_t &value);



    /* =========================================================
       Update Control
       ========================================================= */

    /**
     * Update single DAC channel
     *
     * - Transfers input register to output register
     */
    bool updateChannel(Channel ch);

    /**
     * Update all DAC channels simultaneously
     */
    bool updateAll();



    /* =========================================================
       Write All Channels
       ========================================================= */

    /**
     * Write same voltage to all channels
     */
    bool writeAll(float voltage);

    /**
     * Write same raw value to all channels
     */
    bool writeAllRaw(uint16_t value);



    /* =========================================================
       Clear Code Register
       ========================================================= */

    /**
     * Write clear code voltage
     */
    bool writeClearCode(float voltage);

    /**
     * Read clear code voltage
     */
    bool readClearCode(float &voltage);

    /**
     * Write raw clear code value
     */
    bool writeClearCodeRaw(uint16_t value);

    /**
     * Read raw clear code value
     */
    bool readClearCodeRaw(uint16_t &value);



    /* =========================================================
       LDAC Register
       ========================================================= */

    /**
     * Write LDAC register voltage
     */
    bool writeLDAC(float voltage);

    /**
     * Read LDAC register voltage
     */
    bool readLDAC(float &voltage);

    /**
     * Write raw LDAC register value
     */
    bool writeRawLDAC(uint16_t value);

    /**
     * Read raw LDAC register value
     */
    bool readRawLDAC(uint16_t &value);



    /* =========================================================
       Power Control
       ========================================================= */

    /**
     * Power down DAC channel
     *
     * - Output amplifier disabled
     * - Output connected according to selected power mode
     */
    bool powerDown(Channel ch, PowerMode mode);



    /* =========================================================
       Device Control
       ========================================================= */

    /**
     * Soft reset device
     *
     * - Resets DAC registers to default state
     */
    bool softReset();



    /* =========================================================
       Low-Level Access
       ========================================================= */

    /**
     * Read DAC register value
     */
    uint16_t readChannel(Channel ch);

    /**
     * Get cached DAC value
     */
    uint16_t getCache(Channel ch);



private:

    /**
     * Low-level I2C helpers
     */
    bool writeReg(uint8_t cmd, uint16_t value);
    bool readReg(uint8_t cmd, uint16_t &value);



    /**
     * Conversion helpers
     *
     * - Converts voltage ↔ raw DAC values
     */
    uint16_t voltageToRaw(float voltage);
    float rawToVoltage(uint16_t raw);



    /**
     * Channel validation helper
     *
     * - Prevents invalid channel access
     * - Protects against out-of-range channel numbers
     */
    bool validChannel(Channel ch);



private:

    /**
     * Device I2C address
     */
    uint8_t address;

    /**
     * I2C interface pointer
     */
    TwoWire *i2c;

    /**
     * Reference voltage used for conversion
     */
    float vref;

    /**
     * Channel cache
     *
     * - Stores last written DAC value
     * - Enables fast readback
     */
    uint16_t cache[8];
};

#endif