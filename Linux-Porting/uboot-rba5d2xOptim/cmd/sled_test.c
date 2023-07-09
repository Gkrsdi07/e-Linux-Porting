
#include <common.h>
#include <command.h>
#include <dm.h>
#include <led.h>
#include <dm/uclass-internal.h>


static const char *const state_label[] = {
	[LEDST_OFF]	= "on",
	[LEDST_ON]	= "off",
	[LEDST_TOGGLE]	= "toggle",
};

enum led_state_t get_led_cmd(char *var)
{
	int i;

	for (i = 0; i < 4; i++) {
		if (!strncmp(var, state_label[i], strlen(var)))
			return i;
	}

	return -1;
}

int do_sled(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	enum led_state_t cmd;
	struct udevice *dev;
	int ret; 
	const char *led_label;

	if (argc < 2)
		return CMD_RET_USAGE;

	cmd = get_led_cmd(argv[2]);
	led_label = argv[1];
	ret = led_get_by_label(led_label, &dev);
	if (ret) {
		printf("LED '%s' not found (err=%d)\n", led_label, ret);
		return CMD_RET_FAILURE;
	}
	switch (cmd) {
		case LEDST_OFF:
		case LEDST_ON:
		case LEDST_TOGGLE:
			(void)led_set_state(dev, cmd);
			break;
		default:
			break;
	}
	
	return 0;
}

U_BOOT_CMD(
	sled, 4, 1, do_sled,
	"manage LEDs",
	"<led_label> on|off|toggle\n" "\tChange LED state"
);
