#include <ctime>      
#include <iostream>   
#include <string>     
#include <time.h>
#include <RF24/RF24.h>

using namespace std;

bool detailsShown = false;
void receive(RF24 radio, uint8_t *rx_address, bool dynamic_size);

int main(int argc, char** argv)
{
    uint16_t spi_dev = atoi(getenv("SPI_DEV"));
    uint16_t ce = atoi(getenv("CE_PIN"));
    char *rx_address = getenv("RX_ADDRESS");

    // 0 - dynamic size
    uint16_t payload_size = atoi(getenv("PAYLOAD_SIZE"));
    bool dynamic_size = false;
    // 0 - 1mbps, 1 - 2mbps, 2 - 256kbps
    uint8_t data_rate = atoi(getenv("DATA_RATE"));
    uint8_t _rx_address[6];

    memcpy(_rx_address, rx_address, 6);

    RF24 radio(ce, spi_dev);

    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0;
    }

    if (payload_size > 0) {
        radio.setPayloadSize(payload_size);
        dynamic_size = false;
    } else {
        radio.enableDynamicPayloads();
        dynamic_size = true;
    }

    radio.setPALevel(RF24_PA_LOW);

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
    receive(radio, _rx_address, dynamic_size);
    return 0;
}

void receive(RF24 radio, uint8_t *rx_address, bool dynamic_size)
{
    radio.openReadingPipe(1, rx_address); 
    uint8_t bytes = radio.getPayloadSize();

    if (dynamic_size) {
        bytes = radio.getDynamicPayloadSize();
    }

    char payload[bytes];

    radio.startListening(); 

    if (!detailsShown) {
        radio.printPrettyDetails();
    }

    cout << "----START----" << endl;

    while (true) {
        uint8_t pipe;
        if (radio.available(&pipe)) {
            payload[bytes] = 0;
            radio.read(&payload, bytes);
            cout << payload << endl;
        }
    }
}