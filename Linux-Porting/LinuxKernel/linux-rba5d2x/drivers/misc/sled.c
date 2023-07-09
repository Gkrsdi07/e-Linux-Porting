#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define BUFF_SZ	80
#define GPIO_PC13	77

unsigned int maj_num = 0;
char lbuff[BUFF_SZ];

int led_init(int gpio_num)
{
int retVal=0;

	retVal = gpio_request(gpio_num, "sled");
	if(retVal < 0 ) 
		return retVal;

	retVal = gpio_direction_output(gpio_num, 1);

	return retVal;
}

void led_on(int gpio_num)
{
	gpio_set_value(gpio_num, 0);
}	

void led_off(int gpio_num)
{
	gpio_set_value(gpio_num, 1);
}	

void led_deinit(int gpio_num)
{
	gpio_free(gpio_num);
}

int my_led_open(struct inode *in, struct file *fs) 
{
	led_init(GPIO_PC13);

	printk(KERN_ALERT "THIS IS MY OPEN FUNC \n");

	return 0;
}

int my_led_close(struct inode *in, struct file *fs)
{
	led_deinit(GPIO_PC13);
	printk(KERN_ALERT "  close \n");

	return 0;
}

ssize_t my_led_write(struct file *fp, const char __user *ubuff, size_t sz, loff_t *offset)
{
	int ret;
	ret = copy_from_user(lbuff, ubuff, sz);

	if(lbuff[0]=='o' && lbuff[1]=='n')
		led_on(GPIO_PC13);
	else if(lbuff[0]=='o' && lbuff[1]=='f' && lbuff[2]=='f')
		led_off(GPIO_PC13);
	else
		printk("Invalid led operation...\n");

	return 0;
}

long my_led_ioctl(struct file *fp, unsigned int cmd, unsigned long argp)
{
	printk(KERN_ALERT " in ioctl\n");		// print the cmd 
	switch(cmd)			// Process the cmd
	{
		case 0:
			led_on(GPIO_PC13);
			break;
		case 1:
			led_off(GPIO_PC13);
			break;
		default:
			printk(KERN_ALERT "Invalid IOCTL Cmd ... \n");	
	}

	return 0;
}

int my_led_read(struct file *fp,char *ubuff,size_t sz,loff_t *offset)
{
	printk("my_led_read called...\n");

	return 0;
}

struct file_operations my_led_fops = {
	.owner		= THIS_MODULE,
	.open		= my_led_open,
	.release	= my_led_close,
	.read		= my_led_read,
	.write		= my_led_write,
	.unlocked_ioctl	= my_led_ioctl
};

int init_module(void)
{
	maj_num = register_chrdev( 0,"LED_DRV", &my_led_fops);
	printk(KERN_ALERT "Chr Dev registered with Maj Num %d\n",maj_num);
	printk(KERN_ALERT "led  module is loaded\n");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(maj_num,"LED_DRV");
	printk(KERN_ALERT "led module is unloaded\n");
}

MODULE_LICENSE("GPL");
