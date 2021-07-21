/*

krectpack.h
By Kongfa Waroros (2020)(2563)

	Small Rectangle Packing Library
*/

#ifndef __klib_rectpack_h
#define __klib_rectpack_h

#define krctpck_SUCCESS 0
#define krctpck_MALLOC_FAILED 1
#define krctpck_NOT_FIT 2
#define krctpck_ALL_PACKED 3

typedef struct
{
	int x;
	int y;
	int width;
	int height;
} krctpck_rect;

#ifdef __cplusplus
struct krctpck_rectnode {
	struct krctpck_rectnode* prevnode; // up
	struct krctpck_rectnode* nextnode; // down
	krctpck_rect rect;
	int index;
};
#else
typedef struct {
	struct krctpck_rectnode* prevnode; // up
	struct krctpck_rectnode* nextnode; // down
	krctpck_rect rect;
	int index;
} krctpck_rectnode;
#endif

typedef int krctpck_error;

enum {
	krctpck_not_rotated = 1,
	krctpck_rotated = -1
};

typedef struct
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;

	// start at 1, negative means rotated 90 degrees
	int page_index; // page index
	int group_index; // item index if imported by group
} krctpck_rectout;

typedef struct
{
	unsigned int width;
	unsigned int height;
	int can_rotate;
} krctpck_rectin;

typedef struct
{
	krctpck_rectnode* root;
	krctpck_rectnode* root_used;
} krctpck_rectinarr;

typedef enum
{
	krctpck_maxrects, // Maxrects
	krctpck_skyline, // Skyline
	krctpck_shelf, // Shelf
} krctpck_algorithm;

typedef enum
{
	krctpck_maxrects_best_short_side_fit,
	krctpck_maxrects_best_long_side_fit,
	krctpck_maxrects_best_area_fit,
	krctpck_maxrects_bottom_left,
	krctpck_maxrects_contact_point
} krctpck_maxrects_heuristic_method;

typedef enum
{
	krctpck_skyline_bottom_left,
	krctpck_skyline_min_waste_fit
} krctpck_skyline_heuristic_method;

typedef enum
{
	krctpck_shelf_next_fit,
	krctpck_shelf_first_fit,
	krctpck_shelf_best_area_fit,
	krctpck_shelf_worst_area_fit,
	krctpck_shelf_best_height_fit,
	krctpck_shelf_best_width_fit,
	krctpck_shelf_worst_width_fit
} krctpck_shelf_heuristic_method;
#ifdef __cplusplus
struct krctpck_page {
#else
typedef struct {
#endif
	unsigned int width;
	unsigned int height;
	unsigned int used_count;

	// Skyline & Shelf
	unsigned int used_surface;
	krctpck_rectnode* used_rect;
	union {
		// Maxrects
		// use (x, y, width, height)
		krctpck_rectnode* free_rect;

		// Skyline
		// use (x, y, width, UNUSED)
		krctpck_rectnode* lines;

		// Shelf
		// use (x, y, UNUSED, height)
		/*
			(!) Shelf ordering is reversed.
				First Element : pointed to first shelf. For access last shelf, use nextnode of first element

				  head                                                                      tail
				  V                                                                         V
				({prevnode = f, nextnode = l}) (l {prevnode = null}) (s) (s) . . . (s) (s) (f {nextnode = null})
				                               (%%%%%%%%  above nodes is impossible to access head  %%%%%%%%%%%)
											             
		*/
		krctpck_rectnode* shelves;
	};

	krctpck_algorithm ag;
#ifdef __cplusplus
};
#else
} krctpck_page;
#endif

typedef struct
{
	unsigned long long size;
	unsigned char* data;
	unsigned int comp;
} krctpck_image;

#ifdef __cplusplus
extern "C" {
#endif
	/*
		Round a number to POT numbers

		Useful when trying to create POT texture
		https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	*/
	unsigned int krctpck__round_pot(unsigned int n);

	/*
		Pack one rect into a page
		if you want to pack multiple rects with optimization, use krctpck__packmultiple
	*/
	krctpck_error krctpck__packonce(krctpck_page* page, krctpck_rectin rect, int method, krctpck_rectout* dest_current_rect);

	/*
		Pack a group of rects
		This function may not pack first rect of the array
	*/
	krctpck_error krctpck__packmultiple(krctpck_page* page, krctpck_rectinarr* arr, int method, krctpck_rectout* dest_current_rect);

	krctpck_error krctpck__page_create(krctpck_page* page, unsigned int width, unsigned int height, krctpck_algorithm algorithm);
	void krctpck__page_free(krctpck_page* page);

	krctpck_error krctpck__group_create(krctpck_rectinarr* arr, krctpck_rectin* rects, unsigned int count);
	krctpck_error krctpck__group_create_each(krctpck_rectinarr* arr, unsigned int* w, unsigned int* h, int* can_rotate, unsigned int count);
	void krctpck__group_reset(krctpck_rectinarr* arr);
	void krctpck__group_free(krctpck_rectinarr* arr);

	float krctpck__page_calculate_occupancy(krctpck_page* page);
	void krctpck__render_simple_rgb(unsigned char* dstdata, unsigned int canvas_width, krctpck_rectout* outarr, unsigned int count);
	void krctpck__render_rgba(unsigned char* dstdata, unsigned int canvas_width, krctpck_rectout* outarr, krctpck_image* images, unsigned int count);
	void krctpck___render_page_data(unsigned char* dstdata, krctpck_page* page, int cmp);
	void krctpck___render_page_data_rgb(unsigned char* dstdata, krctpck_page* page);
	void krctpck___render_page_data_rgba(unsigned char* dstdata, krctpck_page* page);

#ifdef __cplusplus
}
#endif
#endif