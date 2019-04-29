#pragma once

////
//
//      ST7789 240x240 TFT Display Driver
//
////

#include <spi.h>
#include <gpio.h>

namespace stm32f0
{

namespace st7789
{

// rgb color using conventional 0..255 ranges
__attribute__((always_inline))
static inline uint16_t from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

// Swap byte order for 16-bit color scheme
__attribute__((always_inline))
static inline uint16_t swap_bytes(uint16_t x)
{
    uint16_t l = x & 0xff;

    return (l << 8) | (x >> 8);
}

template
    < uint8_t           SPI     // SPI peripheral number
    , gpio::gpio_pin_t  SCL     // SPI clock
    , gpio::gpio_pin_t  SDA     // SPI data (bidirectional)
    , gpio::gpio_pin_t  DC      // data / command select
    , gpio::gpio_pin_t  RS      // reset
    >
class st7789_t
{
public:
    static void setup()
    {
        dev::template setup<spi::mode_3, spi::msb_first, spi::fpclk_2, spi::high_speed>();

        res::setup();                               // display controller reset
        dcx::setup();                               // data / command selection

        res::clear();                               // reset display controller (active low)
        sys_tick::delay_ms(1);                      // FIXME: minumum pulse is 10us!
        res::set();                                 // release reset
        sys_tick::delay_ms(5);                      // this seems to work (some modes require 125ms)

        write_cmd(SLPOUT);                          // stop sleeping (in case we were)
        write_cmd(NORON);                           // normal-on
        write_cmd(MADCTL, 0x08);                    // RGB order
        write_cmd(COLMOD, 0x55);                    // 65k RGB & 16 bits per pixel
        write_cmd(LCMCTRL, 0x04);                   // reverse source output order
        write_cmd(CASET, 0, 0, 0, TFT_WIDTH - 1);   // column address set
        write_cmd(RASET, 0, 0, 0, TFT_HEIGHT - 1);  // row address set
        write_cmd(DISPON);                          // display-on
        write_cmd(INVON);                           // inverted mode

        clear();
    }

    static void clear()
    {
        write_cmd(RAMWR);                           // write pixel data
        for (uint32_t i = 0; i < 240*240; ++i)
            dev::write(static_cast<uint16_t>(0));
    }

    static inline void ram_write()
    {
        write_cmd(RAMWR);                           // write pixel data from origin
    }

    static inline void write(uint16_t x)
    {
        dev::write(x);                                     // 16-bit RGB data
    }

    static constexpr uint8_t width() { return TFT_WIDTH; }
    static constexpr uint8_t height() { return TFT_HEIGHT; }

private:
    typedef spi::spi_t<SPI, SCL, SDA>   dev;
    typedef gpio::output_t<DC>          dcx;
    typedef gpio::output_t<RS>          res;

    static const uint8_t TFT_WIDTH  = 240;
    static const uint8_t TFT_HEIGHT = 240;

    enum command_t
        { CABCCTRL     = 0xC7   // CABC control
        , CASET        = 0x2A
        , CMD2EN       = 0xDF   // Command 2 enable
        , COLMOD       = 0x3A
        , DGMEN        = 0xBA   // Digital gamma enable
        , DGMLUTB      = 0xE3   // Digital gamma look-up table for blue
        , DGMLUTR      = 0xE2   // Digital gamma look-up table for red
        , DISPOFF      = 0x28
        , DISPON       = 0x29
        , EQCTRL       = 0xE9   // Equalize time control
        , FRCTR2       = 0xC6   // FR Control 2
        , FRCTRL1      = 0xB3   // Frame rate control
        , GAMSET       = 0x26   // Gamma set
        , GATECTRL     = 0xE4   // Gate control
        , GCTRL        = 0xB7   // Gate control
        , GTADJ        = 0xB8   // Gate on timing adjustment
        , IDMOFF       = 0x38   // Idle mode off
        , IDMON        = 0x39   // Idle mode on
        , IDSET        = 0xC1   // ID setting
        , INVOFF       = 0x20
        , INVON        = 0x21
        , LCMCTRL      = 0xC0   // LCM control
        , MADCTL       = 0x36   // Memory data access control
        , NOP          = 0x00
        , NORON        = 0x13
        , NVGAMCTRL    = 0xE1   // Negative voltage gamma control
        , NVMSET       = 0xFC   // NVM setting
        , PARCTRL      = 0xB5   // Partial mode control
        , PORCTRL      = 0xB2   // Porch control
        , PROMACT      = 0xFE   // Program action
        , PROMCTRL     = 0xEC   // Program control
        , PROMEN       = 0xFA   // Program mode enable
        , PTLAR        = 0x30
        , PTLON        = 0x12
        , PVGAMCTRL    = 0xE0   // Positive voltage gamma control
        , PWCTRL1      = 0xD0   // Power control 1
        , PWCTRL2      = 0xE8   // Power control 2
        , PWMFRSEL     = 0xCC   // PWM frequency selection
        , RAMCTRL      = 0xB0   // RAM control
        , RAMRD        = 0x2E
        , RAMRDC       = 0x3E   // Memory read continue
        , RAMWR        = 0x2C
        , RAMWRC       = 0x3C   // Memory write continue
        , RASET        = 0x2B
        , RDD_COLMOD   = 0x0C   // Read display pixel format
        , RDD_MADCTL   = 0x0B   // Read display MADCTL
        , RDDID        = 0x04
        , RDDIM        = 0x0D   // Read display image mode
        , RDDPM        = 0x0A   // Read display power mode
        , RDDSM        = 0x0E   // Read display signal mode
        , RDDSR        = 0x0F   // Read display self-diagnostic result
        , RDDST        = 0x09
        , REGSEL1      = 0xC8   // Register value section 1
        , REGSEL2      = 0xCA   // Register value section 2
        , RGBCTRL      = 0xB1   // RGB control
        , RGBSET       = 0x2D   // Color setting for 4096, 64K and 262K colors
        , SLPIN        = 0x10
        , SLPOUT       = 0x11
        , SPI2EN       = 0xE7   // SPI2 enable
        , SWRESET      = 0x01
        , TEOFF        = 0x34   // Tearing effect line off
        , TEON         = 0x35   // Tearing effect line on
        , VAPVANEN     = 0xD2   // Enable VAP/VAN signal output
        , VCMOFSET     = 0xC5   // VCOMS offset set
        , VCOMS        = 0xBB   // VCOMS setting
        , VDVSET       = 0xC4   // VDV setting
        , VDVVRHEN     = 0xC2   // VDV and VRH command enable
        , VRHS         = 0xC3   // VRH set
        , VSCRDEF      = 0x33   // Vertical scrolling definition
        };

    template<typename... Args>
    static void write_cmd(command_t cmd, Args&&... args)
    {
        dcx::clear();
        dev::write(static_cast<uint8_t>(cmd));
        dev::wait_idle();
        dcx::set();
        (dev::write(static_cast<uint8_t>(args)), ...);
    }
};

} // namespace st7789

} // namespace stm32f0
