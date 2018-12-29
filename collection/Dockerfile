FROM ubuntu:18.04
RUN apt-get -y update
RUN apt-get -y install python3.6 xinetd cgroup-tools python

RUN groupadd -g 1000 collection && useradd -g collection -m -u 1000 collection -s /bin/bash

RUN mkdir /chall
COPY flag /flag
COPY build/lib.linux-x86_64-3.6/Collection.cpython-36m-x86_64-linux-gnu.so /usr/local/lib/python3.6/dist-packages/Collection.cpython-36m-x86_64-linux-gnu.so
COPY server.py /server.py
COPY starter.sh /starter.sh
RUN chmod +x /starter.sh
COPY start_server.sh /start_server.sh
RUN chmod +x /start_server.sh
COPY xinetd.conf /etc/xinetd.d/chall
COPY pow.py /pow.py
CMD /starter.sh collection
#CMD /bin/bash
