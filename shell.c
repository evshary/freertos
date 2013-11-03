#include "fio.h"
#include "string-util.h"
#include "mmtest.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f
#define ESC 0x1b
#define BUF_SIZE 50
#define MAX_ARGV 10

void hello_func(int argc, char *argv[]);
void help_func(int argc, char *argv[]);
void ps_func(int argc, char *argv[]);
void system_func(int argc, char *argv[]);
void mmtest_func(int argc, char *argv[]);
void cat_func(int argc, char *argv[]);

extern char receive_byte();
extern char non_block_receive_byte();

enum {
	PS = 0,
	HELP,
	HELLO,
	SYSTEM,
	MMTEST,
	CAT,
	MAX_COMMANDS
};

typedef struct{
	char *name;
	char *description;
	void (*function)(int, char**); 
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
	},
	{
		.name = "cat",
		.description = "show the content of the file",
		.function = cat_func
	}
};

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int curr_len = 0;
	char done = 0;
	char esc_or_not = 0;

	do{
		ch[0] = non_block_receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_char] = '\0';
			printf("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			if(curr_char > 0){
				curr_char--;
				curr_len--;
				printf("\b \b");
			}
		}else if(ch[0] == ESC){
			ch[0] = non_block_receive_byte();
			if(ch[0] == '['){
				ch[0] = non_block_receive_byte();
				if(ch[0] == 'C'){
				if(curr_len > curr_char){
					printf("%c",command[curr_char]);
					curr_char++;
				}
				}else if(ch[0] == 'D'){
					curr_char--;
					printf("\b");
				}
			continue;
		}
		}else{
			command[curr_char++] = ch[0];
			curr_len++;
			printf("%c", ch[0]);
		}
	}while(!done);

}

void help_func(int argc, char *argv[]){
	int i;
	printf("This shell supports the commands following:\n");
	for(i = 0; i < MAX_COMMANDS; i++){
		printf("%s: %s\n", commands[i].name, commands[i].description);
	}
}

void hello_func(int argc, char *argv[]){
	printf("Hello World\n");
}

void ps_func(int argc, char *argv[]){
	char buf[BUF_SIZE];
	vTaskList(buf);
	printf("%s\n", buf);
}

void system_func(int argc, char *argv[]){
	char command[MAX_COMMAND_LEN];
	printf("Please enter your command:");
	read_string(command);
}

void mmtest_func(int argc, char *argv[]){
	mmtest();
}

void cat_func(int argc, char *argv[]){
	char buf[BUF_SIZE];
	int fd;
	size_t count;

	fd = fs_open(argv[1], 0, O_RDONLY);
	if(fd < 0){
		printf("No such file or directory\n");
	}else{
		do{
			count = fio_read(fd, buf, sizeof(buf));
			printf("%s", buf);
		}while(count);
	}
}

void user_shell(){
	char command[MAX_COMMAND_LEN];
	int i;
	char *argv[MAX_ARGV] = {NULL};
	int argc = 0;
	while(1){
		printf("evshary->");
		read_string(command);
		argv[argc++] = strtok(command, ' ');
		while(1){
			argv[argc] = strtok(NULL, ' ');
			if(argv[argc] == NULL)break;
			argc++;
		}
		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, argv[0])){
				commands[i].function(argc, argv);
				break;
			}
		}
	}
}
