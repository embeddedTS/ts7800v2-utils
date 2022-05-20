#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <gpiod.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define CONSUMER "lcdmesg"

uint16_t lcd_bias_value;

struct hd44780 {
	struct gpiod_chip *chip;
	struct gpiod_line *en;
	struct gpiod_line *rs;
	struct gpiod_line *wr;
	struct gpiod_line *data[8];
};

void set_8bit_array(int *val, uint8_t data)
{
	val[0] = data & (1 << 0);
	val[1] = data & (1 << 1);
	val[2] = data & (1 << 2);
	val[3] = data & (1 << 3);
	val[4] = data & (1 << 4);
	val[5] = data & (1 << 5);
	val[6] = data & (1 << 6);
	val[7] = data & (1 << 7);
}

void nsleep(long int nsec)
{
	struct timespec target, leftover;
	int ret;

	target.tv_sec = 0;
	target.tv_nsec = nsec;

	ret = nanosleep(&target, &leftover);
	if(ret == -1)
		if (errno == -EINTR)
			nsleep(leftover.tv_nsec);
}
uint8_t get_8bit_array(int *val)
{
	uint8_t ret;
	ret = (val[0] << 0);
	ret |= (val[1] << 1);
	ret |= (val[2] << 2);
	ret |= (val[3] << 3);
	ret |= (val[4] << 4);
	ret |= (val[5] << 5);
	ret |= (val[6] << 6);
	ret |= (val[7] << 7);
	return ret;
}

/* https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 * Sheet 58 */
void lcd_write(struct hd44780 *lcd, uint8_t rs, uint8_t data)
{
	int val[8];
	int i = 0;
	set_8bit_array(val, data);

	gpiod_line_set_value(lcd->rs, rs);
	gpiod_line_set_value(lcd->wr, 0);
	//gpiod_line_set_value_bulk(&lcd->data, val); // TODO:  Fix bulk gpio not compatible with 7800v2.
	for(i = 0; i < 8; i++){
		gpiod_line_set_value(lcd->data[i], val[i]);
	}
	
	nsleep(60); /* tAS */
	gpiod_line_set_value(lcd->en, 1);
	nsleep(230); /* PWEH */
	gpiod_line_set_value(lcd->en, 0);
	nsleep(210); /* tH/tAH + tcycE */

	usleep(37);
}

/* Set a contrast (duty cycle) from 0 (off) to 15 (max).
 * This may need to change depending on the LCD used or the altitude */
void lcd_contrast(uint8_t duty)
{
	//fpoke32(0x1c, duty & 0xf);  // For TS-7250-V3.  TODO: replace with 7800v2 compatible function.
}

void lcd_writechars(struct hd44780 *lcd, char *dat)
{
	while(*dat)
		lcd_write(lcd, 1, *dat++);
}

void lcd_returnhome(struct hd44780 *lcd)
{
	/* Since we cannot poll busy, the write function waits the typical 37us
	 * execution time max.  Clear home must wait 1.52ms, but all other
	 * commands are 37us.
	 */
	lcd_write(lcd, 0, 0x2);
	usleep(1520);
}

void lcd_init(struct hd44780 *lcd)
{
	int ret = 0;
	int i = 0;
	int model;

	// header pin to line number conversion:
	// LCD header LCD_xx pins   08, 07, 10, 09, 12, 11, 14, 13.
	unsigned int datapins[8] = {19, 18, 21, 20, 23, 22, 25, 24}; 

	lcd->chip = gpiod_chip_open_by_number(2);
	assert(lcd->chip);

	// iterate through datapins{} to enable and set lcd_xx pins.
	for(i = 0; i < 8; i++){
		lcd->data[i] = gpiod_chip_get_line(lcd->chip, datapins[i]);
		assert(lcd->data[i]);
	}
	lcd->en = gpiod_chip_get_line(lcd->chip, 16);  //LCD_EN = LCD_05 = line 16
	assert(lcd->en);
	lcd->rs = gpiod_chip_get_line(lcd->chip, 14);  //LCD_RS = LCD_03 = line 14
	assert(lcd->rs);
	lcd->wr = gpiod_chip_get_line(lcd->chip, 17);  //LCD_WR = LCD_06 = line 17
	assert(lcd->wr);

	/* Initialize all IO as high */
	//ret = gpiod_line_request_bulk_output(&lcd->data, CONSUMER, NULL);
	for(i = 0; i<8; i++)
		ret |= gpiod_line_request_output(lcd->data[i], CONSUMER, 1);
	ret |= gpiod_line_request_output(lcd->en, CONSUMER, 1);
	ret |= gpiod_line_request_output(lcd->rs, CONSUMER, 1);
	ret |= gpiod_line_request_output(lcd->wr, CONSUMER, 1);

	/* Recover from any potential state to 8-bit mode, and set:
	 * Function Set
	 * DL = 1 (8-bits)
	 * N = 1 (2 line)
	 * F = 0 (5x8 dots)
	 */
	lcd_write(lcd, 0, 0x38);
	lcd_write(lcd, 0, 0x38);
	lcd_write(lcd, 0, 0x38);

	/*
	 * Entry Mode Set
	 * I/D = 1 (increment)
	 * S = 0 (display shift off)
	 */
	lcd_write(lcd, 0, 0x6);

	/*
	 * Display on/off Control
	 * D = 1 (display on)
	 * C = 0 (cursor off)
	 * B = 0 (cursor blink off)
	 */
	lcd_write(lcd, 0, 0xc);

	/*
	 * Clear Display
	 */
	lcd_write(lcd, 0, 0x1);

	/*
	 * Return home
	 */
	lcd_returnhome(lcd);


	lcd_contrast(lcd_bias_value);
}

int main(int argc, char **argv)
{
	struct hd44780 lcd;
	int i = 0;
	char *contrast = getenv("LCD_CONTRAST");

	/* Contrast can be 0-15.  Default to 12 if not specified. */
	if(contrast) {
		lcd_bias_value = atoi(contrast);
		if(lcd_bias_value > 0xf ) {
			fprintf(stderr, "LCD Contrast must be 0-15");
		}
	} else {
		lcd_bias_value = 12;
	}

	lcd_init(&lcd);
	if (argc == 2) {
		lcd_writechars(&lcd, argv[1]);
		return 0;
	}
	if (argc > 2) {
		lcd_writechars(&lcd, argv[1]);
		/* set DDRAM addr to second row */
		lcd_write(&lcd, 0, 0xa8);
		lcd_writechars(&lcd, argv[2]);
		return 0;
	}
	while(!feof(stdin)) {
		char buf[512];
		if (i) {
			/* XXX: this seek addr may be different for different
			 * LCD sizes!  -JO */
			lcd_write(&lcd, 0, 0xa8);
		} else {
			lcd_returnhome(&lcd);
		}
		i = i ^ 0x1;
		if (fgets(&buf[0], sizeof(buf), stdin) != NULL) {
			unsigned int len;
			buf[0x27] = 0;
			len = strlen(buf);
			if (buf[len - 1] == '\n') buf[len - 1] = 0;
				lcd_writechars(&lcd, buf);
		}
	}
	return 0;
}
