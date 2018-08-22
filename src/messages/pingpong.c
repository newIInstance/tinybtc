#include <stdint.h>
#include <stdlib.h>
#include "pingpong.h"
#include "messages/shared.h"

int32_t make_pingpong_message(
    Message *ptrMessage,
    PingpongPayload *ptrPayload,
    char *command
) {
    ptrMessage->header.magic = parameters.magic;
    strcpy(ptrMessage->header.command, command);

    ptrMessage->ptrPayload = malloc(sizeof(PingpongPayload));
    memcpy(ptrMessage->ptrPayload, ptrPayload, sizeof(PingpongPayload));

    Byte buffer[MAX_MESSAGE_LENGTH] = {0};
    uint64_t payloadLength = serialize_pingpong_payload(ptrPayload, buffer);
    ptrMessage->header.length = (uint32_t)payloadLength;
    calculate_data_checksum(
        &buffer,
        ptrMessage->header.length,
        ptrMessage->header.checksum
    );
    return 0;
}

int32_t make_ping_message(
    Message *ptrMessage,
    PingpongPayload *ptrPayload
) {
    return make_pingpong_message(ptrMessage, ptrPayload, CMD_PING);
}

int32_t make_pong_message(
    Message *ptrMessage,
    PingpongPayload *ptrPayload
) {
    return make_pingpong_message(ptrMessage, ptrPayload, CMD_PONG);
}


uint64_t serialize_pingpong_payload(
    PingpongPayload *ptrPayload,
    Byte *ptrBuffer
) {
    Byte *p = ptrBuffer;
    p += SERIALIZE_TO(ptrPayload->nonce, p);
    return p - ptrBuffer;
}


uint64_t parse_pingpong_payload(
    uint8_t *ptrBuffer,
    PingpongPayload *ptrPayload
) {
    Byte *p = ptrBuffer;
    p += PARSE_INTO(p, &ptrPayload->nonce);
    return ptrBuffer - p;
}

int32_t parse_into_pingpong_message(
    Byte *ptrBuffer,
    Message *ptrMessage
) {
    Header header = {0};
    PingpongPayload payload = {0};
    parse_message_header(ptrBuffer, &header);
    parse_pingpong_payload(ptrBuffer + sizeof(header), &payload);
    memcpy(ptrMessage, &header, sizeof(header));
    ptrMessage->ptrPayload = malloc(sizeof(PingpongPayload));
    memcpy(ptrMessage->ptrPayload, &payload, sizeof(payload));
    return 0;
}

uint64_t serialize_pingpong_message(
    Message *ptrMessage,
    uint8_t *ptrBuffer
) {
    uint64_t messageHeaderSize = sizeof(ptrMessage->header);
    memcpy(ptrBuffer, ptrMessage, messageHeaderSize);
    serialize_pingpong_payload(
        (PingpongPayload *)ptrMessage->ptrPayload,
        ptrBuffer+messageHeaderSize
    );
    return messageHeaderSize + ptrMessage->header.length;
}

void print_pingpong_message(Message *ptrMessage) {
    print_message_header(ptrMessage->header);
    PingpongPayload *ptrPayload = (PingpongPayload *)ptrMessage->ptrPayload;
    printf("payload: count=%llu\n", ptrPayload->nonce);
}
