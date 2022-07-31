# Home Assistant Add-on: NRF24L01Bridge-v2

## Installation

Follow these steps to get the add-on installed on your system:

1. Uncomment dtparam=spi=on in /mnt/boot/config.txt and reboot.
2. Navigate in your Home Assistant frontend to **Supervisor** -> **Add-on Store**.
3. Find the "MRF24L01 Bridge v2" add-on and click it.
4. Click on the "INSTALL" button.

## How to use

The add-on has a couple of options available. To get the add-on running:

1. Start the add-on.
2. Have some patience and wait a couple of minutes.
3. Check the add-on log output to see the result.


If you have old NRF24L01Bridge settings available, remove this old integration and restart Home Assistant to see the new one.

## Configuration

Add-on configuration:

```yaml
"CE": 25,
"SPI_DEV": 0,
"MQTT_READ_TOPIC": "nrf24/read",
"MQTT_WRITE_TOPIC": "nrf24/write",
"RX_ADDRESS": "1Node", 
```

MQTT_READ_TOPIC - topic to recieve message via NRF24

MQTT_WRITE_TOPIC - topic to send message via NRF24

RX address: 1Node

## Example MQTT message

client_address: "text"

## Support

Got questions?

In case you've found a bug, please [open an issue on our GitHub][issue].
[issue] https://github.com/yakhira/NRF24L01Bridge-v2