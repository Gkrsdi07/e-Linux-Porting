
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <led.h>
#include <asm/gpio.h>
#include <dm/lists.h>
#include <dm/uclass-internal.h>

#define LED_ON     0
#define LED_OFF    1
#define LED_TOGGLE 2
#define LED_BLINK  3

struct sled_priv {
	struct gpio_desc gpio;
};


static int sled_set_state(struct udevice *dev, unsigned int state)
{
	struct sled_priv *priv = dev_get_priv(dev);
	int ret;
	int time = 20 ;

	if (!dm_gpio_is_valid(&priv->gpio))
		return -EREMOTEIO;

	switch (state) {
		case LED_ON:
			dm_gpio_set_value(&priv->gpio,0);
			printf("\nsled is in ON state\n");
			break;
		case LED_OFF:
			dm_gpio_set_value(&priv->gpio,1);
			printf("\nsled is in OFF state\n");
			break;
		case LED_TOGGLE:
			dm_gpio_set_value(&priv->gpio,0);
			printf("sled is in ON state\n");
			udelay(100000);
			dm_gpio_set_value(&priv->gpio,1);
			printf("sled is in OFF state\n");
			udelay(100000);
			break;
		case LED_BLINK:
			printf("sled is in Blinking...\n");
			while(time--){
				dm_gpio_set_value(&priv->gpio,0);
				udelay(100000);
				dm_gpio_set_value(&priv->gpio,1);
				udelay(100000);
			}
			break;
		default:
			return -ENOSYS;
	}

	return 0;
}

static int sled_probe(struct udevice *dev)
{
        struct led_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
        struct sled_priv *priv = dev_get_priv(dev);
        const char *default_state;
        int ret;

        /* Ignore the top-level LED node */
        if (!uc_plat->label)
                return 0;

        ret = gpio_request_by_name(dev, "gpios", 0, &priv->gpio, GPIOD_IS_OUT);
        if (ret)
                return ret;

	default_state = dev_read_string(dev, "sled-default-state");
	if (default_state) {
		if (!strncmp(default_state, "on", 2))
			sled_set_state(dev, LED_ON);
		else if (!strncmp(default_state, "off", 3))
			sled_set_state(dev, LED_OFF);
		else if (!strncmp(default_state, "toggle", 6))
			sled_set_state(dev, LED_TOGGLE);
		else if (!strncmp(default_state, "blink", 5))
			sled_set_state(dev, LED_BLINK);
	}

	return 0;
}

static int sled_bind(struct udevice *parent)
{
	struct udevice *dev;
	ofnode node;
	int ret;

	dev_for_each_subnode(node, parent) {
		struct led_uc_plat *uc_plat;
		const char *label;

		label = ofnode_read_string(node, "label");
		if (!label) {
			debug("%s: node %s has no label\n", __func__,
			      ofnode_get_name(node));
			return -EINVAL;
		}
		ret = device_bind_driver_to_node(parent, "sled",
						 ofnode_get_name(node),
						 node, &dev);
		if (ret)
			return ret;
		uc_plat = dev_get_uclass_platdata(dev);
		uc_plat->label = label;

		if (ofnode_read_bool(node, "sled-default-state")) {
			struct udevice *devp;

			ret = uclass_get_device_tail(dev, 0, &devp);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static const struct led_ops sled_ops = {
	.set_state	= sled_set_state,
};

static const struct udevice_id sled_ids[] = {
	{ .compatible = "sled-testing" },
	{ }
};

U_BOOT_DRIVER(sled) = {
	.name	= "sled",
	.id	= UCLASS_LED,
	.of_match = sled_ids,
	.ops	= &sled_ops,
	.priv_auto_alloc_size = sizeof(struct sled_priv),
	.bind	= sled_bind,
	.probe	= sled_probe,
};
