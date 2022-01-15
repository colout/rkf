
#include "serial.h"
#include "config.h"
#include "helpers.h"


uint8_t a=0;

uint8_t this_num=0;
uint8_t last_num=0;

void countTest() {
    if (!IS_LEADER) {

        // Serial stuff
        leaderReady();
        serialWriteByte(a, 8);
        serialLeaderInit();

        a++;

        sleep_us(1000);
    }
    if (!IS_FOLLOWER) {
        // Serial stuff
        followerReady();
        uint8_t c=serialReadByte(8);
        serialFollowerInit();
        sleep_us(1000);

        if (c==0 && this_num==255 && last_num==254) {
            printf ("\ncount restarted.  re-syncing\n");
            a=0;
        }
        if (c!=a) {
            printf("error at %d. received %d\n", a, c);
            printf("bits: \n");
            printBits(sizeof(a), &a);
            printf(" vs \n");
            printBits(sizeof(c), &c);
            printf("\n");

            gpio_pull_up(D0);
            sleep_us(500);
        }
            gpio_pull_down(D0);
        last_num=this_num;
        this_num=c;
        a++;
        //printf ("%d\n", c);
    }
}