## 构建镜像并开启容器的命令

```
docker build -t iterator_trap .
docker run -d -p 9999:9999 iterator_trap
nc 127.0.0.1:9999
```

