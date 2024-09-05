#!/bin/sh

# Get the user
user=$(ls /home)

# Check the environment variables for the flag and assign to INSERT_FLAG
if [ "$DASFLAG" ]; then
    INSERT_FLAG="$DASFLAG"
    export DASFLAG=no_FLAG
    DASFLAG=no_FLAG
elif [ "$FLAG" ]; then
    INSERT_FLAG="$FLAG"
    export FLAG=no_FLAG
    FLAG=no_FLAG
elif [ "$GZCTF_FLAG" ]; then
    INSERT_FLAG="$GZCTF_FLAG"
    export GZCTF_FLAG=no_FLAG
    GZCTF_FLAG=no_FLAG
else
    INSERT_FLAG="flag{TEST_Dynamic_FLAG}"
fi

# 将FLAG写入文件 请根据需要修改
echo $INSERT_FLAG | tee /home/$user/flag

# 赋予程序运行权限 patchelf
chmod 777 /home/ctf/vuln
chmod 777 /home/ctf/ld-2.31.so
chmod 777 /home/ctf/libc-2.31.so

# patchelf
cd /home/ctf/
./patchelf --replace-needed ./libc.so.6 ./libc.so.6 /vuln
./patchelf --set-interpreter ./ld-linux-x86-64.so.2 ./vuln



/etc/init.d/xinetd start;
sleep infinity;
