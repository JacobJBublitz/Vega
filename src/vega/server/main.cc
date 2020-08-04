#include <iostream>

#include "vega/net/tcp.h"
#include "vega/server/client.h"

int main() {
  try {
    WSAData wsa_data;
    ::WSAStartup(MAKEWORD(2, 2), &wsa_data);

    vega::net::TcpServer server;
    server.Bind(25565);

    server.Listen();
    std::cout << "listening\n";

    auto client = server.Accept();
    std::cout << "Accepted connection\n";
    vega::server::ClientConnection conn{std::move(client)};

    while (true) {
      conn.Poll();
    }
  } catch (std::exception e) {
    std::cerr << e.what() << "\n";
  }
}
