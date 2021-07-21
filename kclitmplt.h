/*

kclitmplt.h
By Kongfa Waroros (2020)(2563)
CLI Template
*/

#ifndef __klib_clitmplt_h
#define __klib_clitmplt_h

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef kclitmplt_TEMPLATE_USE_WIDE_CHARACTER
typedef char kclitmplt_char;
#define kclitmplt__LIT(c) c
#else
typedef wchar_t kclitmplt_char;
#define kclitmplt__LIT(c) L##c
#endif

#define kclitmplt_____SUCCESS 0
#define kclitmplt_____MALLOC_FAILED 1
#define kclitmplt_____NOT_FIT 2
#define kclitmplt_____SIZE_BIGGER_THAN_CANVAS 3
#define kclitmplt_____NO_INPUT 4

// WARNING BITWISE
#define kclitmplt_____UNNAMED_SHORT_ARGUMENT 1 // - -= -=a - a
#define kclitmplt_____UNNAMED_LONG_ARGUMENT 2 // -- --= --=a -- a
#define kclitmplt_____UNEXPECTED_ARGUMENT 3 // --width 100 -height 100 -pickle NO_PLEASE

#define kclitmplt_____NO_ARGUMENT_PROVIDED 12 // --ineedargument --justanotherargument --ineedargumenttoo /// Not like TOO_LOW_ARGUMENT. If this parameter haven't any argument.
#define kclitmplt_____TOO_MUCH_ARGUMENT 4 // --square-area 25 25
#define kclitmplt_____TOO_LOW_ARGUMENT 8 // --rect-area 25

#ifdef _WIN32
#include <windows.h>
#endif

WORD kclitmplt_currcol = 0xFFFF;
char kclitmplt_useColor = 1;

typedef struct {
	int opt;
	const kclitmplt_char* optfull;
	const kclitmplt_char* optdesc;
	int required_argument;
} kclitmplt_option;

typedef struct {
	int sameof;
	kclitmplt_char* keyword;
	int parameter_count;
	kclitmplt_char* parameter;

	int warning;
} kclitmplt_parameter;

typedef struct {
	int return_int;

} kclitmplt_result;

struct kclitmplt_parameter_link {
	struct kclitmplt_parameter_link* next;
	kclitmplt_parameter p;
};

WORD kclitmplt__saveOldCol()
{
	CONSOLE_SCREEN_BUFFER_INFO cmdinfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cmdinfo);
	kclitmplt_currcol = cmdinfo.wAttributes;
	return cmdinfo.wAttributes;
}

WORD kclitmplt__setConsoleColorW(WORD c)
{
	WORD prevcol = kclitmplt_currcol;
	kclitmplt_currcol = c;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
	return prevcol;
}

WORD kclitmplt__setConsoleColor(int r, int g, int b, int intensity)
{
	WORD prevcol = kclitmplt_currcol;
	kclitmplt_currcol = 0;
	if (r)
		kclitmplt_currcol |= FOREGROUND_RED;
	if (g)
		kclitmplt_currcol |= FOREGROUND_GREEN;
	if (b)
		kclitmplt_currcol |= FOREGROUND_BLUE;
	if (intensity)
		kclitmplt_currcol |= FOREGROUND_INTENSITY;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), kclitmplt_currcol);
	return prevcol;
}

void kclitmplt__printf_col(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	const char* at = format;

	WORD def = kclitmplt__saveOldCol();

	while (*at)
	{
		if (!(*at == '%'))
		{
			putchar(*at);
			at++;
			continue;
		}
		long long temp;
		char cat[3];
		cat[0] = '%';
		cat[1] = *++at;
		cat[2] = 0;
		if (*at == '%')
		{
			putchar('%');
			at++;
			continue;
		}
		if (*at == '0' || *at == '1')
		{
			if (kclitmplt_useColor)
				kclitmplt__setConsoleColor(*at - '0', *(at + 1) - '0', *(at + 2) - '0', *(at + 3) - '0');
			at += 4;
			continue;
		}

		switch (*at)
		{
		case 'p':
			printf("%p", __crt_va_arg(arg, void*));
		case 'c':
			putchar(va_arg(arg, int));
			break;
		case 's':
			printf("%s", va_arg(arg, char*));
			break;
		case 'i':
		case 'd':
			printf("%i", va_arg(arg, int));
			break;
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			printf(cat, va_arg(arg, unsigned int));
			break;
		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'a':
		case 'A':
			printf(cat, va_arg(arg, double));
			break;
		case 'r':
			if (kclitmplt_useColor)
				kclitmplt__setConsoleColorW(def);
			break;
		}
		at++;
	}
	va_end(arg);
}

void kclitmplt__wprintf_col(const wchar_t* format, ...)
{
	va_list arg;
	va_start(arg, format);
	const wchar_t* at = format;

	WORD def = kclitmplt__saveOldCol();

	while (*at)
	{
		if (!(*at == L'%'))
		{
			putwchar(*at);
			at++;
			continue;
		}
		long long temp;
		wchar_t cat[3];
		cat[0] = L'%';
		cat[1] = *++at;
		cat[2] = 0;
		if (*at == L'%')
		{
			putchar('%');
			at++;
			continue;
		}
		if (*at == L'0' || *at == L'1')
		{
			if (kclitmplt_useColor)
				kclitmplt__setConsoleColor(*at - '0', *(at + 1) - '0', *(at + 2) - '0', *(at + 3) - '0');
			at += 4;
			continue;
		}

		switch (*at)
		{
		case L'p':
			wprintf(L"%p", __crt_va_arg(arg, void*));
		case L'c':
			putwchar(va_arg(arg, wchar_t));
			break;
		case L's':
			wprintf(L"%s", va_arg(arg, char*));
			break;
		case 'i':
		case 'd':
			wprintf(L"%i", va_arg(arg, int));
			break;
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			wprintf(cat, va_arg(arg, unsigned int));
			break;
		case L'f':
		case L'F':
		case L'e':
		case L'E':
		case L'a':
		case L'A':
			wprintf(cat, va_arg(arg, double));
			break;
		case L'r':
			if (kclitmplt_useColor)
				kclitmplt__setConsoleColorW(def);
			break;
		}
		at++;
	}
	va_end(arg);
}
/*
int kclitmplt__add_options(const char** optnamedescarr, unsigned int listnum)
{
	
		kclitmplt_option* opt = (kclitmplt_option*)malloc(sizeof(kclitmplt_option) * listnum);
		if (!opt) return kclitmplt_____MALLOC_FAILED;
		char** st = (char**)calloc(listnum * 2, sizeof(char*));
		if (!st)
		{
			free(opt);
				return kclitmplt_____MALLOC_FAILED;
		}
		for (unsigned int i = 0;;)
		{
			opt[i].optname = (char*)malloc(sizeof(char) * (strlen(optnamedescarr[i * 2]) + 1));
			if (!opt[i].optname)
				break;
			st[i * 2] = opt[i].optname;
			opt[i].optfull = (char*)malloc(sizeof(char) * (strlen(optnamedescarr[i * 2 + 1]) + 1));
			if (!opt[i].optfull)
				break;
			st[i * 2 + 1] = opt[i].optfull;

			if (++i < listnum)
				continue;
			free(st);
			if (tmpl->opts)
				free(tmpl->opts);
			tmpl->opts = opt;
			tmpl->optc = listnum;
			return kclitmplt_____SUCCESS;
		}
		free(opt);
		for (unsigned int i = 0; i < listnum * 2; i++)
			free(st[i]);
		free(st);
		return kclitmplt_____MALLOC_FAILED;
}
*/
// Check while prefix is in
// ("-h") --(h ?)--> TRUE
// Positive is - or --, Negative is else
int kclitmplt__check(const char* kw, const char* in)
{
	if (memcmp(in + 1, kw, strlen(kw)))
	{
		if (memcmp(in + 1, in, 1) || memcmp(in + 2, kw, strlen(kw)) || strlen(kw) < 2)
			return 0;
		return (memcmp(in, "--", 2)) ? -2 : 2;
	}
	return (memcmp(in, "-", 1)) ? -1 : 1;
}
int kclitmplt__first_is(const char* kw, char** argv)
{
	return kclitmplt__check(kw, argv[1]);
}
int kclitmplt__exist(const char* kw, int argc, char** argv)
{
	unsigned int counter = 0;
	for (unsigned int i = 1; i < argc - 1; i++)
	{
		if (memcmp(argv[i] + 1, kw, strlen(kw)))
			if (memcmp(argv[i] + 1, argv[i], 1) || memcmp(argv[i] + 2, kw, strlen(kw)))
				continue;
		counter++;
	}
	return counter;
}

void kclitmplt___destroy_parameter_link(kclitmplt_parameter_link* root)
{
	if (!root) return;
	if (root->next) {
		kclitmplt___destroy_parameter_link(root->next);
	}
	if (root->p.keyword) free(root->p.keyword);
	if (root->p.parameter) free(root->p.parameter);
	free(root);
}

int kclitmplt__splash(const kclitmplt_char* programTitle, const kclitmplt_char* buildDate, const kclitmplt_char* programDesc, const kclitmplt_char* usageSentence, const kclitmplt_option* optnamedescarr, const unsigned int optn, kclitmplt_parameter*** param_store, unsigned int* param_count, int argc, kclitmplt_char** argv)
{
	//kclitmplt__setConsoleColor(1, 0, 1, 0);
	kclitmplt__printf_col("%1000[%1100%s%1000] %0001(%s)\n%1110%s\n%0100usage %1110: %0110%s %1100%s%r\n\n", programTitle, buildDate, programDesc, programTitle, usageSentence);

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cmdattr);
	//printf("Back to normal");
	if (argc < 2)
		kclitmplt__printf_col("%1010Type with %1100-h%1010 to get full help.%r\n");

	int c = 1;
	/*
		1 = expect next keyword
		2 = expect next value
		3 = expect '='

	*/
	int inst;
	kclitmplt_parameter_link* proot = NULL;
	kclitmplt_parameter_link* ptail = NULL;

#define kclitmplt__QUIT(r) kclitmplt___destroy_parameter_link(proot);return r;
#define kclitmplt__ADD_PARAM(kw, prmc, prmptr) kclitmplt_parameter_link* _ = (kclitmplt_parameter_link*)malloc(sizeof(kclitmplt_parameter_link));if (!_) kclitmplt__QUIT(kclitmplt_____MALLOC_FAILED); _->p.next = NULL;_->p.wrn_t = t;_->p.at_param = at;if (!warning_stack_root) warning_stack_root = p; warning_stack_tail->next = p;warning_stack_tail = p;
	while (c < argc)
	{
		kclitmplt_char* at = argv[c];
		if (*at++ == kclitmplt__LIT('-'))
		{
			if (*at++ == kclitmplt__LIT('-'))
			{
				// --asdw
				kclitmplt_char* begin = at;
				while (*at && *at != kclitmplt__LIT('='))
					at++;
				kclitmplt_char* alloc_kw = NULL;
				if (begin != at)
				{
					// not --= or --
					alloc_kw = (kclitmplt_char*)malloc(sizeof(kclitmplt_char) * (at - begin));
					if (alloc_kw) { kclitmplt__QUIT(kclitmplt_____MALLOC_FAILED) }
				}
				if (*at++ == kclitmplt__LIT('='))
				{
					// --...=
					unsigned int paramc = 0;
					while (*at != kclitmplt__LIT('-'))
					{

					}
				}
				else
				{
					// --...
				}
			}
		}
	}

	if (kclitmplt__first_is("h", argv) || kclitmplt__first_is("help", argv) || kclitmplt__first_is("?", argv))
	{
		unsigned int maxv = 0;
		for (unsigned int i = 0; i < optn; i++)
		{
			unsigned int len = strlen(optnamedescarr[i].optfull);
			if (len > maxv)
				maxv = len;
		}
		kclitmplt__printf_col("%0100usage %1110:\n\n");
	}
	return kclitmplt_____SUCCESS;
}

#endif