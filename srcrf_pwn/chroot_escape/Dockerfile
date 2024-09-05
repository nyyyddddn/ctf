FROM ubuntu:22.04

# apt更换镜像源，并安装相关依赖
RUN sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list && \
    sed -i 's@//.*security.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list
RUN apt-get update && apt-get -y dist-upgrade && \
    apt-get install -y lib32z1 xinetd

RUN useradd -m ctf
WORKDIR /home/ctf

# 复制相关lib，并处理环境
RUN cp -R /usr/lib* /home/ctf

# 配置特殊管道映射
RUN mkdir /home/ctf/dev && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    chmod 666 /home/ctf/dev/*

RUN mkdir /home/ctf/bin && \
    cp /bin/sh /home/ctf/bin && \
    cp /bin/echo /home/ctf/bin/echo && \
    cp /usr/bin/cat /home/ctf/bin/cat && \
    cp /usr/bin/ls /home/ctf/bin/ls  && \
    cp /usr/bin/chmod /home/ctf/bin/chmod  && \
    cp /usr/bin/base64 /home/ctf/bin/base64  && \
    cp /usr/bin/pwd /home/ctf/bin/pwd && \
    cp /usr/bin/whoami /home/ctf/bin/whoami

# 部署xinetd服务
COPY ./config/ctf.xinetd /etc/xinetd.d/ctf
RUN echo "Blocked by ctf_xinetd" > /etc/banner_fail

# 复制容器启动脚本
COPY ./service/docker-entrypoint.sh /
RUN chmod +x /docker-entrypoint.sh

# 初始化目录权限
RUN chmod -R 777 /home/ctf
COPY ./flag /flag

# [可选]指定对外暴露端口，对于GZCTF等平台，强制EXPOSE可能会造成非预期端口泄露，请酌情启用
# EXPOSE 9999

# 指定容器入口点
ENTRYPOINT ["/bin/bash","/docker-entrypoint.sh"]
