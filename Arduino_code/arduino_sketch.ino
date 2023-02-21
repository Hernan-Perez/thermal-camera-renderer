
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

#define OK "AMG8833_OK"
#define ERRC "AMG8833_ERROR"
Adafruit_AMG88xx amg;
float buf[64];

char baux[3];

void setup() {
    Serial.begin(9600);

    bool status;

    status = amg.begin();
    if (!status) {
        Serial.println(ERRC);
        while (1);
    }
    else
    {
        Serial.println(OK);
    }

    delay(100); // let sensor boot up
}


void loop() { 

    amg.readPixels(buf);
    write_arr();
    Serial.println();

    delay(250);
}

void write_arr()
{
    int i, j;
    for (i = 0; i < 64; i++)
    {
        sprintf(baux, "%03d", (int)(buf[i]*10.0)); //converts float to char. FF.F -> FFF -> CCC
        Serial.print(baux);
    }
}
