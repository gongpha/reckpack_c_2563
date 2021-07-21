/*

kpattgen.h
By Kongfa Waroros (2020)(2563)

Generating Pattern
*/

#ifndef __klib_pattgen_h
#define __klib_pattgen_h

#include <stdlib.h>
#include <string.h>

#define kpttgn_nullptr NULL
#define kpttgn_whcMUL width * height * channel_num
#define kpttgn___clearzero_whc kpttgn___clear(buffer, 0, channel_num, width, height)
#define kpttgn___clearrgba_whc(col) kpttgn___clear(buffer, col, channel_num, width, height)
#define kpttgn___init_buffer kpttgn_b* buffer
#define kpttgn___Cmalloc_whc(width, height, channel_num) (buffer = kpttgn___malloc_whc(width, height, channel_num))
typedef unsigned char	kpttgn_b;

typedef union {
	unsigned int color;
	struct rgbaTag
	{
		kpttgn_b r;
		kpttgn_b g;
		kpttgn_b b;
		kpttgn_b a;
	} rgba;
	struct rgbTag
	{
		kpttgn_b r;
		kpttgn_b g;
		kpttgn_b b;
	} rgb;
	struct gTag
	{
		kpttgn_b g;
	} g;
}kpttgn_color;

typedef struct {
	kpttgn_color col;
	float fac;
}kpttgn_colorfac;

typedef struct {
	unsigned int num;
	kpttgn_colorfac* rampptr;
}kpttgn_colorramp;

kpttgn_colorramp* kpttgn__colorramp_createwith(kpttgn_colorfac* col, unsigned int size)
{
	kpttgn_colorramp* ramp = malloc(sizeof(kpttgn_colorramp));
	if (!ramp) return kpttgn_nullptr;

	if (col && size)
	{
		ramp->rampptr = malloc(sizeof(kpttgn_colorfac) * size);
		ramp->num = size;
		for (; size>=0;size--)
		for (int psize = 3; psize >= 0; psize--)
		{
			ramp->rampptr[(size * 4) + psize] = col[psize];
		}
	}
	else
	{
		ramp->num = 0;
		ramp->rampptr = kpttgn_nullptr;
	}
	return ramp;
}

kpttgn_colorramp* kpttgn__colorramp_create()
{
	return kpttgn__colorramp_createwith(kpttgn_nullptr, 0);
}

void kpttgn__colorramp_destroy(kpttgn_colorramp* ramp)
{
	if (!ramp) return;
	free(ramp->rampptr);
	free(ramp);
}

void kpttgn__colorramp_clear(kpttgn_colorramp* ramp)
{
	if (!ramp) return;
	free(ramp->rampptr);
	ramp->num = 0;
	ramp->rampptr = kpttgn_nullptr;
}

kpttgn_colorramp* kpttgn__colorramp_add_once(kpttgn_colorramp* ramp, kpttgn_colorfac col)
{
	if (!ramp) return ramp;
	kpttgn_colorfac* mem = realloc(ramp->rampptr, (ramp->num + 1) * sizeof(kpttgn_colorfac));
	if (!mem) return kpttgn_nullptr;
	ramp->num++;
	ramp->rampptr = mem;
	ramp->rampptr[ramp->num - 1] = col;
	return ramp;
}

kpttgn_colorramp* kpttgn__colorramp_add_array(kpttgn_colorramp* ramp, kpttgn_colorfac* col, unsigned int size)
{
	if (!(ramp && size)) return ramp;
	if (size == 1) return kpttgn__colorramp_add_once(ramp, *col);

	kpttgn_colorfac* mem = realloc(ramp->rampptr, (ramp->num + size) * sizeof(kpttgn_colorfac));
	if (!mem) return kpttgn_nullptr;
	for (int psize = (int)size - 1; psize >= 0; psize--)
	{
		mem[ramp->num + psize] = col[psize];
	}
	ramp->num += size;
	ramp->rampptr = mem;
	return ramp;
}

void kpttgn__colorramp_getatfac(kpttgn_colorramp* ramp, float fac, kpttgn_color* color)
{
	if (!ramp->num)
		return;

	for (int i = 0; i < ramp->num; i++)
	{
		kpttgn_colorfac* c = ramp->rampptr + (i * sizeof(kpttgn_colorfac));
		if (fac < c->fac)
		{
			kpttgn_colorfac* pc = ramp->rampptr + (max(0, i - 1) * sizeof(kpttgn_colorfac));
			float valueDiff = (pc->fac - c->fac);
			float fractBetween = (valueDiff == 0) ? 0 : (fac - c->fac) / valueDiff;

			color->rgba.r = ((pc->col.rgba.r - c->col.rgba.r) * fractBetween + c->col.rgba.r) * 255;
			color->rgba.g = ((pc->col.rgba.g - c->col.rgba.g) * fractBetween + c->col.rgba.g) * 255;
			color->rgba.b = ((pc->col.rgba.b - c->col.rgba.b) * fractBetween + c->col.rgba.b) * 255;
			color->rgba.a = ((pc->col.rgba.a - c->col.rgba.a) * fractBetween + c->col.rgba.a) * 255;
			return;
		}
	}
	color->rgba.r = (kpttgn_colorfac*)(ramp->rampptr + (ramp->num * sizeof(kpttgn_colorfac)))->col.rgba.r;
	color->rgba.g = (kpttgn_colorfac*)(ramp->rampptr + (ramp->num * sizeof(kpttgn_colorfac)))->col.rgba.g;
	color->rgba.b = (kpttgn_colorfac*)(ramp->rampptr + (ramp->num * sizeof(kpttgn_colorfac)))->col.rgba.b;
	color->rgba.a = (kpttgn_colorfac*)(ramp->rampptr + (ramp->num * sizeof(kpttgn_colorfac)))->col.rgba.a;
	return;
}

kpttgn_color kpttgn__make_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	kpttgn_color _;
	_.rgba.r = r;
	_.rgba.g = g;
	_.rgba.b = b;
	_.rgba.a = a;
	return _;
}
//										 AABBGGRR
const kpttgn_color kpttgn_color_black = { 0xFF000000 };
const kpttgn_color kpttgn_color_white = { 0xFFFFFFFF };

const kpttgn_color kpttgn_color_red = { 0xFF0000FF };
const kpttgn_color kpttgn_color_green = { 0xFF00FF00 };
const kpttgn_color kpttgn_color_blue = { 0xFFFF0000 };

// https://en.wikipedia.org/wiki/SMPTE_color_bars
const kpttgn_color kpttgn_color_smpte_100white = { 0xFFEBEBEB };
const kpttgn_color kpttgn_color_smpte_75white = { 0xFFB4B4B4 };
const kpttgn_color kpttgn_color_smpte_yellow = { 0xFF0CB4B5 };
const kpttgn_color kpttgn_color_smpte_cyan = { 0xFFB4B50D };
const kpttgn_color kpttgn_color_smpte_green = { 0xFF0CB5B7 };
const kpttgn_color kpttgn_color_smpte_magenta = { 0xFFB80FB7 };
const kpttgn_color kpttgn_color_smpte_red = { 0xFF100FB7 };
const kpttgn_color kpttgn_color_smpte_blue = { 0xFFB8100F };
const kpttgn_color kpttgn_color_smpte_black = { 0xFF101010 };
const kpttgn_color kpttgn_colors_smpte[] = {
	{0xFFEBEBEB},{0xFF0CB4B5},{0xFFB4B50D},{0xFF0CB5B7},
	{0xFFB80FB7},{0xFF100FB7},{0xFFB8100F}
};

kpttgn_b* kpttgn___clear(kpttgn_b* dst, kpttgn_color val, unsigned int channel_num, unsigned int width, unsigned int height)
{
	if (!dst) return kpttgn_nullptr;
	unsigned long w = 0;
	while (w < width * height) {
		for (unsigned int u = 0; u < channel_num; u++)
			(dst + (w * channel_num))[u] = ((unsigned char*)&val)[u];
		w++;
	}
	return dst;
}

kpttgn_b* kpttgn___malloc_whc(unsigned int width, unsigned int height, unsigned int channel_num)
{
	kpttgn_b* _ = (kpttgn_b*)
		malloc
		(width * height * channel_num);
	if (!_) return kpttgn_nullptr;
}

kpttgn_b* kpttgn__gen_checker(unsigned int width, unsigned int height, unsigned int channel_num, kpttgn_color even, kpttgn_color odd)
{
	kpttgn___init_buffer;
	if (!kpttgn___Cmalloc_whc(width, height, channel_num)) return kpttgn_nullptr;
	kpttgn___clearrgba_whc(even);
	unsigned int i = 1;
	while (i < width * height) {
		for (unsigned int cn = 0; cn < channel_num; cn++)
			buffer[(i * channel_num) + cn] = ((unsigned char*)&(odd.color))[cn];
		if (width % 2 == 0 && (i + ((i % 2 == 0) ? 2 : 1)) % (width) == 0) {
			if (i % 2 == 0)
				i += 3;
			else
			{
				++i;
				for (unsigned int cn = 0; cn < channel_num; cn++)
					buffer[(i * channel_num) + cn] = ((unsigned char*)&(odd.color))[cn];
				i += 2;
			}
		}
		else
			i += 2;
	}


	return buffer;
}

kpttgn_b* kpttgn__gen_checker_BW(unsigned int width, unsigned int height, unsigned int channel_num)
{
	return kpttgn__gen_checker(width, height, channel_num, kpttgn_color_black, kpttgn_color_white);
}

kpttgn_b* kpttgn__gen_smpte_simple_color_7bars(unsigned int width, unsigned int height, unsigned int channel_num)
{
	kpttgn_color wo[] = { kpttgn_color_red ,kpttgn_color_green,kpttgn_color_blue };
	kpttgn_colorramp* wow = kpttgn__colorramp_createwith(&(wo), 3);
	if (!wow) return kpttgn_nullptr;
	kpttgn___init_buffer;
	if (!kpttgn___Cmalloc_whc(width, height, channel_num)) return kpttgn_nullptr;
	unsigned int i = 1;
	while (i < width * height) {
		//if ((i + 1) % width == 0) ;
		kpttgn_color col;
		kpttgn__colorramp_getatfac(wow, 0.5f, &col);
		kpttgn__colorramp_getatfac(wow, (float)((i + 1) % width) / (float)width, &col);
		for (unsigned int cn = 0; cn < channel_num; cn++)
			buffer[(i * channel_num) + cn] = ((unsigned char*)&(col))[cn];
		i++;
	}
	kpttgn__colorramp_destroy(wow);
	return buffer;
}
#endif