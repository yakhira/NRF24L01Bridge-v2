   #include <iostream>   
#include <string>     
#include <RF24/RF24.h>

using namespace std;

void receive(RF24 radio, bool dynamic_payload);

int main(int argc, char** argv)
{
    uint16_t spi_dev = atoi(getenv("SPI_DEV"));
    uint16_t ce = atoi(getenv("CE_PIN"));

    char *rx_address = getenv("RX_ADDRESS");

    // 0 - dynamic size
    uint16_t payload_size = atoi(getenv("PAYLOAD_SIZE"));
    // 0 - 1mbps, 1 - 2mbps, 2 - 256kbps
    uint8_t data_rate = atoi(getenv("DATA_RATE"));
    uint8_t _rx_address[6];

    memcpy(_rx_address, rx_address, 6);

    RF24 radio(ce, spi_dev);

    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0;
    }

    radio.openReadingPipe(1, _rx_address); 

    if (payload_size > 0) {
        radio.setPayloadSize(payload_size);
    } else {
        radio.enableDynamicPayloads();
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
    if (argc == 2) {
        if (strcmp(argv[1], "info") == 0) {
            radio.startListening();
            radio.printPrettyDetails();
            return 0;
        }
    }
    receive(radio, payload_size);
    return 0;
}

void receive(RF24 radio, uint16_t payload_size)
{
    char payload[32];
    uint8_t bytes = radio.getPayloadSize();

    radio.startListening(); 

    while (true) {
        uint8_t pipe;
        if (radio.available(&pipe)) {
            if (payload_size == 0) {
                bytes = radio.getDynamicPayloadSize();
            }
            payload[bytes] = 0;
            radio.read(&payload, bytes);
            cout << payload << endl;
        }
    }
}