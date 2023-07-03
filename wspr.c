#include <stdlib.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdbool.h> 
#include <inttypes.h>
#include <pigpio.h>
#include "wspr.h"
#include "MEncode.c"
#include "si5351a.c"

struct BCData {
	char callsign[7];
	char locator[5];
	uint8_t power;
	uint16_t band;
	uint64_t freq;
	bool waitsync;
};
struct BCData OPData;


uint8_t symbolSequence[WSPR_SYMBOL_COUNT];
uint8_t tx_buffer[WSPR_SYMBOL_COUNT];



int main(int argc, char **argv) {
	int c;
	while ((c = getopt (argc, argv, "hc:l:p:b:f:m")) != -1)
                switch (c)
                {
                case 'h':
                        printf ("wspr -c f6bzg -l jn38 -p 0 -b 20m\n -c callsign -l locator -p in dbm -b band (2200,630,160,80,60,40,30,20,17,15,12,10,6,4,2) -f frequency in hz -m dont wait");
                        exit(0);
                        break;
                case 'c':
                        snprintf( OPData.callsign, 7, "%s", optarg );
                        break;
                case 'l':
                        snprintf( OPData.locator, 5, "%s", optarg );
                        break;
                case 'p':
                        OPData.power = atoi(optarg);
                        break;
                case 'b':
                        OPData.band = atoi(optarg);
							switch (OPData.band) {
							  case 2200:
								OPData.freq = FREQ2200m;
								break;
							  case 630:
								OPData.freq = FREQ630m;
								break;
							  case 160:
								OPData.freq = FREQ160m ;
								break;
							  case 80:
								OPData.freq = FREQ80m ;
								break;
							  case 60:
								OPData.freq = FREQ60m ;
								break;
							  case 40:
								OPData.freq = FREQ40m ;
								break;
							  case 30:
								OPData.freq = FREQ30m ;
								break;
							  case 20:
								OPData.freq = FREQ20m ;
								break;
							  case 17:
								OPData.freq = FREQ17m ;
								break;
							  case 15:
								OPData.freq = FREQ15m ;
								break;
							  case 12:
								OPData.freq = FREQ12m ;
								break;
							  case 10:
								OPData.freq = FREQ10m ;
								break;
							  case 6:
								OPData.freq = FREQ6m ;
								break;
							  case 4:
								OPData.freq = FREQ4m ;
								break;
							  case 2:
								OPData.freq = FREQ2m ;
								break;
							}
                        break;
                case 'f':
						if(OPData.freq == 0){OPData.freq = atoi(optarg);}
                        break;
                case 'm':
                        OPData.waitsync = false;
                        break;
                default:
                        abort ();
                }
	
	wspr_encode(OPData.callsign, OPData.locator, OPData.power, symbolSequence);
	
	for(int loop = 0; loop < WSPR_SYMBOL_COUNT; loop++) printf("%d ", symbolSequence[loop]);
	
    // gpioInitialise();
    // si5351aSetFrequency(freq);
    // gpioTerminate();
}
