FROM ubuntu:22.04
RUN sed -i "s/http:\/\/archive.ubuntu.com/http:\/\/mirrors.ustc.edu.cn/g" /etc/apt/sources.list && \
    apt-get update && apt-get install -y lib32z1 xinetd nodejs npm && useradd -u 8888 -m ctfer
WORKDIR /home/ctfer/
COPY ./share/escape.js ./share/flag ./share/start.sh ./
RUN npm install vm2
CMD ["/usr/sbin/xinetd", "-dontfork"]

