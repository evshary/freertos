#include <stdarg.h>
#include "fio.h"
#include "mmtest.h"

#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f

void hello_func();
void help_func();
void ps_func();
void system_func();
void mmtest_func();

extern char receive_byte();

enum {
	PS = 0,
	HELP,
	HELLO,
	SYSTEM,
	MMTEST,
	MAX_COMMANDS
};

typedef struct{
	char *name;
	char *description;
	void (*function)(void);
} shell_cmd;

shell_cmd commands[] = {
	{
		.name = "ps",
		.description = "show the process now",
		.function = ps_func
	},
	{
		.name = "help",
		.description = "show the introduction to the commands",
		.function = help_func
	},
	{
		.name = "hello",
		.description = "saying hello",
		.function = hello_func
	},
	{
		.name = "system",
		.description = "execute the host command",
		.function = system_func
	},
	{
		.name = "mmtest",
		.description = "test memory allocation",
		.function = mmtest_func
	}
};

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

void print(char * print_str){
	fio_write(1, print_str, strlen(print_str));
	if(print_str[strlen(print_str)-1] == '\n')
	{
		fio_write(1, "\r", 1);
	}
}

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int done = 0;
	
	do{
		ch[0] = receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_char] = '\0';
			print("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			curr_char--;
			print("\b \b\0");
		}else{
			command[curr_char++] = ch[0];
			print(ch);
		}
	}while(!done);

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

void my_printf(const char *format, ...){
	va_list argv_list;
	char *ptr_char;
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

void help_func(){
	int i;
	print("This shell supports the commands following:\n");
	for(i = 0; i < MAX_COMMANDS; i++){
		print(commands[i].name);
		print(": ");
		print(commands[i].description);
		print("\n");
	}
}

void hello_func(){
	print("Hello World\n");
}

void ps_func(){
	my_printf("ps:%d %c %s\n", 23, 'a', "abc123");
}

void system_func(){
	char command[MAX_COMMAND_LEN];
	print("Please enter your command:");
	read_string(command);
	//union sys_param func_param[2] = {
	//	{.pCHAR = command},
	//	{.pINT = strlen(command)}
	//};
	//host_call(0x12, func_param);
}

void mmtest_func(){
    int i, size;
    char *p;

    while (1) {
        size = prng() & 0x7FF;
        DBGPRINTF1("try to allocate %d bytes\n", size);
        p = (char *) pvPortMalloc(size);
        DBGPRINTF1("malloc returned %p\n", p);
        if (p == NULL) {
            // can't do new allocations until we free some older ones
            while (circbuf_size() > 0) {
                // confirm that data didn't get trampled before freeing
                struct slot foo = read_cb();
                p = foo.pointer;
                lfsr = foo.lfsr;  // reset the PRNG to its earlier state
                size = foo.size;
                my_printf("free a block, size %d\n", size);
                for (i = 0; i < size; i++) {
                    unsigned char u = p[i];
                    unsigned char v = (unsigned char) prng();
                    if (u != v) {
                        DBGPRINTF2("OUCH: u=%02X, v=%02X\n", u, v);
                        return 1;
                    }
                }
                vPortFree(p);
                if ((prng() & 1) == 0) break;
            }
        } else {
            my_printf("allocate a block, size %d\n", size);
            write_cb((struct slot){.pointer=p, .size=size, .lfsr=lfsr});
            for (i = 0; i < size; i++) {
                p[i] = (unsigned char) prng();
            }
        }
    }
}

void user_shell(){
	char command[MAX_COMMAND_LEN];
	int i;
	while(1){
		print("evshary->");
		read_string(command);

		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, command)){
				commands[i].function();
				break;
			}
		}
	}
}
