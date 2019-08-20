FROM alpine:3.10

RUN apk add g++ py3-pip python3-dev

COPY . /alephzero/py

WORKDIR /alephzero/py
RUN pip3 install -r requirements.txt
RUN python3 setup.py install
