#include<common.h>
#include<command.h>
#include<console.h>
#include<linux/delay.h>
static int do_hello(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	printf("Hi I'm From Hellocmd\n");
	 printf("arg1 : %s\n",argv[1]);
	return 0;
}


U_BOOT_CMD(hello,1,0,do_hello,"hello cmd Introduction","start - Reset the hello.\n");
