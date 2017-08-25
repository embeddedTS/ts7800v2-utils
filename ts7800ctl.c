#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h> 
#include <string.h>
#include <signal.h>


/**
   This is mostly a stub because as of 8/24/2017, the SiLabs code on the
   TS-7800-V2 doesn't have the ADC support, sleep, WDT. 

*/

/* To compile ts7800ctl, use the appropriate cross compiler and run the
 * command:
 *
 *   gcc -Wall -O -mcpu=arm9 -o ts7800ctl ts7800ctl.c 
 *
 * On uclibc based initrd's, the following additional gcc options are
 * necessary: -Wl,--rpath,/slib -Wl,-dynamic-linker,/slib/ld-uClibc.so.0
 */
 
 
#include "i2c-dev.h"

/*
   0,1   P1.2, CPU_CORE
   2,3   P1.3, RAM_1.35V
   4,5   P1.4, 1.2V
         P1.5, 1.8V
   6,7   P2.0, VIN (scaled 5.3%)
   8,9   P2.1  5.2V_A (scaled 44%)
   10,11 P2.2  AN_3.3V
   12,13 P2.3  ADC_4 (scaled 50%)
   14,17 reserved
   18,19 P2.4  ADC_3 (scaled 50%)
   20,21 P2.5  ADC_2 (scaled 50%)
   22,23 P2.6  ADC_1 (scaled 50%)
   24,25 P2.7  ADC_0 (scaled 50%)
   
*/ 
 
volatile unsigned int *data, *control, *status, *led;
static unsigned int verbose, addr;
static int model, done;

static int get_model(void);
static int silabs_init(void);

static void exit_gracefully(int signum) {
	fprintf(stderr, "EXITING....\n");
	signal(signum, exit_gracefully);
	done = 1;
}

static void usage(char **);
static int parsechans(const char*, unsigned int);


static int twifd;

int main(int argc, char **argv)
{
   int c;
   unsigned int val_addr=0, val_data=0;
   unsigned int otp_addr, otp_data, secs = 0;
   unsigned int display_otp=0, display_mem=0, display_mac=0;
	unsigned int display_odom=0, did_something=0, display_bday=0;
   unsigned int start_adc=0, raw=0;
   unsigned int len, odom, bday;
   char str[80];
   
	if(argc == 1) {
		usage(argv);
		return 1;
	}

   if ((model=get_model())) { 
      printf("model=%x\n", model);
      if (model != 0x7800) {
         fprintf(stderr, "Unsupported model\n");
         return 1;
      }
   }
   else {      
      printf("model=unknown\n");
      return 1;
   }
   twifd = silabs_init();
	if(twifd == -1)
	  return 1;

   signal(SIGTERM, exit_gracefully);
	signal(SIGHUP, exit_gracefully);
	signal(SIGINT, exit_gracefully);
	signal(SIGPIPE, exit_gracefully);


	while ((c = getopt(argc, argv, "s:fdr:S:A:D:nFVoOmMB")) != -1) {
	   switch(c) {
			case 's': 
			   printf("TBD: -%c option\n", c);
			   secs = strtoul(optarg, NULL, 0);
				if((secs > 0) && (secs <= (65535 * 8))){
					if((secs % 8) > 0)  secs=(secs/8)+1;
					else secs=(secs/8);
				} else if(secs > (65535 * 8))
					printf("Invalid sleep time,"
					  "maximum sleep time is 524288\n");
				did_something=1;
			   break;
			   
 			case 'f':
 			   printf("TBD: -%c option\n", c);
			   break;
			
 			case 'd':
 			   printf("TBD: -%c option\n", c);
			   break;

  			case 'r':
 			   start_adc=1;
				raw=1;
				for(len=0;len<(sizeof(str)-1);len++) 
					if(optarg[len]=='\0') break;
				strncpy(str, optarg, len);
				str[len] = '\0';
			   break;
			   
 			case 'S':
            start_adc=1;
				for(len=0;len<80;len++)
					if(optarg[len]=='\0') break;
				strncpy(str, optarg, len);
				str[len] = '\0';			   break;

 			case 'A':
 			   printf("TBD: -%c option\n", c);
 			   otp_addr = strtoul(optarg, NULL, 0);
				if(otp_addr < 256) val_addr = 1;
				else fprintf(stderr, "Invalid address," 
				  " valid address are 0-63\n");
			   break;

 			case 'D':
 			   printf("TBD: -%c option\n", c);
 			   otp_data = strtoul(optarg, NULL, 0);
				if(otp_data < 256) val_data = 1;
				else fprintf(stderr, "Invalid data,"
				  " valid data is 0-255");
			   break;

 			case 'n':
 			   printf("TBD: -%c option\n", c);
			   break;

 			case 'F':
 			   printf("TBD: -%c option\n", c);
			   break;

			case 'V':
				verbose = 1;
				break;

			case 'M':
				display_mac = 1;
				did_something=1;
				break;

			case 'O':
				display_odom = 1;
				did_something=1;
				break;

			case 'B':
				display_bday = 1;
				did_something=1;
				break;

			case 'o':
				display_otp = 1;
				did_something=1;
				addr=6;
				break;

			case 'm':
				display_mem = 1;
				did_something=1;
				addr=70;
				break;

			case 'h':	
			default:
				usage(argv);
				return 1;
	
	   }	
   }
   
   if(start_adc) {
      unsigned char data[32];		
	       
		volatile unsigned int *fpga, *sram;
		unsigned int i, loop=0;
		int chans;
		
		chans = parsechans(str, len);
		   if(chans < 0) return -1;
				   
		did_something = 1;
   
      while(! done) {		
		   memset(data, 0, 32);
		   if (read(twifd, data, 32)< 0)
		      perror("read");
		
		   for(i=0; i < 32; i++)
		      printf("%02X ", data[i]);
		   printf("\n");   
		
		   sleep(1);

      }
   }
        
	if(secs > 0) {
	   printf("TBD: implement sleep function\n");
	}
	
	if(val_data && val_addr) {
		printf("TBD: implement OTP write function\n");
	}	
		
	if(display_odom) {
      printf("TBD: implement odometer function\n");
	}
		
	if(display_bday) {
        printf("TBD: implement birthdate function\n");
	}
	
	if(display_mac) {
	    printf("TBD: implement MAC function\n");
	}

	if(display_mem || display_otp) {
	      printf("TBD: implement display memory function\n");
	}

   return 0;
}


static void usage(char **argv) 
{
   fprintf(stderr, "Usage: %s [OPTION] ...\n"
	  "Modify state of TS-7800 hardware.\n"
	  "\n"
	  "General options:\n"
	  "  -s    seconds         Number of seconds to sleep for\n"
	  "  -f                    Feed the WDT for 8s\n"
	  "  -d                    Disable the WDT\n"
	  "  -r    CHANS           Sample ADC channels CHANS, e.g. \"0-2,4\", \"1,3,4\"" 
                                 "output raw data to standard out\n"
     "  -S    CHANS           Sample ADC channels CHANS, e.g. \"0-2,4\", \"1,3,4\"" 
                                  "output string parseable data to standard out\n"
	  "  -A    ADDR            Write DATA to ADDR in one time programmable memory\n"
	  "  -D    DATA            Write DATA to ADDR in one time programmable memory\n"
	  "  -n                    Red LED on\n"
	  "  -F                    Red LED off\n"
     "  -o                    Display one time programmable data\n"
     "  -m                    Display contents of non-volatile memory\n"
     "  -M                    Display MAC address\n"
     "  -O                    Display odometer(hrs board has been running for)\n"
     "  -B                    Display birthdate\n"     
	  "  -V                    Verbose output\n"			
	  "  -h                    This help screen\n",program_invocation_short_name);
}

static int parsechans(const char *str, unsigned int len) 
{

	int chans=0, last_ch=-1, dash=0, i, j;
	//Determine which channels to sample
	for(i=0;i<len;i++) {
		if(str[i] >= '0' && str[i] <= '4') {		  
			if(dash) {
				dash = 0;
				if(last_ch < 0) {
					printf("Invalid format, Sample ADC channels CHANS, e.g. \"0-2,4\", \"1,3,4\"\n");
					printf("\tCh | Pin\n");
					printf("\t---+----\n");
					printf("\t 0 | 1\n");
					printf("\t 1 | 3\n");
					printf("\t 2 | 5\n");
					printf("\t 3 | 7\n");
					printf("\t 4 | 9\n");

					return -1;
				}
		
				for(j=last_ch; j<=(str[i]-'0'); j++)
					chans |= (1<<j);		

			} else {
				last_ch = str[i] - '0';
				chans |= 1<<(str[i] - '0');
			}

		} else if(str[i] == '-') { dash=1;
		} else if((str[i] == ',') || (str[i] == ' ')){ ; 
		} else {
			printf("Invalid format, Sample ADC channels CHANS, e.g. \"0-2,4\", \"1,3,4\"\n");
			printf("\tCh | Pin\n");
			printf("\t---+----\n");
			printf("\t 0 | 1\n");
			printf("\t 1 | 3\n");
			printf("\t 2 | 5\n");
			printf("\t 3 | 7\n");
			printf("\t 4 | 9\n");
			return -1;
		}
	}

	//channel 6 => bit 4
	if(chans & (1<<6)) {
		chans |= 1<<4;
		chans &= ~(1<<6);
	}

	//channel 7 => bit 5
	if(chans & (1<<7)) {
		chans |= 1<<5;
		chans &= ~(1<<7);
	}

	return chans;
}


static int model = 0;

static int get_model(void)
{
	FILE *proc;
	char mdl[256];

	proc = fopen("/proc/device-tree/model", "r");
	if (!proc) {
	    perror("model");
	    return 0;
	}
	fread(mdl, 256, 1, proc);
	if (strstr(mdl, "TS-7800v2"))
	   return 0x7800;
	   
	else {
	   perror("model");
	   return 0;
	}
}

static int silabs_init(void)
{
	static int fd = -1;
	fd = open("/dev/i2c-0", O_RDWR);
	if(fd != -1) {
		if (ioctl(fd, I2C_SLAVE_FORCE, 0x4a) < 0) {
			perror("Microcontroller did not ACK 0x4a\n");
			return -1;
		}
	}

	return fd;
}
