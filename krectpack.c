/*

krectpack.c
By Kongfa Waroros (2020)(2563)

*/

#include "krectpack.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#define UNUSED 0
#define krctpck__SWAP(x,y,t) {t _ = x; x = y; y = _;  }
/*
	For Rendering.
	Make a beautiful color
*/
void krctpck___hsv2rgb(unsigned short h, double s, double v, unsigned char* r, unsigned char* g, unsigned char* b)
{
	typedef struct {
		double r; double g; double b;
	} rgb;
	double H, P, Q, T, fract;
	(h == 360) ? (H = 0) : (H = h / 60.0);
	fract = H - floor(H);

	P = v * (1. - s);
	Q = v * (1. - s * fract);
	T = v * (1. - s * (1. - fract));

	if (0. <= H && H < 1.)
	{
		*r = (v * 255);
		*g = (T * 255);
		*b = (P * 255);
	}
	else if (1. <= H && H < 2.)
	{
		*r = (Q * 255);
		*g = (v * 255);
		*b = (P * 255);
	}
	else if (2. <= H && H < 3.)
	{
		*r = (P * 255);
		*g = (v * 255);
		*b = (T * 255);
	}
	else if (3. <= H && H < 4.)
	{
		*r = (P * 255);
		*g = (Q * 255);
		*b = (v * 255);
	}
	else if (4. <= H && H < 5.)
	{
		*r = (T * 255);
		*g = (P * 255);
		*b = (v * 255);
	}
	else if (5. <= H && H < 6.)
	{
		*r = (v * 255);
		*g = (P * 255);
		*b = (Q * 255);
	}
	else
	{
		*r = 0;
		*g = 0;
		*b = 0;
	}
}
float krctpck__page_calculate_occupancy(krctpck_page* page)
{
	krctpck_rectnode* best_line = NULL;
	switch (page->ag)
	{
	case krctpck_maxrects:
		return krctpck___maxrects_page_calculate_occupancy(page);
	case krctpck_skyline:
		return krctpck___skyline_page_calculate_occupancy(page);
	default:
		return -1.0f;
	}

}
void krctpck__render_simple_rgb(unsigned char* dstdata, unsigned int canvas_width, krctpck_rectout* outarr, unsigned int count)
{
	while (count)
	{
		count--;
		// NOT SO RANDOM
		unsigned char col[3];
		unsigned char colalt[3];
		unsigned short hue = abs((outarr[count].width + outarr[count].height) * (outarr[count].x - outarr[count].y)) % 360;
		krctpck___hsv2rgb(hue, 0.2, 1, &col[0], &col[1], &col[2]);
		krctpck___hsv2rgb(hue, 0.5, 1, &colalt[0], &colalt[1], &colalt[2]);
		if (outarr[count].page_index < 0)
		{
			for (unsigned int hh = 0; hh < outarr[count].height; hh++)
			{
				for (unsigned int ww = 0; ww < outarr[count].width; ww++)
				{
					float w_ = (float)ww / outarr[count].width;
					float h_ = (float)hh / outarr[count].height;
					unsigned long p = ((ww + outarr[count].x) * 3) + ((hh + outarr[count].y) * (canvas_width * 3));
					for (unsigned int cn = 0; cn < 3; cn++)
						dstdata[p + cn] = (w_ >= h_) ? col[cn] : colalt[cn];
				}
			}
			continue;
		}
		for (unsigned int hh = 0; hh < outarr[count].height; hh++)
		{
			for (unsigned int ww = 0; ww < outarr[count].width; ww++)
			{
				float w_ = (float)(outarr[count].width - ww) / outarr[count].width;
				float h_ = (float)hh / outarr[count].height;
				unsigned long p = ((ww + outarr[count].x) * 3) + ((hh + outarr[count].y) * (canvas_width * 3));
				for (unsigned int cn = 0; cn < 3; cn++)
					dstdata[p + cn] = (w_ >= h_) ? col[cn] : colalt[cn];
			}
		}
	}
}

/*
	Image[
		0
		1			->			rect(1) rect(2) rect(3)
		2
		...
	]
*/

void image___setRGB(unsigned char* dstdata, unsigned char* data, unsigned long long p, unsigned long long pr, unsigned char comp)
{
	dstdata[p + 0] = *(data + pr + 0);
	dstdata[p + 1] = *(data + pr + 1);
	dstdata[p + 2] = *(data + pr + 2);
	dstdata[p + 3] = 255;
}

void image___setGA(unsigned char* dstdata, unsigned char* data, unsigned long long p, unsigned long long pr, unsigned char comp)
{
	dstdata[p + 0] = *(data + pr + 0);
	dstdata[p + 1] = *(data + pr + 0);
	dstdata[p + 2] = *(data + pr + 0);
	dstdata[p + 3] = *(data + pr + 1);
}

void image___setG(unsigned char* dstdata, unsigned char* data, unsigned long long p, unsigned long long pr, unsigned char comp)
{
	dstdata[p + 0] = *(data + pr + 0);
	dstdata[p + 1] = *(data + pr + 0);
	dstdata[p + 2] = *(data + pr + 0);
	dstdata[p + 3] = 255;
}

void image___setelse(unsigned char* dstdata, unsigned char* data, unsigned long long p, unsigned long long pr, unsigned char comp)
{
	for (unsigned int cn = 0; cn < comp; cn++)
		dstdata[p + cn] = *(data + pr + cn);
}

unsigned long long image___pr_rot(unsigned int ww, unsigned int hh, krctpck_rect noderect, unsigned char comp)
{
	return ((unsigned long long)hh * comp) + (((unsigned long long)noderect.width - ww - 1) * ((unsigned long long)noderect.height * comp));
}

unsigned long long image___pr_unrot(unsigned int ww, unsigned int hh, krctpck_rect noderect, unsigned char comp)
{
	return ((unsigned long long)ww * comp) + ((unsigned long long)hh * ((unsigned long long)noderect.width * comp));
}

// malloc = page_width * page_height * 3
void krctpck___render_page_data(unsigned char* dstdata, krctpck_page* page, int cmp)
{
	unsigned int c = 0, cc = 0;
	krctpck_rectnode* nodedef, * node;
	switch (page->ag)
	{
	case krctpck_maxrects:
		nodedef = page->free_rect;
		break;
	case krctpck_skyline:
		nodedef = page->lines;
		break;
	case krctpck_shelf:
		nodedef = page->shelves->nextnode;
		break;
	default:
		nodedef = NULL;
	}
	node = nodedef;
	while (node)
	{
		node = node->nextnode;
		c++;
	}
	node = nodedef;
	while (node)
	{
		unsigned char col[3];
		krctpck___hsv2rgb(((float)cc / c) * 300, 1, 1, &col[0], &col[1], &col[2]);
		cc++;
		unsigned int width, height;
		switch (page->ag)
		{
		case krctpck_maxrects:
			width = node->rect.width;
			height = node->rect.height;
			break;
		case krctpck_skyline:
			width = node->rect.width;
			height = page->height - node->rect.y - 1;
			break;
		case krctpck_shelf:
			width = page->width - node->rect.x;
			height = node->rect.height;
			break;
		default:
			width = 0;
			height = 0;
		}
		for (unsigned int hh = 0; hh < height; hh++)
		{
			if (hh == 0 || hh == height - 1)
				for (unsigned int ww = 0; ww < width; ww++)
				{
					unsigned long p = ((ww + node->rect.x) * cmp) + ((hh + node->rect.y) * (page->width * cmp));
					dstdata[p + 0] = col[0];
					dstdata[p + 1] = col[1];
					dstdata[p + 2] = col[2];
					if (cmp > 3)
						dstdata[p + 3] = 255;
				}
			else
			{

				for (unsigned int cn = 0; cn < 3; cn++)
				{
					dstdata[((0 + node->rect.x) * cmp) + ((hh + node->rect.y) * (page->width * cmp)) + cn] = col[cn];
					dstdata[((width - 1 + node->rect.x) * cmp) + ((hh + node->rect.y) * (page->width * cmp)) + cn] = col[cn];
				}

				if (cmp > 3)
				{
					dstdata[((0 + node->rect.x) * cmp) + ((hh + node->rect.y) * (page->width * cmp)) + 3] = 255;
					dstdata[((width - 1 + node->rect.x) * cmp) + ((hh + node->rect.y) * (page->width * cmp)) + 3] = 255;
				}
			}
		}
		node = node->nextnode;
	}
}

void krctpck___render_page_data_rgb(unsigned char* dstdata, krctpck_page* page)
{
	krctpck___render_page_data(dstdata, page, 3);
}

void krctpck___render_page_data_rgba(unsigned char* dstdata, krctpck_page* page)
{
	krctpck___render_page_data(dstdata, page, 4);
}

// malloc = page_width * page_height * 4
void krctpck__render_rgba(unsigned char* dstdata, unsigned int canvas_width, krctpck_rectout* outarr, krctpck_image* images, unsigned int count)
{
	while (count)
	{
		count--;
#define IMAGE_NODE images[count]
		void (*plot)(unsigned char*, unsigned char*, unsigned long long, unsigned long long, unsigned char);
		unsigned long long (*rot)(unsigned int, unsigned int, krctpck_rect, unsigned char);
		if (IMAGE_NODE.comp == 3) plot = &image___setRGB;
		else if (IMAGE_NODE.comp == 2) plot = &image___setGA;
		else if (IMAGE_NODE.comp == 1) plot = &image___setG;
		else plot = &image___setelse;
		if (outarr[count].page_index < 0)
			rot = &image___pr_rot;
		else
			rot = &image___pr_unrot;
		for (unsigned int hh = 0; hh < outarr[count].height; hh++)
		{
			for (unsigned int ww = 0; ww < outarr[count].width; ww++)
			{
				unsigned long long p = (((unsigned long long)ww + outarr[count].x) * 4) + (((unsigned long long)hh + outarr[count].y) * (canvas_width * 4));
				unsigned long long pr;

				(*plot)(dstdata, IMAGE_NODE.data, p, (*rot)(ww, hh, (krctpck_rect) { outarr[count].x, outarr[count].y, outarr[count].width, outarr[count].height }, IMAGE_NODE.comp), IMAGE_NODE.comp);
#undef IMAGE_NODE
			}
		}
		continue;
	}
}

void krctpck___addToList(krctpck_rectnode** root, krctpck_rectnode* node)
{
	node->prevnode = NULL;
	node->nextnode = *root;
	if (*root)
	{
		(*root)->prevnode = node;
	}
	*root = node;
}

void krctpck___addFirstToList(krctpck_rectnode** root, krctpck_rectnode* node)
{
	krctpck_rectnode* curr = *root;
	if (curr)
		while (curr->nextnode)
			curr = curr->nextnode;
	node->prevnode = curr;
	node->nextnode = NULL;
	if (curr)
		curr->nextnode = node;
	if (!*root)
		*root = node;
	else
		curr = node;
}

// (n) (n) (<) (N) (A) (>) (n) (n)
void krctpck___insertToList(krctpck_rectnode** root, krctpck_rectnode* at, krctpck_rectnode* node)
{
	node->prevnode = at->prevnode;
	node->nextnode = at;
	if (at->prevnode) ((krctpck_rectnode*)(at->prevnode))->nextnode = node;
	at->prevnode = node;
	if (at == *root)
		*root = node;
}

void krctpck___insertToListRaw(krctpck_rectnode* at, krctpck_rectnode* node)
{
	node->prevnode = at->prevnode;
	node->nextnode = at;
	if (at->prevnode) ((krctpck_rectnode*)(at->prevnode))->nextnode = node;
	at->prevnode = node;
}

void krctpck___deleteFromList(krctpck_rectnode** root, krctpck_rectnode* node)
{
	if (node->prevnode) ((krctpck_rectnode*)(node->prevnode))->nextnode = node->nextnode;
	if (node->nextnode) ((krctpck_rectnode*)(node->nextnode))->prevnode = node->prevnode;
	if (*root == node) *root = node->nextnode;
	free(node);
}

void krctpck___moveList(krctpck_rectnode** dst_root, krctpck_rectnode** src_root, krctpck_rectnode* node)
{
	if (node->prevnode) ((krctpck_rectnode*)(node->prevnode))->nextnode = node->nextnode;
	if (node->nextnode) ((krctpck_rectnode*)(node->nextnode))->prevnode = node->prevnode;
	if (*src_root == node) *src_root = node->nextnode;

	node->prevnode = NULL;
	node->nextnode = *dst_root;
	if (*dst_root)
	{
		(*dst_root)->prevnode = node;
	}
	*dst_root = node;
}

void krctpck___moveAlltoList(krctpck_rectnode** dst_root, krctpck_rectnode** src_root)
{
	krctpck_rectnode* node = *dst_root;
	if (!(*src_root)) return;
	if (!node)
		*dst_root = *src_root;
	else
	{
		while (node->nextnode)
			node = node->nextnode;
		node->nextnode = *src_root;
		(*src_root)->prevnode = node;
	}
	*src_root = NULL;
}

krctpck_rectnode* krctpck___createRectNode(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	krctpck_rectnode* r = malloc(sizeof(krctpck_rectnode));
	if (!r) return NULL;
	r->rect.x = x;
	r->rect.y = y;
	r->rect.width = width;
	r->rect.height = height;

	r->prevnode = r->nextnode = NULL;
	r->index = 0;
	return r;
}

krctpck_rectnode* krctpck___createRectNodeByRect(krctpck_rect rect)
{
	krctpck_rectnode* r = malloc(sizeof(krctpck_rectnode));
	if (!r) return NULL;
	r->rect.x = rect.x;
	r->rect.y = rect.y;
	r->rect.width = rect.width;
	r->rect.height = rect.height;

	r->prevnode = r->nextnode = NULL;
	r->index = 0;
	return r;
}

krctpck_rectnode* krctpck___createRectNodeByNode(krctpck_rectnode node)
{
	krctpck_rectnode* r = malloc(sizeof(krctpck_rectnode));
	if (!r) return NULL;
	r->rect = node.rect;
	r->prevnode = node.prevnode;
	r->nextnode = node.nextnode;
	r->index = node.index;
	return r;
}

krctpck_rectnode* krctpck___shelf_create_shelf(krctpck_rectnode* head, unsigned int height)
{
	unsigned int y;
	krctpck_rectnode* last_shelf = head->nextnode;
	krctpck_rectnode* first_shelf = head->prevnode;
	if (last_shelf)
		y = last_shelf->rect.y + last_shelf->rect.height;
	else
		y = 0;
	krctpck_rectnode* new_shelf = krctpck___createRectNode(0, y, UNUSED, height);
	if (!new_shelf) return new_shelf;
	if (last_shelf)
		krctpck___insertToListRaw(last_shelf, new_shelf);

	head->nextnode = new_shelf;
	if (!first_shelf) head->prevnode = new_shelf;
	return new_shelf;
}

krctpck_rect krctpck___maxrects_findpos4nn_bssf(krctpck_rectnode* freerect, unsigned int width, unsigned int height, int* bestshortsidefit, int* bestlongsidefit, int* can_rotate)
{
	krctpck_rect best_node = { 0, 0, 0, 0 };

	*bestshortsidefit = INT_MAX;
	krctpck_rectnode* curr_freenode = freerect;
	for (; curr_freenode; curr_freenode = curr_freenode->nextnode)
	{
		if (curr_freenode->rect.width >= (int)width && curr_freenode->rect.height >= (int)height)
		{
			int leftover_horizontal =
				abs((int)curr_freenode->rect.width - width);
			int leftover_vertical =
				abs((int)curr_freenode->rect.height - height);
			int shortsidefit =
				min(leftover_horizontal, leftover_vertical);
			int longsidefit =
				max(leftover_horizontal, leftover_vertical);

			if (shortsidefit < *bestshortsidefit || (shortsidefit == *bestshortsidefit && longsidefit < *bestlongsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = width;
				best_node.height = height;
				*can_rotate = 1;
				*bestshortsidefit = shortsidefit;
				*bestlongsidefit = longsidefit;
			}
		}
		if (curr_freenode->rect.width >= (int)height && curr_freenode->rect.height >= (int)width &&
			*can_rotate)
		{
			int flipped_leftover_horizontal =
				abs((int)(curr_freenode->rect.width - height));
			int flipped_leftover_vertical =
				abs((int)(curr_freenode->rect.height - width));
			int flipped_shortsidefit =
				min(flipped_leftover_horizontal, flipped_leftover_vertical);
			int flipped_longsidefit =
				max(flipped_leftover_horizontal, flipped_leftover_vertical);

			if (flipped_shortsidefit < *bestshortsidefit || (flipped_shortsidefit == *bestshortsidefit && flipped_longsidefit < *bestlongsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = height;
				best_node.height = width;
				*can_rotate = -1;
				*bestshortsidefit = flipped_shortsidefit;
				*bestlongsidefit = flipped_longsidefit;
			}
		}
	}
	return best_node;
}

krctpck_rect krctpck___maxrects_findpos4nn_bl(krctpck_rectnode* freerect, unsigned int width, unsigned int height, int* best_y, int* best_x, int* can_rotate)
{
	krctpck_rect best_node = { 0, 0, 0, 0 };

	*best_y = INT_MAX;
	krctpck_rectnode* curr_freenode = freerect;
	for (; curr_freenode; curr_freenode = curr_freenode->nextnode)
	{
		if (curr_freenode->rect.width >= (int)width && curr_freenode->rect.height >= (int)height)
		{
			int top_side_y = curr_freenode->rect.y + height;

			if (top_side_y < *best_y || (top_side_y == *best_y && curr_freenode->rect.x < *best_x))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = width;
				best_node.height = height;
				*can_rotate = 1;
				*best_x = curr_freenode->rect.x;
				*best_y = top_side_y;
			}
		}
		if (curr_freenode->rect.width >= (int)height && curr_freenode->rect.height >= (int)width &&
			*can_rotate)
		{
			int top_side_y = curr_freenode->rect.y + width;

			if (top_side_y < *best_y || (top_side_y == *best_y && curr_freenode->rect.x < *best_x))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = height;
				best_node.height = width;
				*can_rotate = -1;
				*best_y = top_side_y;
				*best_x = curr_freenode->rect.x;
			}
		}
	}
	return best_node;
}

krctpck_rect krctpck___maxrects_findpos4nn_baf(krctpck_rectnode* freerect, unsigned int width, unsigned int height, int* bestareafit, int* bestshortsidefit, int* can_rotate)
{
	krctpck_rect best_node = { 0, 0, 0, 0 };

	*bestareafit = INT_MAX;
	krctpck_rectnode* curr_freenode = freerect;
	for (; curr_freenode; curr_freenode = curr_freenode->nextnode)
	{
		int areafit = curr_freenode->rect.width * curr_freenode->rect.height - width * height;

		if (curr_freenode->rect.width >= (int)width && curr_freenode->rect.height >= (int)height)
		{
			int leftover_horizontal =
				abs((int)(curr_freenode->rect.width - width));
			int leftover_vertical =
				abs((int)(curr_freenode->rect.height - height));
			int shortsidefit =
				min(leftover_horizontal, leftover_vertical);
			if (areafit < *bestareafit || (areafit == *bestareafit && shortsidefit < *bestshortsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = width;
				best_node.height = height;
				*can_rotate = 1;
				*bestshortsidefit = shortsidefit;
				*bestareafit = areafit;
			}
		}

		if (curr_freenode->rect.width >= (int)height && curr_freenode->rect.height >= (int)width &&
			*can_rotate)
		{
			int leftover_horizontal =
				abs((int)(curr_freenode->rect.width - height));
			int leftover_vertical =
				abs((int)(curr_freenode->rect.height - width));
			int shortsidefit =
				min(leftover_horizontal, leftover_vertical);
			if (areafit < *bestareafit || (areafit == *bestareafit && shortsidefit < *bestshortsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = height;
				best_node.height = width;
				*can_rotate = -1;
				*bestshortsidefit = shortsidefit;
				*bestareafit = areafit;
			}
		}
	}
	return best_node;
}

int krctpck___maxrects_common_interval_length(int i1_start, int i1_end, int i2_start, int i2_end)
{
	if (i1_end < i2_start || i2_end < i1_start)
		return 0;
	return min(i1_end, i2_end) - max(i1_start, i2_start);
}
int krctpck___maxrects_cp_scorenode(krctpck_rectnode* usedrect, unsigned int page_width, unsigned int page_height, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	int score = 0;

	if (!x || x + width == page_width) score += height;
	if (!y || y + height == page_height) score += width;

	krctpck_rectnode* curr_usednode = usedrect;
	for (; curr_usednode; curr_usednode = curr_usednode->nextnode)
	{
		if (curr_usednode->rect.x == x + width || curr_usednode->rect.x + width == x)
			score += krctpck___maxrects_common_interval_length(curr_usednode->rect.y, curr_usednode->rect.y + curr_usednode->rect.height, y, y + height);
		if (curr_usednode->rect.y == y + height || curr_usednode->rect.y + height == y)
			score += krctpck___maxrects_common_interval_length(curr_usednode->rect.x, curr_usednode->rect.x + curr_usednode->rect.width, x, x + width);
	}
	return score;
}

krctpck_rect krctpck___maxrects_findpos4nn_cp(krctpck_rectnode* freerect, krctpck_rectnode* usedrect, unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, int* bestcontactscore, int* can_rotate)
{
	krctpck_rect best_node = { 0, 0, 0, 0 };

	*bestcontactscore = -1;
	krctpck_rectnode* curr_freenode = freerect;
	for (; curr_freenode; curr_freenode = curr_freenode->nextnode)
	{
		if (curr_freenode->rect.width >= (int)width && curr_freenode->rect.height >= (int)height)
		{
			int score = krctpck___maxrects_cp_scorenode(usedrect, page_width, page_height, curr_freenode->rect.x, curr_freenode->rect.y, width, height);

			if (score > (*bestcontactscore))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = width;
				best_node.height = height;
				*can_rotate = 1;
				*bestcontactscore = score;
			}
		}
		if (curr_freenode->rect.width >= (int)height && curr_freenode->rect.height >= (int)width &&
			*can_rotate)
		{
			int score = krctpck___maxrects_cp_scorenode(usedrect, page_width, page_height, curr_freenode->rect.x, curr_freenode->rect.y, height, width);

			if (score > (*bestcontactscore))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = height;
				best_node.height = width;
				*can_rotate = -1;
				*bestcontactscore = score;
			}
		}
	}
	return best_node;
}

krctpck_rect krctpck___maxrects_findpos4nn_blsf(krctpck_rectnode* freerect, unsigned int width, unsigned int height, int* bestshortsidefit, int* bestlongsidefit, int* can_rotate)
{
	krctpck_rect best_node = { 0, 0, 0, 0 };

	*bestlongsidefit = INT_MAX;
	krctpck_rectnode* curr_freenode = freerect;
	for (; curr_freenode; curr_freenode = curr_freenode->nextnode)
	{
		if (curr_freenode->rect.width >= (int)width && curr_freenode->rect.height >= (int)height)
		{
			int leftover_horizontal =
				abs((int)(curr_freenode->rect.width - width));
			int leftover_vertical =
				abs((int)(curr_freenode->rect.height - height));
			int shortsidefit =
				min(leftover_horizontal, leftover_vertical);
			int longsidefit =
				max(leftover_horizontal, leftover_vertical);

			if (longsidefit < *bestlongsidefit || (longsidefit == *bestlongsidefit && shortsidefit < *bestshortsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = width;
				best_node.height = height;
				*can_rotate = 1;
				*bestshortsidefit = shortsidefit;
				*bestlongsidefit = longsidefit;
			}
		}
		if (curr_freenode->rect.width >= (int)height && curr_freenode->rect.height >= (int)width &&
			*can_rotate)
		{
			int flipped_leftover_horizontal =
				abs((int)(curr_freenode->rect.width - height));
			int flipped_leftover_vertical =
				abs((int)(curr_freenode->rect.height - width));
			int flipped_shortsidefit =
				min(flipped_leftover_horizontal, flipped_leftover_vertical);
			int flipped_longsidefit =
				max(flipped_leftover_horizontal, flipped_leftover_vertical);

			if (flipped_longsidefit < *bestlongsidefit || (flipped_longsidefit == *bestlongsidefit && flipped_shortsidefit < *bestshortsidefit))
			{
				best_node.x = curr_freenode->rect.x;
				best_node.y = curr_freenode->rect.y;
				best_node.width = height;
				best_node.height = width;
				*can_rotate = -1;
				*bestshortsidefit = flipped_shortsidefit;
				*bestlongsidefit = flipped_longsidefit;
			}
		}
	}
	return best_node;
}

krctpck_rect krctpck___maxrects_scorerect(krctpck_page* page, krctpck_maxrects_heuristic_method method, unsigned int width, unsigned int height, int* can_rotate, int* score1, int* score2)
{
	krctpck_rect new_node = { 0, 0, 0, 0 };
	*score1 = INT_MAX;
	*score2 = INT_MAX;

	switch (method)
	{
	case krctpck_maxrects_best_short_side_fit:
		new_node = krctpck___maxrects_findpos4nn_bssf(page->free_rect, width, height, score1, score2, can_rotate);
		break;
	case krctpck_maxrects_best_long_side_fit:
		new_node = krctpck___maxrects_findpos4nn_bl(page->free_rect, width, height, score1, score2, can_rotate);
		break;
	case krctpck_maxrects_best_area_fit:
		new_node = krctpck___maxrects_findpos4nn_cp(page->free_rect, page->used_rect, page->width, page->height, width, height, score1, can_rotate);
		*score1 = -(*score1);
		break;
	case krctpck_maxrects_bottom_left:
		new_node = krctpck___maxrects_findpos4nn_blsf(page->free_rect, width, height, score2, score1, can_rotate);
		break;
	case krctpck_maxrects_contact_point:
		new_node = krctpck___maxrects_findpos4nn_baf(page->free_rect, width, height, score1, score2, can_rotate);
		break;
	}

	if (!new_node.height)
	{
		*score1 = INT_MAX;
		*score2 = INT_MAX;
	}
	return new_node;
}

int krctpck___maxrects_splitfreenode(krctpck_rectnode** freerect_ptr, krctpck_rectnode* free_node, krctpck_rectnode* used_node)
{
	if (used_node->rect.x >= free_node->rect.x + free_node->rect.width || used_node->rect.x + used_node->rect.width <= free_node->rect.x || used_node->rect.y >= free_node->rect.y + free_node->rect.height || used_node->rect.y + used_node->rect.height <= free_node->rect.y)
		return 0;

	if (used_node->rect.x < free_node->rect.x + free_node->rect.width && used_node->rect.x + used_node->rect.width > free_node->rect.x)
	{
		if (used_node->rect.y > free_node->rect.y && used_node->rect.y < free_node->rect.y + free_node->rect.height)
		{
			krctpck_rectnode* new_node = krctpck___createRectNodeByRect(free_node->rect);
			if (!new_node) return -1;
			new_node->rect.height = used_node->rect.y - new_node->rect.y;
			krctpck___addToList(freerect_ptr, new_node);
		}

		if (used_node->rect.y + used_node->rect.height < free_node->rect.y + free_node->rect.height)
		{
			krctpck_rectnode* new_node = krctpck___createRectNodeByRect(free_node->rect);
			if (!new_node) return -1;
			new_node->rect.y = used_node->rect.y + used_node->rect.height;
			new_node->rect.height = free_node->rect.y + free_node->rect.height - (used_node->rect.y + used_node->rect.height);
			krctpck___addToList(freerect_ptr, new_node);
		}
	}

	if (used_node->rect.y < free_node->rect.y + free_node->rect.height && used_node->rect.y + used_node->rect.height > free_node->rect.y)
	{
		if (used_node->rect.x > free_node->rect.x && used_node->rect.x < free_node->rect.x + free_node->rect.width)
		{
			krctpck_rectnode* new_node = krctpck___createRectNodeByRect(free_node->rect);
			if (!new_node) return -1;
			new_node->rect.width = used_node->rect.x - new_node->rect.x;
			krctpck___addToList(freerect_ptr, new_node);
		}

		if (used_node->rect.x + used_node->rect.width < free_node->rect.x + free_node->rect.width)
		{
			krctpck_rectnode* new_node = krctpck___createRectNodeByRect(free_node->rect);
			if (!new_node) return -1;
			new_node->rect.x = used_node->rect.x + used_node->rect.width;
			new_node->rect.width = free_node->rect.x + free_node->rect.width - (used_node->rect.x + used_node->rect.width);
			krctpck___addToList(freerect_ptr, new_node);
		}
	}

	return 1;
}

int krctpck___iscontainedin(krctpck_rectnode* a, krctpck_rectnode* b)
{
	return
		a->rect.x >= b->rect.x &&
		a->rect.y >= b->rect.y &&
		a->rect.x + a->rect.width <= b->rect.x + b->rect.width &&
		a->rect.y + a->rect.height <= b->rect.y + b->rect.height;
}

void krctpck___maxrects_prunefreelist(krctpck_rectnode** freerect_ptr)
{
	krctpck_rectnode* curr_freenode[2];
	krctpck_rectnode* curr_freenodefor[2];
	for (curr_freenodefor[0] = *freerect_ptr; curr_freenodefor[0];)
	{
		curr_freenode[0] = curr_freenodefor[0];
		curr_freenodefor[0] = curr_freenodefor[0]->nextnode;
		curr_freenodefor[1] = curr_freenodefor[0];
		for (; curr_freenodefor[1];)
		{
			curr_freenode[1] = curr_freenodefor[1];
			curr_freenodefor[1] = curr_freenodefor[1]->nextnode;
			if (krctpck___iscontainedin(curr_freenode[0], curr_freenode[1]))
			{
				krctpck___deleteFromList(freerect_ptr, curr_freenode[0]);
				break;
			}
			if (krctpck___iscontainedin(curr_freenode[1], curr_freenode[0]))
			{
				if (curr_freenode[1] == curr_freenodefor[0]) curr_freenodefor[0] = curr_freenodefor[0]->nextnode;
				krctpck___deleteFromList(freerect_ptr, curr_freenode[1]);
			}
		}
	}
}

krctpck_error krctpck___maxrects_placerect(krctpck_rectnode** freerect_ptr, krctpck_rectnode** usedrect_ptr, krctpck_rectnode* node)
{
	krctpck_rectnode* curr_freenode = *freerect_ptr;
	while (curr_freenode)
	{
		krctpck_rectnode* free_node = curr_freenode;
		int splitr = krctpck___maxrects_splitfreenode(freerect_ptr, curr_freenode, node);
		if (splitr < 0) return krctpck_MALLOC_FAILED;
		curr_freenode = curr_freenode->nextnode;
		if (splitr) krctpck___deleteFromList(freerect_ptr, free_node);
	}
	krctpck___maxrects_prunefreelist(freerect_ptr);
	krctpck___addToList(usedrect_ptr, node);
	return krctpck_SUCCESS;
}

void krctpck___cleanNodeChains(krctpck_rectnode* root)
{
	if (!root) return;
	if (root->nextnode) {
		krctpck___cleanNodeChains(root->nextnode);
	}
	free(root);
}

float krctpck___skyline_page_calculate_occupancy(krctpck_page* page)
{
	return (float)page->used_surface / (page->width * page->height);
}

float krctpck___maxrects_page_calculate_occupancy(krctpck_page* page)
{
	unsigned long long used_area = 0;
	krctpck_rectnode* curr_usednode = page->used_rect;
	for (; curr_usednode; curr_usednode = curr_usednode->nextnode)
		used_area += (unsigned long long)curr_usednode->rect.width * curr_usednode->rect.height;

	return (float)used_area / (page->width * page->height);
}

krctpck_error krctpck__page_create(krctpck_page* page, unsigned int width, unsigned int height, krctpck_algorithm algorithm)
{
	page->width = width;
	page->height = height;
	page->used_count = 0;
	page->ag = algorithm;
	page->free_rect = page->used_rect = NULL;

	krctpck_rectnode* initnode;
	switch (page->ag)
	{
	case krctpck_maxrects:
		initnode = krctpck___createRectNode(0, 0, page->width, page->height);
		if (!initnode) return krctpck_MALLOC_FAILED;
		krctpck___addToList(&page->free_rect, initnode);
		break;
	case krctpck_skyline:
		page->used_surface = 0;
		initnode = krctpck___createRectNode(0, 0, page->width, UNUSED);
		if (!initnode) return krctpck_MALLOC_FAILED;
		krctpck___addToList(&page->lines, initnode);
		break;
	case krctpck_shelf:
		page->used_surface = 0;

		initnode = krctpck___createRectNode(UNUSED, UNUSED, UNUSED, UNUSED);
		if (!initnode) return krctpck_MALLOC_FAILED;
		krctpck___addToList(&page->shelves, initnode);

		initnode = krctpck___shelf_create_shelf(initnode, 0);
		if (!initnode) return krctpck_MALLOC_FAILED;
		break;
	}

	return krctpck_SUCCESS;
}

krctpck_error krctpck___maxrects_packonce(krctpck_page* page, krctpck_rectin rect, krctpck_maxrects_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	int score1 = INT_MAX;
	int score2 = INT_MAX;

	int rot = rect.can_rotate;
	krctpck_rect node = krctpck___maxrects_scorerect(page, method, rect.width, rect.height, &rot, &score1, &score2);

	if (score1 == INT_MAX)
		return krctpck_NOT_FIT;

	krctpck_rectnode* new_node = krctpck___createRectNode(node.x, node.y, node.width, node.height);
	new_node->index = rot;
	krctpck___maxrects_placerect(&page->free_rect, &page->used_rect, new_node);

	dest_current_rect->x = new_node->rect.x;
	dest_current_rect->y = new_node->rect.y;
	dest_current_rect->width = new_node->rect.width;
	dest_current_rect->height = new_node->rect.height;
	dest_current_rect->page_index = new_node->index;

	return krctpck_SUCCESS;
}

krctpck_error krctpck___maxrects_packmultiple(krctpck_page* page, krctpck_rectinarr* arr, krctpck_maxrects_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	krctpck_rectnode best_node;
	krctpck_rectnode* best_rect = NULL;
	krctpck_rectnode* curr_node = arr->root;
	krctpck_error err;

	int best_score1 = INT_MAX;
	int best_score2 = INT_MAX;

	if (!curr_node) return krctpck_ALL_PACKED;

	while (curr_node)
	{
		int score1, score2, rot;
		rot = curr_node->index < 0;
		krctpck_rect new_node = krctpck___maxrects_scorerect(page, method, curr_node->rect.width, curr_node->rect.height, &rot, &score1, &score2);
		if (score1 < best_score1 || (score1 == best_score1 && score2 < best_score2))
		{
			// New best node
			best_score1 = score1;
			best_score2 = score2;
			best_node = (krctpck_rectnode){ NULL, NULL, new_node, rot };
			best_rect = curr_node;
		}
		curr_node = curr_node->nextnode;
	}
	if (!best_rect) return krctpck_NOT_FIT;

	krctpck_rectnode* selected_node = krctpck___createRectNodeByNode(best_node);
	if (selected_node) return krctpck_MALLOC_FAILED;
	if (err = krctpck___maxrects_placerect(&page->free_rect, &page->used_rect, selected_node))
	{
		free(selected_node);
		return err;
	}

	krctpck___moveList(&arr->root_used, &arr->root, best_rect);

	dest_current_rect->x = selected_node->rect.x;
	dest_current_rect->y = selected_node->rect.y;
	dest_current_rect->width = selected_node->rect.width;
	dest_current_rect->height = selected_node->rect.height;
	dest_current_rect->page_index = selected_node->index * ++page->used_count;
	dest_current_rect->group_index = selected_node->index * abs(best_rect->index);
	selected_node->index *= page->used_count;
	return krctpck_SUCCESS;
}

int krctpck___skyline_rectfits(krctpck_rectnode* line, unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, int* y)
{
	if (line->rect.x + width > page_width) return 0;
	int width_left = width;
	*y = line->rect.y;
	while (width_left > 0)
	{
		*y = max(*y, line->rect.y);
		if (*y + height > page_height) return 0;
		width_left -= line->rect.width;
		line = line->nextnode;
	}
	return 1;
}

int krctpck___skyline_compute_wasted_area(krctpck_rectnode* line, unsigned int width, unsigned int height, int y)
{
	int wasted_area = 0;
#define RECT_LEFT line->rect.x
#define RECT_RIGHT (line->rect.x + width)
	for (; line && line->rect.x < RECT_RIGHT; line = line->nextnode)
	{
		if (line->rect.x >= RECT_RIGHT || line->rect.x + line->rect.width <= RECT_LEFT) break;

		int left_side = line->rect.x;
		int right_side = min(RECT_RIGHT, left_side + line->rect.width);
		wasted_area += (right_side - left_side) * (y - line->rect.y);
	}
	return wasted_area;
#undef RECT_LEFT
#undef RECT_RIGHT
}

int krctpck___skyline_rectfits_wasted(krctpck_rectnode* line, unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, int* y, int* wasted_area)
{
	int fits = krctpck___skyline_rectfits(line, page_width, page_height, width, height, y);
	if (fits)
		*wasted_area = krctpck___skyline_compute_wasted_area(line, width, height, *y);
	return fits;
}
krctpck_rect krctpck___skyline_findpos4nn_bl(unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, krctpck_rectnode* usednode_root, int* best_height, int* best_width, krctpck_rectnode** best_line, int* can_rotate)
{
	krctpck_rect new_node = { 0, 0, 0, 0 };
	*best_height = INT_MAX;
	*best_width = INT_MAX;
	*best_line = NULL;

	krctpck_rectnode* curr_line = usednode_root;
	for (; curr_line; curr_line = curr_line->nextnode)
	{
		int y;
		if (krctpck___skyline_rectfits(curr_line, page_width, page_height, width, height, &y))
		{
			if (y + height < *best_height || (y + height == *best_height && curr_line->rect.width < *best_width))
			{
				*best_height = y + height;
				*best_width = curr_line->rect.width;
				*best_line = curr_line;
				new_node.x = curr_line->rect.x;
				new_node.y = y;
				new_node.width = width;
				new_node.height = height;
				*can_rotate = 1;
			}

		}
		if (*can_rotate) {
			if (krctpck___skyline_rectfits(curr_line, page_width, page_height, height, width, &y))
			{
				if (y + width < *best_height || (y + width == *best_height && curr_line->rect.width < *best_width))
				{
					*best_height = y + width;
					*best_width = curr_line->rect.width;
					*best_line = curr_line;
					new_node.x = curr_line->rect.x;
					new_node.y = y;
					new_node.width = height;
					new_node.height = width;
					*can_rotate = -1;
				}
			}
		}
	}

	return new_node;
}

krctpck_rect krctpck___skyline_findpos4nn_mwf(unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, krctpck_rectnode* usednode_root, int* best_height, int* best_wasted_area, krctpck_rectnode** best_line, int* can_rotate)
{
	krctpck_rect new_node = { 0, 0, 0, 0 };
	*best_height = INT_MAX;
	*best_wasted_area = INT_MAX;
	*best_line = NULL;

	krctpck_rectnode* curr_line = usednode_root;
	for (; curr_line; curr_line = curr_line->nextnode)
	{
		int y;
		int wasted_area;
		if (krctpck___skyline_rectfits_wasted(curr_line, page_width, page_height, width, height, &y, &wasted_area))
		{
			if (wasted_area < *best_wasted_area || (wasted_area == *best_wasted_area && y + height < *best_height))
			{
				*best_height = y + height;
				*best_wasted_area = wasted_area;
				*best_line = curr_line;
				new_node.x = curr_line->rect.x;
				new_node.y = y;
				new_node.width = width;
				new_node.height = height;
				*can_rotate = 1;
			}

		}
		if (*can_rotate) {
			if (krctpck___skyline_rectfits_wasted(curr_line, page_width, page_height, height, width, &y, &wasted_area))
			{
				if (wasted_area < *best_wasted_area || (wasted_area == *best_wasted_area && y + width < *best_height))
				{
					*best_height = y + width;
					*best_wasted_area = wasted_area;
					*best_line = curr_line;
					new_node.x = curr_line->rect.x;
					new_node.y = y;
					new_node.width = height;
					new_node.height = width;
					*can_rotate = -1;
				}
			}
		}
	}

	return new_node;
}

int krctpck___skyline_addlevel(krctpck_rectnode** usednode_root, krctpck_rectnode* line, krctpck_rectnode* rect)
{
	krctpck_rectnode* new_line = krctpck___createRectNode(rect->rect.x, rect->rect.y + rect->rect.height, rect->rect.width, UNUSED);
	if (!new_line) return krctpck_MALLOC_FAILED;
	krctpck___insertToList(usednode_root, line, new_line);

	krctpck_rectnode* curr_line;
	for (curr_line = line; curr_line;)
	{
		krctpck_rectnode* prev_line = curr_line->prevnode;
		if (curr_line->rect.x < prev_line->rect.x + prev_line->rect.width)
		{
			int shrink = prev_line->rect.x + prev_line->rect.width - curr_line->rect.x;

			curr_line->rect.x += shrink;
			curr_line->rect.width -= shrink;

			if (curr_line->rect.width <= 0)
			{
				krctpck_rectnode* next = curr_line->nextnode;
				//prev_line = curr_line->prevnode;
				krctpck___deleteFromList(usednode_root, curr_line);
				curr_line = next;
			}
			else break;
		}
		else break;
	}
	for (curr_line = *usednode_root; curr_line;)
	{
		krctpck_rectnode* next_node = curr_line->nextnode;
		if (!next_node) break;
		if (curr_line->rect.y == next_node->rect.y)
		{
			curr_line->rect.width += next_node->rect.width;
			krctpck___deleteFromList(usednode_root, next_node);
		}
		else
			curr_line = curr_line->nextnode;
	}
	return krctpck_SUCCESS;
}

krctpck_error krctpck___skyline_packonce(krctpck_page* page, krctpck_rectin rect, krctpck_maxrects_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	krctpck_rect new_node;
	int best_width_wasted_area, best_height;
	krctpck_rectnode* best_line = NULL;
	int rot = rect.can_rotate;
	switch (method)
	{
	case krctpck_skyline_bottom_left:
		new_node = krctpck___skyline_findpos4nn_bl(page->width, page->height, rect.width, rect.height, page->lines, &best_height, &best_width_wasted_area, &best_line, &rot);
		break;
	case krctpck_skyline_min_waste_fit:
		new_node = krctpck___skyline_findpos4nn_mwf(page->width, page->height, rect.width, rect.height, page->lines, &best_height, &best_width_wasted_area, &best_line, &rot);
		break;
	}
	krctpck_rectnode* selected_node = krctpck___createRectNodeByRect(new_node);
	selected_node->index = rot;
	if (!selected_node) return krctpck_MALLOC_FAILED;

	if (best_line)
		if (krctpck___skyline_addlevel(&page->lines, best_line, selected_node))
			return krctpck_MALLOC_FAILED;
		else
			return krctpck_NOT_FIT;

	krctpck___addToList(&page->used_rect, selected_node);
	page->used_surface += selected_node->rect.width * selected_node->rect.height;
	dest_current_rect->x = selected_node->rect.x;
	dest_current_rect->y = selected_node->rect.y;
	dest_current_rect->width = selected_node->rect.width;
	dest_current_rect->height = selected_node->rect.height;
	dest_current_rect->page_index = selected_node->index * ++page->used_count;
	selected_node->index *= page->used_count;
	return krctpck_SUCCESS;
}

krctpck_error krctpck___skyline_packmultiple(krctpck_page* page, krctpck_rectinarr* arr, krctpck_skyline_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	krctpck_rectnode best_node;
	krctpck_rectnode* best_rect = NULL;
	krctpck_rectnode* best_line = NULL;
	krctpck_rectnode* curr_node = arr->root;
	krctpck_error err;

	int best_score1 = INT_MAX;
	int best_score2 = INT_MAX;

	if (!curr_node) return krctpck_ALL_PACKED;

	while (curr_node)
	{
		int score1, score2, rot = curr_node->index < 0;
		krctpck_rect new_node, * line = NULL;

		switch (method)
		{
		case krctpck_skyline_bottom_left:
			new_node = krctpck___skyline_findpos4nn_bl(page->width, page->height, curr_node->rect.width, curr_node->rect.height, page->lines, &score1, &score2, &line, &rot);
			break;
		case krctpck_skyline_min_waste_fit:
			new_node = krctpck___skyline_findpos4nn_mwf(page->width, page->height, curr_node->rect.width, curr_node->rect.height, page->lines, &score2, &score1, &line, &rot);
			break;
		}
		if (new_node.height)
		{
			if (score1 < best_score1 || (score1 == best_score1 && score2 < best_score2))
			{
				best_score1 = score1;
				best_score2 = score2;
				best_node = (krctpck_rectnode){ NULL, NULL, new_node, rot };
				best_rect = curr_node;
			}
		}
		curr_node = curr_node->nextnode;
	}
	if (!best_rect) return krctpck_NOT_FIT;

	krctpck_rectnode* selected_node = krctpck___createRectNodeByNode(best_node);
	if (!selected_node) return krctpck_MALLOC_FAILED;
	krctpck___skyline_addlevel(&page->lines, best_line, selected_node);

	krctpck___addToList(&page->used_rect, selected_node);
	page->used_surface += best_rect->rect.width * best_rect->rect.height;
	krctpck___moveList(&arr->root_used, &arr->root, best_rect);
	dest_current_rect->x = selected_node->rect.x;
	dest_current_rect->y = selected_node->rect.y;
	dest_current_rect->width = selected_node->rect.width;
	dest_current_rect->height = selected_node->rect.height;
	dest_current_rect->page_index = selected_node->index * ++page->used_count;
	dest_current_rect->group_index = selected_node->index * abs(best_rect->index);
	selected_node->index *= page->used_count;
	return krctpck_SUCCESS;
}

krctpck_rect krctpck___shelf_add_to_shelf(krctpck_rectnode* shelf, unsigned int width, unsigned int height)
{
	krctpck_rect rect;

	rect.x = shelf->rect.x;
	rect.y = shelf->rect.y;
	rect.width = width;
	rect.height = height;
	shelf->rect.x += width;
	shelf->rect.height = max(shelf->rect.height, height);

	return rect;
}

int krctpck___shelf_is_fit(krctpck_rectnode* shelf, unsigned int page_width, unsigned int page_height, unsigned int width, unsigned int height, int resizable, int* rot)
{
	if (!shelf) return 0;
	const unsigned int shelf_height = (resizable) ? page_height - shelf->rect.y : shelf->rect.height;
	if (*rot == -1 && (shelf->rect.x + width <= page_width && height <= shelf_height) && (shelf->rect.x + height <= page_width && width <= shelf_height))
		if ((width > height && width > page_width - shelf->rect.x) || (width > height && width < shelf->rect.height) || (width < height && height > shelf->rect.height && height <= page_width - shelf->rect.x))
			return 1;

	if (shelf->rect.x + width <= page_width && height <= shelf_height)
	{
		*rot = 1;
		return 1;
	}
	else if (*rot == -1 && shelf->rect.x + height <= page_width && width <= shelf_height) return 1;
	else return 0;

}

krctpck_rectnode* krctpck___shelf_look_for_shelf(krctpck_rectnode* head, int page_width, unsigned int page_height, unsigned int width, unsigned int height, int method, int* rot)
{
	krctpck_rectnode* proc_shelf = NULL;
	krctpck_rectnode* first_shelf = head->prevnode;
	krctpck_rectnode* last_shelf = head->nextnode;
	krctpck_rectnode* selected_shelf = NULL;
	long long best_value;
	switch (method)
	{
	case krctpck_shelf_next_fit:
		if (krctpck___shelf_is_fit(last_shelf, page_width, page_height, width, height, last_shelf, &rot)) return last_shelf;
		break;
	case krctpck_shelf_first_fit:
		proc_shelf = first_shelf;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				return proc_shelf;
			}
			proc_shelf = proc_shelf->prevnode;
		}
		break;
	case krctpck_shelf_best_area_fit:
		proc_shelf = first_shelf;
		best_value = LLONG_MAX;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				unsigned long surface = (page_width - proc_shelf->rect.x) * proc_shelf->rect.height;
				if (surface < best_value)
				{
					selected_shelf = proc_shelf;
					best_value = surface;
				}
			}
			proc_shelf = proc_shelf->prevnode;
		}
		return selected_shelf;
	case krctpck_shelf_worst_area_fit:
		proc_shelf = first_shelf;
		best_value = -1;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				unsigned long surface = (page_width - proc_shelf->rect.x) * proc_shelf->rect.height;
				if (surface > best_value)
				{
					selected_shelf = proc_shelf;
					best_value = surface;
				}
			}
			proc_shelf = proc_shelf->prevnode;
		}
		return selected_shelf;
	case krctpck_shelf_best_height_fit:
		proc_shelf = first_shelf;
		best_value = LONG_MAX;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				int height_diff = max(0, proc_shelf->rect.height - height);
				if (height_diff < best_value)
				{
					selected_shelf = proc_shelf;
					best_value = height_diff;
				}
			}
			proc_shelf = proc_shelf->prevnode;
		}
		return selected_shelf;
	case krctpck_shelf_best_width_fit:
		proc_shelf = first_shelf;
		best_value = LONG_MAX;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				int width_diff = page_width - proc_shelf->rect.x - width;

				if (width_diff < best_value)
				{
					selected_shelf = proc_shelf;
					best_value = width_diff;
				}
			}
			proc_shelf = proc_shelf->prevnode;
		}
		return selected_shelf;
	case krctpck_shelf_worst_width_fit:
		proc_shelf = first_shelf;
		best_value = -1;
		while (proc_shelf)
		{
			if (krctpck___shelf_is_fit(proc_shelf, page_width, page_height, width, height, proc_shelf == last_shelf, &rot))
			{
				int width_diff = page_width - proc_shelf->rect.x - width;

				if (width_diff > best_value)
				{
					selected_shelf = proc_shelf;
					best_value = width_diff;
				}
			}
			proc_shelf = proc_shelf->prevnode;
		}
		return selected_shelf;
	}
}

int krctpck___shelf_should_rotate(krctpck_rectnode* shelf, unsigned int page_width, unsigned int width, unsigned height)
{
	return (width > height && width < shelf->rect.height) || (width > height && width > page_width - shelf->rect.x) || (width < height&& height > shelf->rect.height && height <= page_width - shelf->rect.x);
}

krctpck_error krctpck___shelf_packonce(krctpck_page* page, krctpck_rectin rect, int method, krctpck_rectout* dest_current_rect)
{
	krctpck_rect new_rect = { 0, 0, 0, 0 };
	krctpck_rectnode* first_shelf = page->shelves->prevnode;
	krctpck_rectnode* last_shelf = page->shelves->nextnode;
	int rot = rect.can_rotate ? -1 : 1;
	unsigned int _;

	krctpck_rectnode* selected_shelf = krctpck___shelf_look_for_shelf(page->shelves, page->width, page->height, rect.width, rect.height, method, &rot);

	if (selected_shelf)
	{
		if (rot == -1)
			new_rect = krctpck___shelf_add_to_shelf(selected_shelf, rect.height, rect.width);
		else
			new_rect = krctpck___shelf_add_to_shelf(selected_shelf, rect.width, rect.height);
		page->used_surface += rect.width * rect.height;
	}
	else
	{

		// can add more shelf ?
		if (last_shelf->rect.y + last_shelf->rect.height + rect.height <= page->height)
		{
			krctpck_rectnode* new_shelf;
			new_shelf = krctpck___shelf_create_shelf(page->shelves, rect.height);
			if (!new_shelf) return krctpck_MALLOC_FAILED;
			if (rot == -1)
				if (krctpck___shelf_should_rotate(new_shelf, page->width, rect.width, rect.height))
					krctpck__SWAP(rect.width, rect.height, unsigned int)
				else
					rot == 1;
			new_rect = krctpck___shelf_add_to_shelf(new_shelf, rect.width, rect.height);
			page->used_surface += rect.width * rect.height;
		}
	}

	if (new_rect.height == 0)
		return krctpck_NOT_FIT;

	krctpck_rectnode* new_node = krctpck___createRectNodeByRect(new_rect);
	if (!new_node) return krctpck_MALLOC_FAILED;
	krctpck___addToList(&page->used_rect, new_node);
	dest_current_rect->x = new_rect.x;
	dest_current_rect->y = new_rect.y;
	dest_current_rect->width = new_rect.width;
	dest_current_rect->height = new_rect.height;
	new_node->index = dest_current_rect->page_index = rot * ++page->used_count;
	return krctpck_SUCCESS;
}

krctpck_error krctpck___shelf_packmultiple(krctpck_page* page, krctpck_rectinarr* arr, krctpck_shelf_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	krctpck_rect new_rect = { 0, 0, 0, 0 };
	krctpck_rectnode* first_shelf = page->shelves->prevnode;
	krctpck_rectnode* last_shelf = page->shelves->nextnode;
	int rot = rect.can_rotate ? -1 : 1;
	unsigned int _;

	krctpck_rectnode* selected_shelf = krctpck___shelf_look_for_shelf(page->shelves, page->width, page->height, rect.width, rect.height, method, &rot);

	if (selected_shelf)
	{
		if (rot == -1)
			new_rect = krctpck___shelf_add_to_shelf(selected_shelf, rect.height, rect.width);
		else
			new_rect = krctpck___shelf_add_to_shelf(selected_shelf, rect.width, rect.height);
		page->used_surface += rect.width * rect.height;
	}
	else
	{

		// can add more shelf ?
		if (last_shelf->rect.y + last_shelf->rect.height + rect.height <= page->height)
		{
			krctpck_rectnode* new_shelf;
			new_shelf = krctpck___shelf_create_shelf(page->shelves, rect.height);
			if (!new_shelf) return krctpck_MALLOC_FAILED;
			if (rot == -1)
				if (krctpck___shelf_should_rotate(new_shelf, page->width, rect.width, rect.height))
					krctpck__SWAP(rect.width, rect.height, unsigned int)
				else
					rot == 1;
			new_rect = krctpck___shelf_add_to_shelf(new_shelf, rect.width, rect.height);
			page->used_surface += rect.width * rect.height;
		}
	}

	if (new_rect.height == 0)
		return krctpck_NOT_FIT;

	krctpck_rectnode* new_node = krctpck___createRectNodeByRect(new_rect);
	if (!new_node) return krctpck_MALLOC_FAILED;
	krctpck___addToList(&page->used_rect, new_node);
	dest_current_rect->x = new_rect.x;
	dest_current_rect->y = new_rect.y;
	dest_current_rect->width = new_rect.width;
	dest_current_rect->height = new_rect.height;
	new_node->index = dest_current_rect->page_index = rot * ++page->used_count;
	return krctpck_SUCCESS;
}

krctpck_error krctpck___shelf_packmultipleAAA(krctpck_page* page, krctpck_rectinarr* arr, krctpck_shelf_heuristic_method method, krctpck_rectout* dest_current_rect)
{
	krctpck_rectnode* curr_node = arr->root;

	if (!curr_node) return krctpck_ALL_PACKED;

	float best = -1;
	float best_height = FLT_MAX;
	float best_heightw = FLT_MAX;
	krctpck_rectnode* best_node = NULL;
	while (curr_node)
	{
		if (curr_node->rect.height == best_height)
		{
			if (curr_node->rect.width < best_heightw)
			{
				best = (float)curr_node->rect.width / curr_node->rect.height;
				best_heightw = curr_node->rect.width;
				best_node = curr_node;
			}
			else if(curr_node->index < 0)
			{
				if (curr_node->rect.height < best_heightw)
				{
					best = (float)curr_node->rect.width / curr_node->rect.height;
					best_heightw = curr_node->rect.width;
					best_node = curr_node;
				}
			}
		}
		else {
			best_heightw = FLT_MAX;
			if (curr_node->rect.height < best_height)
			{
				best = (float)curr_node->rect.width / curr_node->rect.height;
				best_height = curr_node->rect.height;
				best_node = curr_node;
			}
			else if (curr_node->index < 0)
			{
				if (curr_node->rect.width < best_height)
				{
					best = (float)curr_node->rect.width / curr_node->rect.height;
					best_height = curr_node->rect.height;
					best_node = curr_node;
				}
			}
		}

		curr_node = curr_node->nextnode;
	}
	krctpck_error err = krctpck___shelf_packonce(page, (krctpck_rectin) { best_node->rect.width, best_node->rect.height, 1 }, method, dest_current_rect);
	krctpck___moveList(&arr->root_used, &arr->root, best_node);
	return err;
}

krctpck_error krctpck__packonce(krctpck_page* page, krctpck_rectin rect, int method, krctpck_rectout* dest_current_rect)
{
	switch (page->ag)
	{
	case krctpck_maxrects:
		return krctpck___maxrects_packonce(page, rect, method, dest_current_rect);
	case krctpck_skyline:
		return krctpck___skyline_packonce(page, rect, method, dest_current_rect);
	case krctpck_shelf:
		return krctpck___shelf_packonce(page, rect, method, dest_current_rect);
	}
}

krctpck_error krctpck__packmultiple(krctpck_page* page, krctpck_rectinarr* arr, int method, krctpck_rectout* dest_current_rect)
{
	switch (page->ag)
	{
	case krctpck_maxrects:
		return krctpck___maxrects_packmultiple(page, arr, method, dest_current_rect);
	case krctpck_skyline:
		return krctpck___skyline_packmultiple(page, arr, method, dest_current_rect);
	case krctpck_shelf:
		return krctpck___shelf_packmultiple(page, arr, method, dest_current_rect);
	}
}

unsigned int krctpck__round_pot(unsigned int n)
{
	unsigned int r = n--;
	r |= r >> 1;
	r |= r >> 2;
	r |= r >> 4;
	r |= r >> 8;
	r |= r >> 16;
	r++;

	return r;
}

void krctpck__page_free(krctpck_page* page)
{
	switch (page->ag)
	{
		krctpck___cleanNodeChains(page->used_rect);
	case krctpck_skyline:
	case krctpck_maxrects:
		krctpck___cleanNodeChains(page->free_rect);
		break;
	case krctpck_shelf:
		krctpck___cleanNodeChains(page->shelves->nextnode);
		free(page->shelves);
	}

}

krctpck_error krctpck__group_create(krctpck_rectinarr* arr, krctpck_rectin* rects, unsigned int count)
{
	krctpck_rectnode* root = NULL;
	for (unsigned int c = 0; c < count; c++)
	{

		krctpck_rectnode* n = krctpck___createRectNode(0, 0, rects[c].width, rects[c].height);
		n->index = rects[c].can_rotate ? -((int)c + 1) : c + 1;
		if (!n)
		{
			krctpck___cleanNodeChains(root);
			return krctpck_MALLOC_FAILED;
		}
		krctpck___addToList(&root, n);
	}
	arr->root = root;
	arr->root_used = NULL;
	return krctpck_SUCCESS;
}

krctpck_error krctpck__group_create_each(krctpck_rectinarr* arr, unsigned int* w, unsigned int* h, int* can_rotate, unsigned int count)
{
	krctpck_rectnode* root = NULL;
	for (unsigned int c = 0; c < count; c++)
	{
		krctpck_rectnode* n = krctpck___createRectNode(0, 0, w[c], h[c]);
		n->index = can_rotate[c] ? -((int)c + 1) : c + 1;
		if (!n)
		{
			krctpck___cleanNodeChains(root);
			return krctpck_MALLOC_FAILED;
		}
		krctpck___addToList(&root, n);
	}
	arr->root = root;
	arr->root_used = NULL;
	return krctpck_SUCCESS;
}

void krctpck__group_reset(krctpck_rectinarr* arr)
{
	krctpck___moveAlltoList(&arr->root, &arr->root_used);
}

void krctpck__group_free(krctpck_rectinarr* arr)
{
	krctpck___cleanNodeChains(arr->root);
	krctpck___cleanNodeChains(arr->root_used);
}
#undef UNUSED