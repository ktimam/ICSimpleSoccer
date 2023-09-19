# syntax=docker.io/docker/dockerfile:1.4
FROM cartesi/toolchain:0.12.0 as dapp-build

WORKDIR /opt/cartesi/dapp
COPY . .
RUN make CXX=riscv64-cartesi-linux-gnu-g++
