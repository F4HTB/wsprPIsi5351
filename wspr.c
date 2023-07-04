#include <stdlib.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdbool.h> 
#include <inttypes.h>
#include <pigpio.h>
#include <sys/time.h>
#include<signal.h>
#include "wspr.h"
#include "MEncode.c"
#include "si5351a.c"

struct sigaction old_action;

struct BCData {
	char callsign[7];
	char locator[5];
	uint8_t power;
	uint16_t band;
	uint64_t freq;
	bool waitsync;
	uint8_t repeatncycle;
};
struct BCData OPData;


uint8_t symbolSequence[WSPR_SYMBOL_COUNT];
uint8_t tx_buffer[WSPR_SYMBOL_COUNT];

unsigned long long millis() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    unsigned long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

// Wait for the system clock's minute to reach one second past 'minute'
void waitWindows(int minute){
  time_t t;
  struct tm* ptm;
  char ttime[4];
  for(;;){
    time(&t);
    ptm = gmtime(&t);
    if((ptm->tm_min % minute) == 0 && ptm->tm_sec == 0) break;
	sprintf(ttime, "%3ds", 120-((ptm->tm_min % minute) *60 + ptm->tm_sec));
	printf("\b\b\b\b%s",ttime);
	fflush(stdout);
    usleep(1000);
  }
  usleep(1000000); // wait another second
}

void sigint_handler(int sig_no)
{
	si5351aOutputOffAll();
    printf("CTRL-C pressed\n");
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}

int main(int argc, char **argv) {
	
	int c;
	OPData.waitsync = true;
	OPData.repeatncycle = 2;
	
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
	
	while ((c = getopt (argc, argv, "hc:l:p:b:f:mr:")) != -1)
                switch (c)
                {
                case 'h':
                        printf ("wspr -c f6bzg -l jn38 -p 0 -b 20m -r 4\n -c callsign\n -l locator\n -p in dbm\n -b band (2200,630,160,80,60,40,30,20,17,15,12,10,6,4,2)\n -r repeat, n cycle to wait\n -f frequency in hz -m dont wait");
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
						if(OPData.freq == 0){OPData.freq = atoi(optarg)*100ULL;}
                        break;
                case 'm':
                        OPData.waitsync = false;
                        break;
                case 'r':
                        OPData.repeatncycle = false;
                        break;
                default:
                        abort ();
                }
	
	wspr_encode(OPData.callsign, OPData.locator, OPData.power, symbolSequence);
	
	si5351ainit();
	
	unsigned long long startmillis;
	unsigned long long endmillis;
	uint8_t wrc = 1;
	while(true){
		
		uint64_t center_freq_desired = OPData.freq + (2.0*rand()/((double)RAND_MAX+1.0)-1.0)*(WSPR_RAND_OFFSET);
		
		// Wait for WSPR transmission window to arrive.
		if (OPData.waitsync) {
			while(wrc>0){
				printf("Waiting for next WSPR transmit on %" PRIu64 " window in %d cycles and      ", center_freq_desired, wrc-1);
				fflush(stdout);
				waitWindows(2);
				printf("\n");
				wrc--;
			}
		}
		
		
		
		startmillis = millis();
		
		for(uint8_t loop = 0; loop < WSPR_SYMBOL_COUNT; loop++) 
		{
			endmillis = startmillis + ((loop + 1) * (unsigned long) 683) ;
			// printf("%d ", symbolSequence[loop]);
			si5351aSetFrequency(OPData.freq+(symbolSequence[loop] * 146));
			while (millis() < endmillis){};
		}
		si5351aOutputOffAll();
		wrc=OPData.repeatncycle;
		waitWindows(1);
	}
}
