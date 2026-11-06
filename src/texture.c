#include <texture.h>

#include <stdio.h>
#include <stdlib.h>

int texture_width = 64;
int texture_height = 64;

upng_t* png_texture = NULL;
uint32_t *mesh_texture = NULL;

void load_png_texture_data(const char *base_path, const char *filename) {
	char full_path[1024];
	snprintf(full_path, sizeof full_path, "%s%s", base_path, filename);

	png_texture = upng_new_from_file(full_path);
	if (png_texture == NULL) {
		return;
	}

	upng_decode(png_texture);
	if (upng_get_error(png_texture) != UPNG_EOK ||
	    upng_get_format(png_texture) != UPNG_RGBA8) {
		return;
	}

	const unsigned width = upng_get_width(png_texture);
	const unsigned height = upng_get_height(png_texture);
	const size_t pixel_count = (size_t)width * height;
	const unsigned char *src = upng_get_buffer(png_texture);

	uint32_t *pixels = malloc(pixel_count * sizeof *pixels);
	if (pixels == NULL) {
		return;
	}

	for (size_t i = 0; i < pixel_count; i++) {
		const unsigned char *p = src + i * 4;
		pixels[i] = ((uint32_t)p[0] << 24) |  // R
		            ((uint32_t)p[1] << 16) |  // G
		            ((uint32_t)p[2] << 8)  |  // B
		             (uint32_t)p[3];          // A
	}

	mesh_texture = pixels;
	texture_width = width;
	texture_height = height;
}

tex2_t tex2_clone(tex2_t *t) {
	tex2_t result = { t->u, t->v };
	return result;
}

