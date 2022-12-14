#!/usr/bin/env bashio
set +u

CONFIG_PATH=/data/options.json

export SPI_DEV=$(jq --raw-output ".spi_dev" $CONFIG_PATH)
export CE_PIN=$(jq --raw-output ".ce_pin" $CONFIG_PATH)

export MQTT_READ_TOPIC=$(jq --raw-output ".mqtt_read_topic" $CONFIG_PATH)
export MQTT_WRITE_TOPIC=$(jq --raw-output ".mqtt_write_topic" $CONFIG_PATH)
export MQTT_STATUS_TOPIC=$(jq --raw-output ".mqtt_status_topic" $CONFIG_PATH)
export MQTT_HOST=$(bashio::services mqtt "host")
export MQTT_USER=$(bashio::services mqtt "username")
export MQTT_PASSWORD=$(bashio::services mqtt "password")

export RX_ADDRESS=$(jq --raw-output ".rx_address" $CONFIG_PATH)

export TX_STANDBY_INTERVAL=$(jq --raw-output ".tx_standby_interval" $CONFIG_PATH)
export PAYLOAD_SIZE=$(jq --raw-output ".payload_size" $CONFIG_PATH)
export DATA_RATE=$(jq --raw-output ".data_rate" $CONFIG_PATH)

RECEIVER=/app/bin/receive
SENDER=/app/bin/send
START=0

$RECEIVER info

start_rf_listening() {
    while read -r line;
    do
        echo "[$(date)] Received message $line" | xargs
        mosquitto_pub -h $MQTT_HOST -u $MQTT_USER  -P $MQTT_PASSWORD -t $MQTT_READ_TOPIC -m "$line"
    done < <($RECEIVER)
}
stop_rf_listening() {
    kill -2 $(pidof $RECEIVER) 
}

start_mqtt_listening() {
    while read -r message;
    do
        stop_rf_listening

        TX_ADDRESS=$(echo $message | cut -d ':' -f 1 | xargs)
        TEXT=$(echo $message | cut -d ':' -f 2 | xargs)
        STATUS=$($SENDER $TX_ADDRESS $TEXT)

        echo "[$(date)] Transmission message $TEXT to address $TX_ADDRESS $STATUS!"
        mosquitto_pub -h $MQTT_HOST -u $MQTT_USER  -P $MQTT_PASSWORD -t $MQTT_STATUS_TOPIC -m "$TX_ADDRESS:$TEXT:$STATUS"

        start_rf_listening &
    done < <(mosquitto_sub -h $MQTT_HOST -u $MQTT_USER -P $MQTT_PASSWORD -t $MQTT_WRITE_TOPIC)
}

start_rf_listening &
start_mqtt_listening