FROM docker.io/alpine:3.23.3 AS builder

WORKDIR /luaumb

RUN apk add --no-cache \
    cmake \
    ninja \
    build-base \
    linux-headers

COPY CMakeLists.txt .
COPY src ./src
COPY submodules ./submodules

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLUAUMB_STATIC_LINUX=ON \
    && cmake --build build

FROM scratch

COPY --from=builder /luaumb/build/luaumb /luaumb

ENTRYPOINT [ "/luaumb" ]

CMD [ "/luaumb" ]
