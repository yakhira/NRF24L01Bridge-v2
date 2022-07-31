#include <ctime>      
#include <iostream>   
#include <string>     
#include <time.h>
#include <RF24/RF24.h>

using namespace std;

void send(RF24 radio, uint8_t *tx_address, char *message);

int main(int argc, char** argv)
{
    char *message;
    uint8_t tx_address[6];

    uint16_t spi_dev = atoi(getenv("SPI_DEV"));
    uint16_t ce = atoi(getenv("CE_PIN"));

    RF24 radio(ce, spi_dev);

    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0;
    }

    if (argc == 3) {
        message = argv[2];
        memcpy(tx_address, argv[1], 6);
    } else {
        cout << "TX address or message not set!!" << endl;
        return -1;
    }

    radio.setPayloadSize(32);
    radio.setPALevel(RF24_PA_LOW);
    send(radio, tx_address, message);
    return 0;
}

void send(RF24 radio, uint8_t *tx_address, char *message)
{
    radio.openWritingPipe(tx_address); 
    radio.stopListening(); 
    //radio.printDetails();

    bool report = radio.write(message, strlen(message));

    if (report) {
        cout << "Transmission message " << message << " to address " << tx_address << " successful!" << endl;
    }
    else {
        cout << "Transmission to address " << tx_address << " failed or timed out" << endl;
    }
}

