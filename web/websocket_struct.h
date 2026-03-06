//
// Created by fallrain on 2026/3/5.
//

#ifndef ASIO_DEMO_WEBSOCKET_STRUCT_H
#define ASIO_DEMO_WEBSOCKET_STRUCT_H
#include <cstdint>


enum class websocket_opcode:uint8_t {
    continuation = 0x0,
    text = 0x1,
    binary = 0x2,
    close = 0x8,
    ping = 0x9,
    pong = 0xA
};


#endif //ASIO_DEMO_WEBSOCKET_STRUCT_H
