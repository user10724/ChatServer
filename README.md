# ChatServer

这是一个聊天服务器，目前还没有完成,我会尽快完成。



# 1. install nginx   
```
    git clone https://github.com/nginx/nginx.git
    cd nginx && ./auto/configure --with-stream
    sudo make && sudo make install
```
# 2. modify nginx.conf

add to /usr/local/nginx/nginx.conf

```
stream {
    upstream MyServer {
        server 127.0.0.1:6000 weight max_fails=3 fail_timeout=30s;
        server 127.0.0.1:6002 weight max_fails=3 fail_timeout=30s;
    }

    server {
        proxy_connect_timeout 1s;
        #proxy_timeout 3s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}

```
