#include "fio.h"
#include "string-util.h"
#include "mmtest.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f
#define BUF_SIZE 50

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

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int done = 0;
	
	do{
		ch[0] = receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_char] = '\0';
			my_printf("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			curr_char--;
			my_printf("\b \b\0");
		}else{
			command[curr_char++] = ch[0];
			my_printf("%c", ch[0]);
		}
	}while(!done);

}

void help_func(){
	int i;
	my_printf("This shell supports the commands following:\n");
	for(i = 0; i < MAX_COMMANDS; i++){
		my_printf("%s: %s\n", commands[i].name, commands[i].description);
	}
}

void hello_func(){
	my_printf("Hello World\n");
	char dest[BUF_SIZE];
	char *d = "abc";
	sprintf(dest, "%d %d", 123, 10);
	//my_printf("123123\n");
	my_printf("%s", d);
}

void ps_func(){
	char buf[BUF_SIZE];
	vTaskList(buf);
	my_printf("%s\n", buf);
}

void system_func(){
	char command[MAX_COMMAND_LEN];
	my_printf("Please enter your command:");
	read_string(command);
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
		my_printf("evshary->");
		read_string(command);

		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, command)){
				commands[i].function();
				break;
			}
		}
	}
}
