FROM ubuntu:18.04
RUN apt-get -y update
RUN apt-get -y install xinetd cgroup-tools python

RUN groupadd -g 1000 h2w && useradd -g h2w -m -u 1000 h2w -s /bin/bash

RUN mkdir /chall
COPY flag /flag
COPY js /js
RUN chmod +x /js
COPY server.py /server.py
COPY starter.sh /starter.sh
RUN chmod +x /starter.sh
COPY start_server.sh /start_server.sh
RUN chmod +x /start_server.sh
COPY xinetd.conf /etc/xinetd.d/chall
COPY pow.py /pow.py
CMD /starter.sh h2w

