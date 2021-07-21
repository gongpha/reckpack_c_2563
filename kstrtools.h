/*

kstrtools.h
By Kongfa Waroros (2020)(2563)

*/

#ifndef __strtools_h
#define __strtools_h

#ifndef nullptr
#define nullptr NULL
#endif

#if _WIN32 || _WIN64
#if _WIN64
typedef long long __ptr__;
typedef unsigned long long __uptr__;
#else
typedef long __ptr__;
typedef unsigned long __uptr__;
#endif
#endif

typedef struct {
	unsigned int index;
	unsigned int length;
	char* str;
} kstrtools__replace_item;

char* kstrtools__replace_list_strings(char* str, kstrtools__replace_item* itemlist, unsigned int count) {
	if (!str) return nullptr;
	if (!itemlist) return nullptr;
	__uptr__ src_size = strlen(str);
	unsigned int mmc = 0;
	for (unsigned int n = 0; n < count; n++)
		mmc += (itemlist[n].str ? strlen(itemlist[n].str) : 0) - itemlist[n].length;
	__uptr__ newstr_size = strlen(str) + mmc;
	if (newstr_size == 0) return nullptr;
	char* newstr = malloc(sizeof(char) * (((src_size > newstr_size) ? src_size : newstr_size) + 1));
	char* m_ = newstr;
	if (!m_) return nullptr;
	memcpy(m_, str, src_size);

	int rm = 0;
	for (unsigned int n = 0; n < count; n++)
	{
		char* srcstr = itemlist[n].str;
		unsigned int srcstr_size = (itemlist[n].str ? strlen(itemlist[n].str) : 0);
		__ptr__ diff = srcstr_size - itemlist[n].length;
		if (diff == 0)
		{
			if (itemlist[n].index < 0) continue;
			memcpy(newstr + itemlist[n].index, srcstr, itemlist[n].length);
		}
		else if (diff > 0)
		{

			char* p = newstr + newstr_size;
			while (p > (newstr + itemlist[n].index + rm + (srcstr_size - 1))) {
				*p = *(p - diff);
				p--;
			}
			memcpy(newstr + itemlist[n].index + rm, srcstr, srcstr_size);
			rm += diff;
		}
		else if (diff < 0)
		{
			memcpy(newstr + itemlist[n].index + rm, srcstr, srcstr_size);
			memcpy(newstr + itemlist[n].index + rm + srcstr_size, str + itemlist[n].index + itemlist[n].length, src_size - (itemlist[n].index + (itemlist[n].length + diff)));
			rm += diff;
		}
	}
	if (src_size > newstr_size)
	{
		char* newstr_r = realloc(newstr, newstr_size + 1);
		if (!newstr_r) {
			free(newstr);
			return nullptr;
		}
		newstr = newstr_r;
	}
	*(newstr + newstr_size) = 0;
	return newstr;
}

char* kstrtools__replace_list(char* str, char* srcstr, __uptr__* pos, unsigned int count, __uptr__* length) {
	if (!str) return nullptr;
	__uptr__ srcstr_size = 0;
	if (srcstr)
		srcstr_size = strlen(srcstr);
	__uptr__ src_size = strlen(str);
	unsigned int mmc = 0;
	for (unsigned int n = 0; n < count; n++)
		mmc += srcstr_size - length[n];
	__uptr__ newstr_size = strlen(str) + mmc;
	if (newstr_size == 0) return nullptr;
	char* newstr = malloc(sizeof(char) * (((src_size > newstr_size) ? src_size : newstr_size) + 1));
	char* m_ = newstr;
	if (!m_) return nullptr;
	memcpy(m_, str, src_size);

	int rm = 0;
	for (unsigned int n = 0; n < count; n++)
	{
		if (pos[n] < 0) continue;
		__ptr__ diff = srcstr_size - length[n];
		if (diff == 0)
		{
			if (pos[n] < 0) continue;
			memcpy(newstr + pos[n], srcstr, length[n]);
		}
		else if (diff > 0)
		{
			char* p = newstr + newstr_size;
			while (p > (newstr + pos[n] + rm + (srcstr_size - 1))) {
				*p = *(p - diff);
				p--;
			}
			memcpy(newstr + pos[n] + rm, srcstr, srcstr_size);
			rm += diff;
		}
		else if (diff < 0)
		{
			memcpy(newstr + pos[n] + rm, srcstr, srcstr_size);
			memcpy(newstr + pos[n] + rm + srcstr_size, str + pos[n] + length[n], src_size - (pos[n] + (length[n] + diff)));
			rm += diff;
		}
	}
	if (src_size > newstr_size)
	{
		char* newstr_r = realloc(newstr, newstr_size + 1);
		if (!newstr_r) {
			free(newstr);
			return nullptr;
		}
		newstr = newstr_r;
	}
	*(newstr + newstr_size) = 0;
	return newstr;
}
// Same Destination's Length
char* kstrtools__replace_list_smdstlngth(char* str, char* srcstr, __uptr__* list, unsigned int count, unsigned int length) {
	if (!str) return nullptr;
	__ptr__ diff;
	__uptr__ dst_size = 0;
	if (srcstr)
		dst_size = strlen(srcstr);
	diff = dst_size - length;
	__uptr__ src_size = strlen(str);
	__uptr__ newstr_size = strlen(str) + (diff * count);
	if (newstr_size == 0) return nullptr;
	char* newstr = malloc(sizeof(char) * (((src_size > newstr_size) ? src_size : newstr_size) + 1));
	char* m_ = newstr;
	if (!m_) return nullptr;
	memcpy(m_, str, src_size);
#define _____AT (newstr + list[n])
#define ____OAT (str + list[n])
	unsigned int rm = 0;
	if (diff == 0)
		for (unsigned int n = 0; n < count; n++)
		{
			if (list[n] < 0) continue;
			memcpy(_____AT, srcstr, length);
		}
	else if (diff > 0)
		for (unsigned int n = 0; n < count; n++)
		{
			if (list[n] < 0) continue;
			char* p = newstr + newstr_size;
			while (p > _____AT + rm + (dst_size - 1)) {
				*p = *(p - diff);
				p--;
			}
			memcpy(_____AT + rm, srcstr, dst_size);
			rm += diff;
		}
	else if (diff < 0)
	{
		for (unsigned int n = 0; n < count; n++)
		{
			if (list[n] < 0) continue;
			memcpy(_____AT - rm, srcstr, dst_size);
			memcpy(_____AT - rm + dst_size, ____OAT + length, src_size - (list[n] + (length + diff)));
			rm -= diff;
		}
		char* newstr_r = realloc(newstr, newstr_size + 1);
		if (!newstr_r) {
			free(newstr);
			return nullptr;
		}
		newstr = newstr_r;
	}
	*(newstr + newstr_size) = 0;
#undef _____AT
#undef ____OAT
	return newstr;
}

/*
	str : 3
	my string\0this is second\0ok boomer\0
	0          1               2

*/
char* kstrtools__strarr__getByIndex(char* src, unsigned int len,  int index)
{
	if (!src) return nullptr;
	if (index < 0) index += len;
	if (index >= len) return nullptr;
	return st->tokens[index];
}

#endif // !__strtools_h
