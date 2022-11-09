#include <ctime>      
#include <iostream>   
#include <string>     
#include <unistd.h>
#include <RF24/RF24.h>

using namespace std;

#define WRITE_INTERVAL 500
#define TX_STANDBY_INTERVAL 5000

void send(RF24 radio, uint8_t *tx_address, char *message);

int main(int argc, char** argv)
{
    char *message;
    uint8_t tx_address[6];

    uint16_t spi_dev = atoi(getenv("SPI_DEV"));
    uint16_t ce = atoi(getenv("CE_PIN"));

    // 0 - dynamic size
    uint16_t payload_size = atoi(getenv("PAYLOAD_SIZE"));
    // 0 - 1mbps, 1 - 2mbps, 2 - 256kbps, 
    uint8_t data_rate = atoi(getenv("DATA_RATE"));

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

    if (payload_size > 0) {
        radio.setPayloadSize(payload_size);
    } else {
        radio.enableDynamicPayloads();
    }

    switch (data_rate)
    {
        case 0:
            radio.setDataRate(RF24_1MBPS);
            break;
        case 1:
            radio.setDataRate(RF24_2MBPS);
            break;
        case 2:
            radio.setDataRate(RF24_250KBPS);
            break;
    }

    radio.setPALevel(RF24_PA_LOW);

    send(radio, tx_address, message);
    return 0;
}

void send(RF24 radio, uint8_t *tx_address, char *message)
{
    radio.openWritingPipe(tx_address); 
    radio.stopListening(); 
    //radio.printDetails();

    radio.writeBlocking(message, strlen(message), WRITE_INTERVAL);

    if (radio.txStandBy(TX_STANDBY_INTERVAL)) {
        cout << "successful" << endl;
    }
    else {
        cout << "failed" << endl;
    }
}

