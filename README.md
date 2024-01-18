# ICSoccerWorld Server
Backend for a soccer simulation built for ICP using ICPP. Based on code sample from book "AI Game Programming by Example".
Demo is built in C++ and uses Jolt physics Engine.

## Installation
dfx start --clean --background
icpp build-wasm
//Install wasm larger than 2mb
gzip ./build/SimSoccerServer.wasm
dfx canister create SimSoccerServer
dfx canister install SimSoccerServer --wasm ./build/SimSoccerServer.wasm.gz
//For redeploying
dfx canister install SimSoccerServer --mode reinstall --wasm ./build/SimSoccerServer.wasm.gz

### Prerequisites
dfx
icpp

## Usage

### Local
Usage examples for local query:

```bash
$ dfx canister --network local call SimSoccerServer play_match '(1674211940: nat64)'
```

## License
This project is licensed under the MIT license.

