#ifndef STRING_UTIL_H
#define STRING_UTIL_H

void *memset(void *dest, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);
char *strchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int strlen(char *string);
int strcmp(char *str1, char *str2);
int strcat(char *str1, char *str2);
void print(char * print_str);
void itoa(int num, char *num_to_str);
void htoa(int num, char *num_to_str);
void utoa(unsigned num, char *num_to_str);
void my_printf(const char *format, ...);
int sprintf(char *dest, const char *format, ...);
#endif
