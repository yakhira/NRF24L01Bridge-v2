#!/usr/bin/env bashio
set +u

CONFIG_PATH=/data/options.json

export SPI_DEV=$(jq --raw-output ".spi_dev" $CONFIG_PATH)
export CE_PIN=$(jq --raw-output ".ce_pin" $CONFIG_PATH)

export MQTT_READ_TOPIC=$(jq --raw-output ".mqtt_read_topic" $CONFIG_PATH)
export MQTT_WRITE_TOPIC=$(jq --raw-output ".mqtt_write_topic" $CONFIG_PATH)
export MQTT_HOST=$(bashio::services mqtt "host")
export MQTT_USER=$(bashio::services mqtt "username")
export MQTT_PASSWORD=$(bashio::services mqtt "password")

export RX_ADDRESS=$(jq --raw-output ".rx_address" $CONFIG_PATH)

RECEIVER=/app/bin/receive
SENDER=/app/bin/send
START=0

start_rf_listening() {
    while read -r line;
    do
        echo $line

        if [ "$line" == "----START----" ];
        then
            START=1
        else
            if [ "$START" == 1 ];
            then
                mosquitto_pub -h $MQTT_HOST -u $MQTT_USER  -P $MQTT_PASSWORD -t $MQTT_WRITE_TOPIC -m "$message"
            fi
        fi
    done < <($RECEIVER)
}
stop_rf_listening() {
    kill -2 $(pidof $RECEIVER) 
}

start_mqtt_listening() {
    while read -r message;
    do
        stop_rf_listening
        TX_ADDRESS=$(echo $message | cut -d ':' -f 1)
        TEXT=$(echo $message | cut -d ':' -f 2)
        $SENDER $TX_ADDRESS $TEXT
        start_rf_listening &
    done < <(mosquitto_sub -h $MQTT_HOST -u $MQTT_USER -P $MQTT_PASSWORD -t $MQTT_WRITE_TOPIC)
}

start_rf_listening &
start_mqtt_listening