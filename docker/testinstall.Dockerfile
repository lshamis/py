FROM alpine:3.10

RUN apk add g++ git linux-headers make py3-pip python3-dev

RUN cd / &&                                                 \
    git clone https://github.com/alephzero/alephzero.git && \
    cd /alephzero &&                                        \
    make install -j &&                                      \
    cd / &&                                                 \
    rm -rf /alephzero

COPY . /alephzero/py

WORKDIR /alephzero/py
RUN pip3 install -r requirements.txt
RUN python3 setup.py install
