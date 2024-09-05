#!/bin/sh

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

touch /flag
chmod 777 /flag
# 将FLAG写入文件 请根据需要修改
echo $INSERT_FLAG | tee /flag



cd /
mkdir /initramfs_content
chmod 777 /initramfs_content
cd /initramfs_content
cpio -idmv < /initramfs.cpio



cp /flag /initramfs_content/flag
rm /flag

cd /initramfs_content
find . | cpio -o --format=newc > ../initramfs.cpio
cd ..

chmod 777 /initramfs.cpio
chmod 777 /bzImage


/etc/init.d/xinetd start;
sleep infinity;
