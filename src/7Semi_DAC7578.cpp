#include "7Semi_DAC7578.h"

/**
 * DAC7578 Constructor
 *
 * - Initializes DAC address
 * - Sets default reference voltage
 * - Clears internal channel cache
 */
DAC7578_7Semi::DAC7578_7Semi()
{
    vref = 5.0f;
    i2c = nullptr;

    for(int i=0;i<8;i++)
        cache[i] = 0;
}



/**
 * Initialize DAC device
 *
 * - Initializes I2C interface
 * - Configures I2C clock
 * - Verifies device presence
 *
 * - Return
 *   - true  → device detected
 *   - false → I2C device not found
 */
bool DAC7578_7Semi::begin(uint8_t i2cAddress,
                           TwoWire &i2cPort,
                           uint32_t i2cClock,
                           uint8_t sda,
                           uint8_t scl)
{
    i2c = &i2cPort;
    address = i2cAddress;

#ifdef ARDUINO_ARCH_ESP32
    if (sda != 255 && scl != 255)
        i2c->begin(sda, scl);
    else
        i2c->begin();
#else
    (void)sda;
    (void)scl;
    i2c->begin();
#endif

    i2c->setClock(i2cClock);

    /**
     * Probe device address
     */

    i2c->beginTransmission(address);

    if (i2c->endTransmission() != 0)
        return false;

    return true;
}



/**
 * Set reference voltage
 *
 * - Used for voltage conversion
 */
void DAC7578_7Semi::setVref(float v)
{
    if(v > 0.01f)
        vref = v;
}



/**
 * Get reference voltage
 */
float DAC7578_7Semi::getVref()
{
    return vref;
}



// ============================================================
// Voltage / Raw Channel Writes
// ============================================================



/**
 * Write DAC channel using voltage
 *
 * - Converts voltage to raw DAC value
 * - Updates output immediately
 */
bool DAC7578_7Semi::writeChannel(Channel ch, float voltage)
{
    if(!validChannel(ch))
        return false;

    uint16_t value = voltageToRaw(voltage);

    uint8_t cmd = CMD_WRITE_UPDATE | ch;

    if(!writeReg(cmd,value))
        return false;

    cache[ch] = value;

    return true;
}



/**
 * Write DAC channel using raw value
 *
 * - Raw range: 0 – 4095
 */
bool DAC7578_7Semi::writeChannelRaw(Channel ch, uint16_t value)
{
    if(!validChannel(ch))
        return false;

    value &= 0x0FFF;

    uint8_t cmd = CMD_WRITE_UPDATE | ch;

    if(!writeReg(cmd,value))
        return false;

    cache[ch] = value;

    return true;
}



/**
 * Write voltage to DAC channel
 */
bool DAC7578_7Semi::writeVoltage(Channel ch, float voltage)
{
    if(!validChannel(ch))
        return false;

    uint16_t val = voltageToRaw(voltage);

    return writeChannelRaw(ch,val);
}



/**
 * Write value to DAC input register
 *
 * - Output is NOT updated until update command
 */
bool DAC7578_7Semi::setChannelRaw(Channel ch, uint16_t value)
{
    if(!validChannel(ch))
        return false;

    value &= 0x0FFF;

    uint8_t cmd = CMD_WRITE_INPUT | ch;

    if(!writeReg(cmd,value))
        return false;

    cache[ch] = value;

    return true;
}



/**
 * Read cached DAC value
 *
 * - DAC7578 does not reliably support readback
 * - Returns cached value instead
 */
bool DAC7578_7Semi::getChannelRaw(Channel ch, uint16_t &value)
{
    if(!validChannel(ch))
        return false;

    value = cache[ch];

    return true;
}



/**
 * Set channel voltage without updating output
 */
bool DAC7578_7Semi::setChannel(Channel ch, float voltage)
{
    if(!validChannel(ch))
        return false;

    uint16_t raw = voltageToRaw(voltage);

    uint8_t cmd = CMD_WRITE_INPUT | ch;

    if(!writeReg(cmd,raw))
        return false;

    cache[ch] = raw;

    return true;
}



/**
 * Get cached channel voltage
 */
bool DAC7578_7Semi::getChannel(Channel ch, float &voltage)
{
    uint16_t raw;

    if(!getChannelRaw(ch, raw))
        return false;

    voltage = rawToVoltage(raw);

    return true;
}



// ============================================================
// Conversion Helpers
// ============================================================



/**
 * Convert raw DAC value to voltage
 */
float DAC7578_7Semi::rawToVoltage(uint16_t raw)
{
    raw &= 0x0FFF;

    if(vref <= 0.001f)
        return 0;

    return (raw / 4095.0f) * vref;
}



/**
 * Convert voltage to raw DAC value
 */
uint16_t DAC7578_7Semi::voltageToRaw(float voltage)
{
    if(vref <= 0.001f)
        return 0;

    if(voltage < 0)
        voltage = 0;

    if(voltage > vref)
        voltage = vref;

    return (uint16_t)((voltage / vref) * 4095.0f + 0.5f);
}



// ============================================================
// Update Operations
// ============================================================



/**
 * Update single DAC channel
 *
 * - Transfers input register to output register
 */
bool DAC7578_7Semi::updateChannel(Channel ch)
{
    if(!validChannel(ch))
        return false;

    uint8_t cmd = CMD_UPDATE_DAC | ch;

    return writeReg(cmd,0);
}



/**
 * Update all DAC channels
 */
bool DAC7578_7Semi::updateAll()
{
    uint8_t cmd = CMD_UPDATE_DAC | CH_ALL;

    return writeReg(cmd,0);
}



/**
 * Write same raw value to all DAC channels
 */
bool DAC7578_7Semi::writeAllRaw(uint16_t value)
{
    value &= 0x0FFF;

    uint8_t cmd = CMD_WRITE_UPDATE | CH_ALL;

    if(!writeReg(cmd,value))
        return false;

    for(int i=0;i<8;i++)
        cache[i] = value;

    return true;
}



/**
 * Write same voltage to all DAC channels
 */
bool DAC7578_7Semi::writeAll(float voltage)
{
    uint16_t raw = voltageToRaw(voltage);

    return writeAllRaw(raw);
}



// ============================================================
// Clear Code
// ============================================================



bool DAC7578_7Semi::writeClearCodeRaw(uint16_t value)
{
    value &= 0x0FFF;

    return writeReg(CMD_CLEAR_CODE,value);
}



bool DAC7578_7Semi::readClearCodeRaw(uint16_t &value)
{
    return readReg(CMD_CLEAR_CODE,value);
}



bool DAC7578_7Semi::writeClearCode(float voltage)
{
    return writeReg(CMD_CLEAR_CODE, voltageToRaw(voltage));
}



bool DAC7578_7Semi::readClearCode(float &voltage)
{
    uint16_t raw = 0;

    if(!readReg(CMD_CLEAR_CODE, raw))
        return false;

    voltage = rawToVoltage(raw);

    return true;
}



// ============================================================
// LDAC Register
// ============================================================



bool DAC7578_7Semi::writeRawLDAC(uint16_t value)
{
    value &= 0x0FFF;

    return writeReg(CMD_WRITE_LDAC,value);
}



bool DAC7578_7Semi::readRawLDAC(uint16_t &value)
{
    return readReg(CMD_WRITE_LDAC,value);
}



bool DAC7578_7Semi::writeLDAC(float voltage)
{
    return writeReg(CMD_WRITE_LDAC, voltageToRaw(voltage));
}



bool DAC7578_7Semi::readLDAC(float &voltage)
{
    uint16_t raw = 0;

    if(!readReg(CMD_WRITE_LDAC, raw))
        return false;

    voltage = rawToVoltage(raw);

    return true;
}



// ============================================================
// Power / Reset
// ============================================================



/**
 * Power down DAC channel
 *
 * Power modes
 * - 1kΩ to GND
 * - 100kΩ to GND
 * - High-Z
 */
bool DAC7578_7Semi::powerDown(Channel ch, PowerMode mode)
{
    if(!validChannel(ch))
        return false;

    uint16_t val = ((uint16_t)(mode & 0x03) << 8);

    val |= (1 << ch);

    return writeReg(CMD_POWERDOWN, val);
}



/**
 * Soft reset DAC
 */
bool DAC7578_7Semi::softReset()
{
    return writeReg(CMD_SOFT_RESET,0x00);
}



// ============================================================
// Low-Level I2C Access
// ============================================================



bool DAC7578_7Semi::readReg(uint8_t cmd, uint16_t &value)
{
    if(!i2c)
        return false;

    i2c->beginTransmission(address);
    i2c->write(cmd);

    if(i2c->endTransmission(false) != 0)
        return false;

    if(i2c->requestFrom(address,(uint8_t)2) != 2)
        return false;

    uint8_t msb = i2c->read();
    uint8_t lsb = i2c->read();

    value = (((uint16_t)msb << 4) | (lsb >> 4)) & 0x0FFF;

    return true;
}



uint16_t DAC7578_7Semi::readChannel(Channel ch)
{
    uint16_t val = 0;

    if(readReg(CMD_WRITE_INPUT | ch,val))
        return val;

    return 0;
}



uint16_t DAC7578_7Semi::getCache(Channel ch)
{
    if(!validChannel(ch))
        return 0;

    return cache[ch];
}



/**
 * Write register via I2C
 *
 * - Converts 12-bit DAC value into device format
 */
bool DAC7578_7Semi::writeReg(uint8_t cmd, uint16_t value)
{
    if(!i2c)
        return false;

    value &= 0x0FFF;

    uint8_t msb = value >> 4;
    uint8_t lsb = (value & 0x0F) << 4;

    i2c->beginTransmission(address);

    i2c->write(cmd);
    i2c->write(msb);
    i2c->write(lsb);

    return (i2c->endTransmission() == 0);
}



/**
 * Channel validation helper
 *
 * - Prevents invalid channel access
 */
bool DAC7578_7Semi::validChannel(Channel ch)
{
    return (ch >= CH0 && ch <= CH7);
}
