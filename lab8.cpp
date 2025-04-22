#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <openssl/sha.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

class Token {
public:
    explicit Token(std::string asset)
        : name_(std::move(asset)) 
    {
        auto timestamp = std::chrono::high_resolution_clock::now()
                             .time_since_epoch()
                             .count();

        std::string raw = name_ + std::to_string(timestamp);

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(raw.data()),
               raw.size(), hash);

        std::ostringstream oss;
        for (unsigned char c : hash) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        }
        id_ = oss.str();
    }

    const std::string& getName() const { return name_; }
    const std::string& getId() const { return id_; }

private:
    std::string name_;
    std::string id_;
};

class WalletManager {
public:
    void createToken(const std::string& asset, size_t walletIdx = 0) {
        auto newToken = std::make_unique<Token>(asset);
        accessWallet(walletIdx).push_back(std::move(newToken));
    }

    void moveToken(size_t from, size_t to, size_t index) {
        if (from == to || index >= accessWallet(from).size()) {
            throw std::out_of_range("Invalid move");
        }

        auto& source = accessWallet(from);
        auto& target = accessWallet(to);

        target.push_back(std::move(source[index]));
        source.erase(source.begin() + index);
    }

    size_t getWalletSize(size_t index) const {
        return accessWallet(index).size();
    }

    const Token* inspectToken(size_t walletIdx, size_t tokenIdx) const {
        const auto& wallet = accessWallet(walletIdx);
        return tokenIdx < wallet.size() ? wallet[tokenIdx].get() : nullptr;
    }

private:
    std::vector<std::unique_ptr<Token>> w1, w2;

    std::vector<std::unique_ptr<Token>>& accessWallet(size_t index) {
        if (index == 0) return w1;
        if (index == 1) return w2;
        throw std::out_of_range("Invalid wallet index");
    }

    const std::vector<std::unique_ptr<Token>>& accessWallet(size_t index) const {
        if (index == 0) return w1;
        if (index == 1) return w2;
        throw std::out_of_range("Invalid wallet index");
    }
};

TEST_CASE("Unique token generation with correct metadata") {
    WalletManager manager;
    manager.createToken("Alpha");
    manager.createToken("Beta");

    std::cout << "Minted tokens in Wallet 0:\n";
    for (size_t i = 0; i < manager.getWalletSize(0); ++i) {
        auto token = manager.inspectToken(0, i);
        std::cout << "  Token[" << i << "] Name: " << token->getName()
                  << ", ID: " << token->getId() << "\n";
    }

    CHECK(manager.getWalletSize(0) == 2);
    const Token* token1 = manager.inspectToken(0, 0);
    const Token* token2 = manager.inspectToken(0, 1);
    REQUIRE(token1); REQUIRE(token2);
    CHECK(token1->getName() == "Alpha");
    CHECK(token2->getName() == "Beta");
    CHECK(token1->getId() != token2->getId());
    CHECK(token1->getId().length() == 64);
    CHECK(token2->getId().length() == 64);
}

TEST_CASE("Token transfer between wallets") {
    WalletManager manager;
    manager.createToken("One");
    manager.createToken("Two");

    std::cout << "\nTransferring a token...\n";

    manager.moveToken(0, 1, 0);

    CHECK(manager.getWalletSize(0) == 1);
    CHECK(manager.getWalletSize(1) == 1);
    CHECK(manager.inspectToken(0, 0)->getName() == "Two");
    CHECK(manager.inspectToken(1, 0)->getName() == "One");
}

TEST_CASE("Invalid token transfer throws exception") {
    WalletManager manager;
    manager.createToken("Solo");

    CHECK_THROWS_AS(manager.moveToken(0, 0, 0), std::out_of_range);
    CHECK_THROWS_AS(manager.moveToken(0, 1, 42), std::out_of_range);
}
