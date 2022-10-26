ARG BUILD_FROM
FROM $BUILD_FROM

ENV WORKDIR /data
ENV APPDIR /app
ENV LANG C.UTF-8

COPY app/ $APPDIR/

WORKDIR $WORKDIR

RUN apk add git musl-dev gcc linux-headers make g++ ncurses mosquitto-clients ncurses-dev && \
    chmod +x $APPDIR/run.sh && \
    git clone https://github.com/joan2937/pigpio.git && \
    sed -i 's|ldconfig|ldconfig /etc/ld.so.conf.d|' pigpio/Makefile && \
    git clone https://github.com/tmrh20/RF24.git && \
    sed -i 's|$(LDCONFIG)$|$(LDCONFIG) /etc/ld.so.conf.d|' RF24/Makefile && \
    git clone https://github.com/tmrh20/RF24Network.git && \
    sed -i 's|@ldconfig|@ldconfig /etc/ld.so.conf.d|' RF24Network/Makefile && \
    git clone https://github.com/tmrh20/RF24Mesh.git && \
    sed -i 's|@ldconfig|@ldconfig /etc/ld.so.conf.d|' RF24Mesh/Makefile && \
    git clone https://github.com/tmrh20/RF24Gateway.git && \
    sed -i 's|@ldconfig|@ldconfig /etc/ld.so.conf.d|' RF24Gateway/Makefile && \
    cd RF24; ./configure --driver=SPIDEV; cd .. && \
    make -C pigpio; make install -C pigpio; rm -rf pigpio && \
    make -C RF24; make install -C RF24; rm -rf RF24 && \
    make -B -C RF24Network; make install -C RF24Network; rm -rf RF24Network && \
    make -B -C RF24Mesh; make install -C RF24Mesh; rm -rf RF24Mesh && \
    make -B -C RF24Gateway; make install -C RF24Gateway && \
    make -B -C RF24Gateway/examples/ncurses; rm -rf RF24Gateway && \
    g++ -Ofast -Wall -pthread -I/usr/local/include/RF24/.. -I.. -L/usr/local/lib /app/src/receive.cpp -lrf24 -o /app/bin/receive && \
    g++ -Ofast -Wall -pthread -I/usr/local/include/RF24/.. -I.. -L/usr/local/lib /app/src/send.cpp -lrf24 -o /app/bin/send

LABEL \
    io.hass.name="NRF24L01 Bridge v2" \
    io.hass.description="NRF24L01 Bridge v2." \
    io.hass.arch="${BUILD_ARCH}" \
    io.hass.type="addon" \
    io.hass.version=${BUILD_VERSION} \
    maintainer="Ruslan Iakhin <ruslan.k.yakhin@gmail.com>"

ENTRYPOINT ["/app/run.sh"]