#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <linux/pci.h>
#include "fpga.c"

uint32_t *fpga_base = NULL;

static void usage(void)
{
   fprintf(stderr, "Usage: %s [OPTION] ...\n"
     "\n"
     "   -c CHAN         PWM Channel number 0..7\n"
     "   -d DUTYCYCLE    Duty-cycle 0..4095 (or 0%%..100%%)\n"
     "   -g GEN          Frequency generator 0 or 1\n"
     "\n"
     "There are six PWM channels (0..5).  These are presented on the \n"
     "DIO header, pins 1,3,5,7,9,11).\n\n"
     "The other channels (6 & 7) are actually the frequency-generators 0 and 1\n"
     "respectively.  For these, the DUTYCYCLE parameter is really the\n"
     "number of period units, and the GEN parameter is really the selector\n"
     "for the units.  When GEN=0, the units are 1us, and when GEN=1, the units\n"
     "are 100us.  The period of the generator, then, is the number of period-\n"
     "units multiplied by either 1 or 100\n"
     "\n", program_invocation_short_name);
}


int main(int argc, char *argv[])
{
   int c, devmem, chan, duty, gen;
   unsigned int reg;
   volatile uint32_t *syscon;

   chan=duty=-1;
   gen=0;

   if (argc < 4) {
      usage();
      return 1;
   }

   while ((c = getopt(argc, argv, "c:d:g:h")) != -1) {
      switch(c) {
         case 'c': chan = strtoul(optarg, NULL, 0);
            if (chan < 0 || chan > 7) {
               usage();
               return 1;
            }
            break;

         case 'd': duty = strtoul(optarg, NULL, 0);
            if (strchr(optarg, '%')) {
               duty = (duty * 4095) / 100;
            }
            if (duty < 0 || duty > 4095) {
               usage();
               return 1;
            }
            break;

         case 'g':
            gen =  !!strtoul(optarg, NULL, 0);
            if (gen < 0 || gen > 1) {
               usage(); return 1;
            }
            break;
         case 'h':
            usage(); return 0;
      }
   }

   fpga_base = fpga_init();

   if (fpga_base == 0) {
      fprintf(stderr, "Warning:  Did not discover FPGA base from PCI probe\n");
   }

   syscon = fpga_base;

   if (syscon == MAP_FAILED) {
      fprintf(stderr, "Error:  Can't mmap syscon registers\n");
      return 1;
   }

   reg = syscon[0] & 0xFF;

   if (reg < 37) {
      fprintf(stderr, "Error: PWM needs FPGA Rev 37 or later.  Found Rev %d\n",
         reg);
      return 1;
   }

   reg = (1 << 29) | (chan << 21) | (gen << 20) | (duty << 8);

#if DEBUG
   printf("chan %d, gen %d, duty %d\n",
      (reg >> 21) & 7, (reg >> 20) & 1 , (reg >> 8) & 0xfff);
#endif

   syscon[0x4c / 4] = reg;

   munmap(syscon, 4096);

}

