#include <iostream>

#include "uvw.hpp"
#include "vega/server/client.h"

namespace {

void UvwListen(uvw::Loop &loop) {
  auto tcp = loop.resource<uvw::TCPHandle>();

  tcp->on<uvw::ListenEvent>([](const uvw::ListenEvent &, uvw::TCPHandle &srv) {
    auto cli = srv.loop().resource<uvw::TCPHandle>();
    cli->data(std::make_shared<vega::server::ClientConnection>(*cli));

    // cli->on<uvw::CloseEvent>(
    //     [ptr = srv.shared_from_this()](const uvw::CloseEvent &,
    //                                    uvw::TCPHandle &) { ptr->close(); });
    cli->on<uvw::EndEvent>(
        [](const uvw::EndEvent &, uvw::TCPHandle &cli) { cli.close(); });

    srv.accept(*cli);
    cli->read();
  });

  tcp->bind("::", 25565);
  tcp->listen();
}

}  // namespace

int main() {
  auto loop = uvw::Loop::create();
  UvwListen(*loop);
  loop->run();
}
