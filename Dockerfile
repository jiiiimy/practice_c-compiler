FROM debian:bullseye-slim
WORKDIR /app

RUN apt-get update && apt-get install -y \
    git \
    gcc \
    gdb \
    make \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

CMD /bin/sh -c "echo '準備完了' && sleep infinity"