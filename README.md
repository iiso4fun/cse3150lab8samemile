# cse3150lab8samemile
Lab 8 - Minting Non-Fungible Tokens and Xfer. 

Samuel Emile

---

Overview

This lab demonstrates how to:

- Create unique non-fungible tokens (NFTs) using C++
- Store NFTs in two digital wallets
- Transfer NFTs between wallets
- Use std::unique_ptr to manage memory safely
- Hash token data using OpenSSL's SHA-256 algorithm
- Test functionality with doctest

---

Files

- lab8.cpp: Main source file with class definitions, hashing logic, wallet management, and unit tests
- doctest.h: Header file for unit testing (included with lab8.cpp)

---

Features

- Mint NFTs with unique IDs using SHA-256 hashing
- Store NFTs in two separate wallets (wallet 0 and wallet 1)
- Transfer NFTs from one wallet to another
- Catch and handle invalid operations like transferring from an empty or same wallet
- Automatically tested using doctest

---

Compile and Run

Compile:
g++ lab8.cpp -o lab8 -lcrypto

Run:
./lab8

---

Example Output

Minted tokens in Wallet 0:
  Token[0] Name: Alpha, ID: <64-char SHA>
  Token[1] Name: Beta, ID: <64-char SHA>

Transferring a token...

(doctest logs and checks)

---

Notes

- Make sure OpenSSL is installed on your system
- Use the -lcrypto flag during compilation to link OpenSSL's crypto library
- Tested on Linux with g++ and doctest
