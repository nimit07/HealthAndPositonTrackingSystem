
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <TinyGPS.h>
#define SSID        "NITAP_Admin"
#define PASS        "1234567890"
//#define SSID        "TIGER hostel"
//#define PASS        "ughostel@boys"
#define DEST_HOST   "172.16.30.195"
#define DEST_IP     "172.16.30.195"
#define TIMEOUT     2000 // mS
#define DEVICE_ID "5727e69b68645f320b948bc9" 

TinyGPS gps;
//setup software Serial for GPS at pin 5(RX),4(TX)
SoftwareSerial gpsSerial(5, 4);
//setup software Serial for ESP8266 at pin 6(RX),7(TX)
SoftwareSerial esp8266Serial(6, 7);
int pulse[5]={0},sat=8;
// Sensor and pins variables
int pulsePin = 0;
int blinkPin = 13;
int sosLedPin=8;
int sosPin=9;
int counter=0;
int stateSos=0;
int satallite=0,prec=0;
String _lat,_lon;

// Pulse rate variable
volatile int BPM;
int counts=0;

// Raw signal
volatile int Signal;

// Interval between beats
volatile int IBI = 600;

// Becomes true when the pulse is high
volatile boolean Pulse = false;

// Becomes true when Arduino finds a pulse
volatile boolean QS = false;

// communicate and execute AT commands in esp8266 module 
void executeCommand(String cmd){
    esp8266Serial.println(cmd);
}
//connect to access point 
void connectWiFi()
{
    String cmd = "AT+CWJAP=\""; cmd += SSID; cmd += "\",\""; cmd += PASS; cmd += "\"";
    for(int i=0;i<2;i++)
    {executeCommand(cmd);
        delay(1000);
    }
}
//prepare data and send data to base station through put request
void sendData(int _counter){
    //start tcp connection at base station
    String cmd = "AT+CIPSTART=\"TCP\",\""; cmd += DEST_IP; cmd += "\",3030";
    executeCommand(cmd);
    delay(1000);
    // prepare body for put request
    String postData="{\"pulse\": ";
    postData+=String(pulse[_counter]);
    postData+=",\"gpsData\":{\"lat\": ";
    postData+=_lat;
    postData+=", \"lon\":";
    postData+=_lon;
    postData+=",\"sat\":";
    postData+=String(satallite);
    postData+=",\"prec\":";
    postData+=String(prec)+"}";
    postData+=",\"sosState\":";
    postData+=String(stateSos)+"}";
    // Build HTTP request.
    cmd = "PUT /api/device/"+String(DEVICE_ID)+" HTTP/1.1\r\nUser-Agent: Arduino/1.0\r\n Host:";
    cmd += DEST_HOST;
    cmd += ":3030\r\n";
    cmd+="Content-Type: application/json\r\n";
    cmd+="Content-Length:";
    cmd+=String(postData.length());
    cmd+="\r\n\r\n";
    cmd+=postData;
    //print request to Serial
    Serial.println(cmd);
    // Ready the module to receive raw data
    executeCommand("AT+CIPSEND="+String(cmd.length()));
    executeCommand(cmd);
    delay(500);
    // executeCommand("AT+CIPCLOSE");
    // Loop forever echoing data received from destination server.

    //   while (esp8266Serial.available())
    // Serial.write(esp8266Serial.read());
}
void setup()
{
    Serial.begin(115200);
    esp8266Serial.begin(115200);
    gpsSerial.begin(9600);
    pinMode(sosLedPin, OUTPUT);
    // initialize the switch pin as an input:
    pinMode(sosPin, INPUT);
    //writing sos led pin as low(turn off)
    digitalWrite(sosLedPin, LOW);
    Serial.print("AT");
// set the station mode to default mode router
    executeCommand("AT+CWMODE=1");
    //  AT+CIPMUX= - Value can be '0' (single IP) or '1' (multi IP)
    executeCommand("AT+CIPMUX=0");
// connect to access point
    connectWiFi();
    //set up interrupt 
    interruptSetup();
}

void loop()
{
    // delay(1000);
    //disable timer 2 to encode,decode gps data and send data to base station through put request from esp8266 module
    TCCR2A = 0x00;// ENABLE PWM ON DIGITAL PINS 3 AND 11
    TIMSK2 = 0x00;// DISABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
    // read sos signal input from pin 10
    stateSos = digitalRead(sosPin);

    // check if the switch is pressed.
    // if it is, the buttonState is HIGH:
    if (stateSos == 1 ) {
        Serial.print(stateSos);
        Serial.print("HIGH");
        // turn LED on:
        digitalWrite(sosLedPin,HIGH);
    }
    else{
        Serial.print("LOW");
        //  turn LED off
        digitalWrite(sosLedPin,LOW);
    }
    // loop to send gps data, pulse counts and sos state to basse station 
    for (unsigned long duration = millis(); millis() - duration < 7000;)
    {
        // loop  to get gps data
        for (unsigned long duration = millis(); millis() - duration < 2000;)
        gps_data();

        int _counter=0;
        for (unsigned long duration = millis(); millis() - duration < 5000;)
        {
            //prepare and data to send to base station
            sendData((_counter++)%counter);
        }
    }
    // set timer two to two milliseconds for pulse readings
    TCCR2A = 0x02;// DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
    TIMSK2 = 0x02;// ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
    for (unsigned long duration = millis(); millis() - duration < 4000;)
    {//counter=0;
        pulse_data();
    }
    delay(1000);


}
//get gps data
void gps_data(){
    bool newData = false;
    unsigned long chars;
    unsigned short sentences, failed;

    // For one second we parse GPS data and report some key values

    for (unsigned long start = millis(); millis() - start < 1000;)
    {
        while (gpsSerial.available())
        {

            char c = gpsSerial.read();
//       Serial.write(c); // uncomment this line if you want to see the GPS data flowing
            if (gps.encode(c)) // Did a new valid sentence come in?
                newData = true;
        }
    }

    if (newData)
    {
        float flat, flon;
        unsigned long age;
        char latitude[9],longitude[9];
        gps.f_get_position(&flat, &flon, &age);
        _lat="";_lon="";
        //convert latitude(float) value to latitude(String) 
        dtostrf((flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat),8,6,latitude);
        for(int i=0;i<sizeof(latitude);i++)
        {
            _lat+=latitude[i];
        }
        //convert longitude(float) value to longitude(String) 
        dtostrf((flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon),8,6,longitude);
        for(int i=0;i<sizeof(longitude);i++)
        {
            _lon+=longitude[i];
        }
        //set satallite value
        satallite=(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        
        //set precision value
        prec=(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());

    }
        //print chars,sentences length and error code to Serial in case any occur 
    gps.stats(&chars, &sentences, &failed);
    Serial.print(" CHARS=");
    Serial.print(chars);
    Serial.print(" SENTENCES=");
    Serial.print(sentences);
    Serial.print(" CSUM ERR=");
    Serial.println(failed);
    if (chars == 0)
        Serial.println("** No characters received from GPS: check wiring **");

}
//count pulse
void pulse_data(){
    if (QS == true) {


        pulse[(counter++)%5]=BPM;
        if(counter>=5)
            counter=0;
        // Reset the Quantified Self flag for next time      
        QS = false;
    }

    // Wait 20 ms
    delay(20);
}
