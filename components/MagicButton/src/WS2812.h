

#ifndef MAIN_WS2812_H_
#define MAIN_WS2812_H_
#include <stdint.h>
#include <driver/rmt.h>
#include <driver/gpio.h>
#include <math.h>

/**
 * @brief A data type representing the color of a pixel.
 */
//typedef struct {
//	uint8_t red;
//	uint8_t green;
//	uint8_t blue;
//} pixel_t;

#define min(a,b) ((a)<(b)?(a):(b))

struct RgbLedColor_t
{
	union
    {
        struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };
        uint8_t raw[3];
    };

    inline RgbLedColor_t() __attribute__((always_inline))
    {
    }

    // allow construction from R, G, B
    inline RgbLedColor_t( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
            : red(ir), green(ig), blue(ib)
    {
    }

    // allow construction from 32-bit (really 24-bit) bit 0xRRGGBB color code
    inline RgbLedColor_t( uint32_t colorcode)  __attribute__((always_inline))
            : red((colorcode >> 16) & 0xFF), green((colorcode >> 8) & 0xFF), blue((colorcode >> 0) & 0xFF)
    {
    }

    bool operator == (const RgbLedColor_t &c) const
    {
        return(this->red == c.red and this->green == c.green and this->blue == c.blue);
    }
/*
    ESPectro_LED_Color getPixel(AlaColor maxOut)
    {
        return ESPectro_LED_Color(r*maxOut.r/255, g*maxOut.g/255, b*maxOut.b/255);
    }
*/
    RgbLedColor_t sum(RgbLedColor_t color)
    {
        int r0 = min(color.red + red, 255);
        int g0 = min(color.green + green, 255);
        int b0 = min(color.blue + blue, 255);
        return RgbLedColor_t(r0, g0, b0);
    }

    RgbLedColor_t interpolate(RgbLedColor_t color, float x)
    {
        int r0 = x*(color.red - red) + red;
        int g0 = x*(color.green - green) + green;
        int b0 = x*(color.blue - blue) + blue;
        return RgbLedColor_t(r0, g0, b0);
    }

    RgbLedColor_t scale(float k)
    {
        int r0 = min(red*k, 255);
        int g0 = min(green*k, 255);
        int b0 = min(blue*k, 255);
        return RgbLedColor_t(r0, g0, b0);
    }
};


struct RgbLedPalette_t
{
    int numColors;
    RgbLedColor_t *colors;

    /**
    * Get the interpolated color from the palette.
    * The argument is a floating number between 0 and numColors.
    */
    RgbLedColor_t getPaletteColor(float i)
    {
        int i0 = (int)i%(numColors);
        int i1 = (int)(i+1)%(numColors);

        // decimal part is used to interpolate between the two colors
        float t0 = i - trunc(i);
        //float t0 = i - (int)i;

        return colors[i0].interpolate(colors[i1], t0);
    }

    bool operator == (const RgbLedPalette_t &c) const
    {
        if (!(this->numColors == c.numColors))
            return false;

        for(int i=0; i<numColors; i++)
        {
            if (!(this->colors[i] == c.colors[i]))
                return false;
        }
        return true;
    }
};


/**
 * @brief Driver for WS2812/NeoPixel data.
 *
 * NeoPixels or WS2812s are LED devices that can illuminate in arbitrary colors with
 * 8 bits of data for each of the red, green and blue channels.  These devices can be
 * daisy chained together to produce a string of LEDs.  If we call each LED instance
 * a pixel, then when we want to set the value of a string of pixels, we need to supply
 * the data for all the pixels.  This class encapsulates setting the color value for
 * an individual pixel within the string and, once you have set up all the desired colors,
 * you can then set all the pixels in a show() operation.  The class hides from you
 * the underlying details needed to drive the devices.
 *
 * @code{.cpp}
 * WS2812 ws2812 = WS2812(
 *   16, // Pin
 *   8   // Pixel count
 * );
 * ws2812.setPixel(0, 128, 0, 0);
 * ws2812.show();
 * @endcode
 */
class WS2812 {
public:
	WS2812(gpio_num_t gpioNum, uint16_t pixelCount, int channel=RMT_CHANNEL_0);
	void show();
	void setColorOrder(char *order);
	void setPixel(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
	void setPixel(uint16_t index, RgbLedColor_t pixel);
	void setPixel(uint16_t index, uint32_t pixel);
	void setBrightness(uint16_t index, uint8_t factor);
	void setBrightnessPercent(uint16_t index, float percent);
	void clear();
	uint16_t getPixelCount() {
		return pixelCount;
	}
	virtual ~WS2812();
private:
	char          *colorOrder;
	uint16_t       pixelCount;
	rmt_channel_t  channel;
	rmt_item32_t  *items;
	RgbLedColor_t *pixels;

	uint8_t 	  *brightnesses = NULL;
};

#endif /* MAIN_WS2812_H_ */
