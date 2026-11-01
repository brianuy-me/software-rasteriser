#include <light.h>

#include <stdint.h>

light_t light = {
	.direction = { 0.0f, 0.0f, 1.0f }
};

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {
	if (percentage_factor < 0) {
		percentage_factor = 0;
	}
	if (percentage_factor > 1) {
		percentage_factor = 0;
	}
	
	uint32_t r = (uint32_t)((original_color & 0xFF000000) * percentage_factor);
	uint32_t g = (uint32_t)((original_color & 0x00FF0000) * percentage_factor);
	uint32_t b = (uint32_t)((original_color & 0x0000FF00) * percentage_factor);
	uint32_t a = (original_color & 0x000000FF);
	
	uint32_t new_color = a | (r & 0xFF000000) | (g & 0x00FF0000) | (b & 0x0000FF00);
	
	return new_color;
}

