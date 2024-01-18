dfx start --clean

//In a seperate console window
icpp build-wasm
//Install wasm larger than 2mb
gzip ./build/SimSoccerServer.wasm
dfx canister create SimSoccerServer
dfx canister install SimSoccerServer --wasm ./build/SimSoccerServer.wasm.gz
dfx canister --network local call SimSoccerServer play_match '(1674211940: nat64)'

//For redeploying
dfx canister install SimSoccerServer --mode reinstall --wasm ./build/SimSoccerServer.wasm.gz
