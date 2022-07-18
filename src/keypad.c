#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

struct gpiod_chip *chip;
struct gpiod_line_bulk dout;
struct gpiod_line_bulk din;

const char *key_label[16] = {
	"1", "2", "3", "UP",
	"4", "5", "6", "DOWN",
	"7", "8", "9", "2ND",
	"CLEAR", "0", "HELP", "ENTER",
};

void set_4bit_array(int *val, uint8_t data)
{
	val[0] = data & (1 << 0);
	val[1] = data & (1 << 1);
	val[2] = data & (1 << 2);
	val[3] = data & (1 << 3);
}

void scan_keypad(uint8_t *keys)
{
	int key;
	int lines[4];
	int r;
	uint8_t row, col;
	memset(keys, 0, 16);

	for (row = 0; row < 4; row++) {
		set_4bit_array(lines, ~(1 << row));
		r = gpiod_line_set_value_bulk(&dout, lines);
		assert (!r);
		r = gpiod_line_get_value_bulk(&din, lines);
		assert (!r);
		for (col = 0; col < 4; col++) {
			key = (row * 4) + col;
			if (!lines[col]) {
				keys[key] = 1;
			}
		}
	}
}

void debounce_keypad(uint8_t *keys, uint8_t *debounced)
{
	struct timeval exptime, now, maxtime;
	static struct timeval db[16];
	int i, ret;
	memset(debounced, 0, 16);

	/* Require minimum press of 50ms. */
	exptime.tv_sec = 0;
	exptime.tv_usec = 1000 * 50;

	ret = gettimeofday(&now, NULL);
	assert(!ret);
	for (i = 0; i < 16; i++) {
		if (keys[i]){
			if (!timerisset(&db[i])) {
				db[i] = now;
				continue;
			} else {
				timeradd(&db[i], &exptime, &maxtime);
				/* Debounce until exptime */
				if(timercmp(&maxtime, &now, >))
					continue;
				debounced[i] = 1;
			}
		}
		timerclear(&db[i]);
	}
}

int main()
{
	int ret, i;
	unsigned int out_pins[4] = {1, 2, 3, 4};
	unsigned int in_pins[4] = {6, 7, 8, 9};
	uint8_t keys[16], debounced[16], oldstate[16];
	memset(oldstate, 0, 16);

	chip = gpiod_chip_open_by_number(5);
	assert(chip);
	gpiod_line_bulk_init(&dout);
	gpiod_line_bulk_init(&din);
	ret = gpiod_chip_get_lines(chip, out_pins, 4, &dout);
	assert(!ret);
	ret = gpiod_chip_get_lines(chip, in_pins, 4, &din);
	assert(!ret);
	ret = gpiod_line_request_bulk_output(&dout, "keypad rows", NULL);
	assert(!ret);
	ret = gpiod_line_request_bulk_input(&din, "keypad cols");
	assert(!ret);

	while(1) {
		scan_keypad(keys);
		debounce_keypad(keys, debounced);
		for (i = 0; i < 16; i++) {
			if(keys[i] && oldstate[i])
				continue;
			if(keys[i] != oldstate[i]) {
				if(debounced[i]) {
					printf("%s\n", key_label[i]);
					oldstate[i] = 1;
				}
			}
			if(!keys[i]) {
				oldstate[i] = 0;
			}
		}
		/* Poll at ~100hz */
		usleep(10000);
	}

	return 0;
}
