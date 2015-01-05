#include "mbed.h"
#include "ISL1208.h"

ISL1208 rtc(p28, p27);

int main()
{
    //Try to open the ISL1208
    if (rtc.open()) {
        printf("Device detected!\n");

        //Configure the oscillator for a 32.768kHz crystal
        rtc.oscillatorMode(ISL1208::OSCILLATOR_CRYSTAL);

        //Check if we need to reset the time
        if (rtc.powerFailed()) {
            //The time has been lost due to a power complete power failure
            printf("Device has lost power! Resetting time...\n");

            //Set RTC time to Wed, 28 Oct 2009 11:35:37
            rtc.time(1256729737);
        }

        while(1) {
            //Get the current time
            time_t seconds = rtc.time();

            //Print the time in various formats
            printf("\nTime as seconds since January 1, 1970 = %d\n", seconds);
            printf("Time as a basic string = %s", ctime(&seconds));
            char buffer[32];
            strftime(buffer, 32, "%I:%M %p\n", localtime(&seconds));
            printf("Time as a custom formatted string = %s", buffer);

            //Delay for 1.0 seconds
            wait(1.0);
        }
    } else {
        error("Device not detected!\n");
    }
}
