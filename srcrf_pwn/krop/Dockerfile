FROM ubuntu:22.04

# apt更换镜像源，并安装相关依赖
RUN sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list && \
    sed -i 's@//.*security.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list
RUN apt-get update && apt-get -y dist-upgrade && \
    apt-get install -y lib32z1 xinetd && \
	apt-get install -y qemu-system-x86 && \
	apt-get install -y cpio

# 部署xinetd服务
COPY ./config/ctf.xinetd /etc/xinetd.d/ctf
RUN echo "Blocked by ctf_xinetd" > /etc/banner_fail

# 复制容器启动脚本
COPY ./service/docker-entrypoint.sh /
RUN chmod +x /docker-entrypoint.sh

# 部署程序
COPY ./src/initramfs.cpio /initramfs.cpio
COPY ./src/startvm.sh /startvm.sh
COPY ./src/bzImage /bzImage

# [可选]指定对外暴露端口，对于GZCTF等平台，强制EXPOSE可能会造成非预期端口泄露，请酌情启用
# EXPOSE 9999

# 指定容器入口点
ENTRYPOINT ["/bin/bash","/docker-entrypoint.sh"]
