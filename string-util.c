#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)                                                                      
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#define SS (sizeof(size_t))
void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & ALIGN) && n; n--) *s++ = c;
	if (n) {
		size_t *w, k = ONES * c;
		for (w = (void *)s; n>=SS; n-=SS, w++) *w = k;
		for (s = (void *)w; n; n--, s++) *s = c;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	void *ret = dest;
	
	//Cut rear
	uint8_t *dst8 = dest;
	const uint8_t *src8 = src;
	switch (n % 4) {
		case 3 : *dst8++ = *src8++;
		case 2 : *dst8++ = *src8++;
		case 1 : *dst8++ = *src8++;
		case 0 : ;
	}
	
	//stm32 data bus width
	uint32_t *dst32 = (void *)dst8;
	const uint32_t *src32 = (void *)src8;
	n = n / 4;
	while (n--) {
		*dst32++ = *src32++;
	}
	
	return ret;
}

char *strchr(const char *s, int c)
{
	for (; *s && *s != c; s++);
	return (*s == c) ? (char *)s : NULL;
}

char *strcpy(char *dest, const char *src)
{
	const unsigned char *s = src;
	unsigned char *d = dest;
	while ((*d++ = *s++));
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	const unsigned char *s = src;
	unsigned char *d = dest;
	while (n-- && (*d++ = *s++));
	return dest;
}

int strlen(char *string){
	int len = 0;
	char *current = string;
	while(current[len] != '\0'){
		len++;
	}
	return len;
}

int strcmp(char *str1, char *str2){
	char *cur1 = str1;
	char *cur2 = str2;
	int i = 0;
	while(cur1[i] == cur2[i]){
		if(cur1[i] == '\0'){
			return 0;
		}
		i++;
	}
	return -1;
}

int strcat(char *str1, char *str2){
	int i;
	int str1_end = strlen(str1);
	for(i = 0; i < strlen(str2); i++){
		str1[str1_end++] = str2[i];
	}
	str1[str1_end] = '\0';
	return 1;
}

void print(char * print_str){
	fio_write(1, print_str, strlen(print_str));
	if(print_str[strlen(print_str)-1] == '\n')
	{
		fio_write(1, "\r", 1);
	}
}

void itoa(int num, char *num_to_str){
	int digit = 0;
	int tmp_num = num;
	int i;
	while(tmp_num != 0){
		digit++;
		tmp_num /= 10;
	}
	tmp_num = num;
	num_to_str[digit] = '\0';
	for(i = 1; i <= digit; i++){
		num_to_str[digit-i] = "0123456789"[tmp_num%10];
		tmp_num /= 10;
	}
}

void htoa(int num, char *num_to_str){
	int digit = 0;
	int tmp_num = num;
	int i;
	while(tmp_num != 0){
		digit++;
		tmp_num /= 16;
	}
	digit = digit+2;
	tmp_num = num;
	num_to_str[digit] = '\0';
	num_to_str[0] = '0';
	num_to_str[1] = 'x';
	for(i = 1; i <= digit-2; i++, tmp_num /= 16){
		num_to_str[digit-i] = "0123456789abcdef"[tmp_num%16];
	}
}

void utoa(unsigned num, char *num_to_str){
	int digit = 0;
	unsigned tmp_u = num;
	int i;
	while(tmp_u != 0){
		digit++;
		tmp_u /= 10;
	}
	tmp_u = num;
	num_to_str[digit] = '\0';
	for(i = 1; i <= digit; i++, tmp_u /= 10){
		num_to_str[digit-i] = "0123456789"[tmp_u%10];
	}
}

void my_printf(const char *format, ...){
	va_list argv_list;
	char print_ch;
	char buf[32];
	va_start(argv_list, format);
	print_ch = *format;
	typedef union {
		char arg_char;
		int arg_int;
		char *arg_string;
	} argvs;
	argvs args;
	while(print_ch != '\0'){
		if(print_ch == '%'){
			format++;
			print_ch = *format;
			switch(print_ch){
				case 'c':
					args.arg_int = va_arg(argv_list, int);
					buf[0] = (char)args.arg_int;
					buf[1] = '\0';
					print(buf);
					break;
				case 'd':
					args.arg_int = va_arg(argv_list, int);
					itoa(args.arg_int, buf);
					print(buf);
					break;
				case 's':
					args.arg_string = va_arg(argv_list, char*);
					print(args.arg_string);
					break;
				case 'p':
					args.arg_int = va_arg(argv_list, int);
					if(args.arg_int == NULL){
						print("<nil>");
					}else{
						htoa(args.arg_int, buf);
						print(buf);
					}
					break;
				default:
					buf[0] = print_ch;
					buf[1] ='\0';
					print(buf);
			}
		}else{
			buf[0] = print_ch;
			buf[1] ='\0';
			print(buf);
		}

		format++;
		print_ch = *format;
	}

	va_end(argv_list);
}

int sprintf(char *dest, const char *format, ...){
	va_list argv_list;
	char *pstr = dest;
	va_start(argv_list, format);
	typedef union {
		unsigned arg_unsigned;
		int arg_int;
		char *arg_string;
	} argvs;
	argvs args;
	while(*format != '\0'){
		if(*format == '%'){
			format++;
			switch(*format){
				case 'c':
					args.arg_int = va_arg(argv_list, int);
					*pstr++ = (char)args.arg_int;
					break;
				case 'd':
					args.arg_int = va_arg(argv_list, int);
					itoa(args.arg_int, pstr);
					pstr += strlen(pstr);
					break;
				case 's':
					args.arg_string = va_arg(argv_list, char*);
					strcpy(pstr, args.arg_string);
					pstr += strlen(pstr);
					break;
				case 'p':
					args.arg_int = va_arg(argv_list, int);
					if(args.arg_int == NULL){
						strcpy(pstr, args.arg_int);
						pstr += strlen(pstr);
					}else{
						htoa(args.arg_int, pstr);
						pstr += strlen(pstr);
					}
					break;
				case 'u':
					args.arg_unsigned = va_arg(argv_list, unsigned);
					utoa(args.arg_unsigned, pstr);
					pstr += strlen(pstr);
					break;
				default:
					*pstr++ = *format;
			}
		}else{
			*pstr++ = *format;
		}
		format++;
	}
	*pstr = '\0';
	va_end(argv_list);
	return dest - pstr;
}
