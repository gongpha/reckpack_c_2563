
//#include "kpattgen.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "krectpack.h"
#include "kclitmplt.h"
#ifdef __cplusplus
#include <iostream>
#endif
int main(int argc, char** argv)
{
	/*
	const char* opt_[] = {
		"f", "Fuck MEEEEEE", "fuck|fuckme#?",
		"b", "GUDBAI VORLD", "bye",
		"o", "OK BOOMER", "ok|boomer",
		"w", "WTF is YOUR AGE", "wtfisyourage|age|wtfqisyourage#i1"
	};
	const kclitmplt_option opt[] = {
		{'w', "wtfisyourage", "WTF is YOUR AGE", 1},
		{'o', "okboomer", "OK BOOMER"}
	};
	kclitmplt_parameter** params;
	unsigned int count;
	kclitmplt__splash("krectpack", __DATE__, "A Rectangle Packer", "[options]", opt, 3, &params, &count, argc, argv);

	int return_count = -3;
	float occupancy;
#define C 256
#define www 512
#define hhh www
	krctpck_rectsize boxes[C];

	*/



	//#define RANDPLS


//#define MULTIPAGE
//#define RANDOM
#define SIMPLE
#define PROC
#define RANDOM

#ifndef SIMPLE
#define COUNT 30
#define WIDTH 900
#define HEIGHT 900
	char ch_arr[COUNT][14] = {
		"images/01.png",
		"images/02.png",
		"images/03.png",
		"images/04.png",
		"images/05.png",
		"images/06.png",
		"images/07.png",
		"images/08.png",
		"images/09.png",
		"images/10.png",
		"images/11.png",
		"images/12.png",
		"images/13.png",
		"images/14.png",
		"images/15.png",
		"images/16.png",
		"images/17.png",
		"images/18.png",
		"images/19.png",
		"images/20.png",
		"images/21.png",
		"images/22.png",
		"images/23.png",
		"images/24.png",
		"images/25.png",
		"images/26.png",
		"images/27.png",
		"images/28.png",
		"images/29.png",
		"images/30.png"
	};

	unsigned char ch[COUNT];
	unsigned char* data[COUNT];
	krctpck_rectin rects[COUNT];
	for (int a = 0; a < COUNT; a++) {
		int c;
		data[a] = stbi_load(ch_arr[a], (int*)&rects[a].width, (int*)&rects[a].height, &c, 0);
		ch[a] = (char)c;
		rects[a].can_rotate = 0;
	}
	krctpck_image images[COUNT];
#else

#ifdef RANDOM
#define COUNT 300
#define WIDTH 1024
#define HEIGHT 1024
	krctpck_rectin rects[COUNT];
	// RANDOM MACHINE
	srand(time((time_t*)rects));
	for (unsigned int i = 0; i < COUNT; i++)
	{
		do {
			rects[i].width = rand() % WIDTH / 8;
			rects[i].height = rand() % HEIGHT / 8;
		} while (!(rects[i].width && rects[i].height));
		rects[i].can_rotate = 0;
	}
#else
#define COUNT 6
#define WIDTH 128
#define HEIGHT 128
	krctpck_rectin rects[COUNT];
	////////////////////// CUSTOM HERE
	rects[0].width = 64;
	rects[1].width = 32;
	rects[2].width = 31;
	rects[3].width = 16;
	rects[4].width = 15;
	rects[5].width = 128;
	rects[0].height = rects[1].height = 16;
	rects[2].height = rects[3].height = 32;
	rects[4].height = rects[5].height = 64;
	rects[0].can_rotate = rects[1].can_rotate = rects[2].can_rotate = 0;
	rects[3].can_rotate = rects[4].can_rotate = rects[5].can_rotate = 0;
#endif
#endif
	krctpck_rectinarr arr;
	krctpck__group_create(&arr, rects, COUNT);

	krctpck_rectout out[COUNT];
	int a = 0;
#ifdef MULTIPAGE
	krctpck_page page[32];
	krctpck_page* curr_page = page;
	unsigned int num_page = 1;
#define PAGE curr_page
	retry :
#else
	krctpck_page page;
#define PAGE &page
#endif
	krctpck__page_create(PAGE, WIDTH, HEIGHT, krctpck_shelf);
#ifdef PROC
	unsigned char* pxss = (unsigned char*)calloc(WIDTH * HEIGHT * 4, sizeof(char));
#endif
	for (; a < COUNT; a++) {
		int r = krctpck__packmultiple(PAGE, &arr, krctpck_shelf_first_fit, &out[a]);
#ifdef MULTIPAGE
		if (r)
		{
			curr_page++;
			num_page++;
			std::cout << "========= NEW PAGE =========" << std::endl;
			goto retry;
		}
#else
		if (r) break;
#endif

#ifndef SIMPLE
		unsigned int i = abs(out[a].group_index) - 1;
		images[a].comp = ch[i];
		images[a].data = data[i];
		images[a].size = (unsigned long long)rects[i].width * rects[i].height * ch[i];
#endif
		std::cout << a << " " << r << " " << out[a].x << ":" << out[a].y << std::endl;
#ifdef PROC
			char filename[64];
			sprintf(filename, "result/proc/%i.png", a);
#ifndef SIMPLE
#define CH 4
			krctpck__render_rgba(pxss, WIDTH, out, images, a + 1);
			krctpck___render_page_data_rgba(pxss, PAGE);
#else
#define CH 3
			krctpck__render_simple_rgb(pxss, WIDTH, out, a + 1);
			krctpck___render_page_data_rgb(pxss, PAGE);
#endif
			stbi_write_png(filename, WIDTH, HEIGHT, CH, pxss, 0);
			memset(pxss, 0, WIDTH * HEIGHT * CH * sizeof(char));
#endif
	}
#ifdef PROC
	free(pxss);
#endif
#ifdef MULTIPAGE
	for (unsigned int p = 0; p < num_page; p++)
	{
#define PAGE_TARGET (&page[p])
#else
#define PAGE_TARGET (&page)
#endif

#ifndef PROC
#ifndef SIMPLE
#define CH 4
	unsigned char* pxs = (unsigned char*)calloc(PAGE_TARGET->width * PAGE_TARGET->height * 4, sizeof(char));
	krctpck__render_rgba(pxs, PAGE_TARGET->width, out, images, a);
#else
#define CH 3
	unsigned char* pxs = (unsigned char*)calloc(PAGE_TARGET->width * PAGE_TARGET->height * 3, sizeof(char));
	krctpck__render_simple_rgb(pxs, PAGE_TARGET);
#endif
#endif
	krctpck__page_free(PAGE_TARGET);
	//unsigned char* pxs = krctpck__preview_buffer_rgb(&out, width, height, boxes, ROT, 0);
	//unsigned char* pxs = krctpck__preview_buffer_rgb(&out, www, hhh, boxes, ROT, MUL);
#ifndef SIMPLE
	for (int a = 0; a < COUNT; a++) {
		stbi_image_free(data[a]);
	}
#endif
#ifdef MULTIPAGE
	char filename[64];
	sprintf(filename, "result/%i.png", p);
#else
	const char* filename = "result/_.png";
#endif
#ifndef PROC
	if (!stbi_write_png(filename, WIDTH, HEIGHT, CH, pxs, 0))
		return -2;
	free(pxs);
#endif
#ifdef MULTIPAGE
	}
#endif
krctpck__group_free(&arr);

return 0;
}
/*
int main()
{
#define mmm 128
#define nnn 4
	unsigned char* pxs;

	pxs = kpttgn__gen_checker_BW(mmm, mmm, nnn);
	stbi_write_png("result/result.png", mmm, mmm, nnn, pxs, mmm*nnn);
	free(pxs);

	pxs = kpttgn__gen_checker(mmm, mmm, nnn, kpttgn_color_red, kpttgn_color_green);
	stbi_write_png("result/result_rg.png", mmm, mmm, nnn, pxs, mmm * nnn);
	free(pxs);

	pxs = kpttgn__gen_smpte_simple_color_7bars(mmm, mmm, nnn);
	stbi_write_png("result/result_7b.png", mmm, mmm, nnn, pxs, mmm * nnn);
	free(pxs);

	return 0;
}
*/

void printUsage()
{

}

int mainc(int argc, char* argv[])
{
	if (argc < 2)
	{

	}
	return 0;
}