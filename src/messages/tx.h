#pragma once

#include <stdint.h>
#include "datatypes.h"
#include "hash.h"
#include "shared.h"

// @see https://en.bitcoin.it/wiki/Protocol_documentation#tx
// For witness related: @see https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki

#define WITNESS_MARKER 0x00
#define WITNESS_FLAG 0x01

#define MAX_SIGNATURE_SCRIPT_LENGTH 10000
#define MAX_PK_SCRIPT_LENGTH 10000
#define MAX_WITNESS_DATA_LENGTH 10000

struct Outpoint {
    SHA256_HASH txHash;
    uint32_t index;
};

typedef struct Outpoint Outpoint;

struct TxIn {
    Outpoint previous_output;
    uint32_t sequence;
    uint64_t signature_script_length;
    Byte signature_script[MAX_SIGNATURE_SCRIPT_LENGTH];
};

typedef struct TxIn TxIn;

struct TxOut {
    int64_t value;
    uint64_t public_key_script_length;
    Byte public_key_script[MAX_PK_SCRIPT_LENGTH];
};

typedef struct TxOut TxOut;

// @see https://bitcoin.stackexchange.com/questions/68924/

struct TxWitness {
    VarIntMem length;
    Byte data[MAX_WITNESS_DATA_LENGTH];
};

typedef struct TxWitness TxWitness;

struct TxPayload {
    int32_t version;
    Byte marker;
    Byte flag;
    VarIntMem txInputCount;
    TxIn *txInputs;
    VarIntMem txOutputCount;
    TxOut *txOutputs;
    TxWitness *txWitnesses;
    uint32_t lockTime;
};

typedef struct TxPayload TxPayload;

uint64_t serialize_tx_payload(TxPayload *ptrPayload, Byte *ptrBuffer);
uint64_t parse_into_tx_payload(Byte *ptrBuffer, TxPayload *ptrTx);
uint64_t serialize_tx_message(Message *ptrPayload, Byte *ptrBuffer);
int32_t make_tx_message(Message *ptrMessage, TxPayload *ptrPayload);
int32_t compute_merkle_root(TxPayload txs[], uint64_t txCount, SHA256_HASH result);
void print_tx_payload(TxPayload *ptrTx);
bool is_coinbase(TxIn *input);
bool is_tx_legal(TxPayload *ptrTx);
void release_items_in_tx(TxPayload *tx);
void clone_tx(TxPayload *txFrom, TxPayload *txTo);
uint64_t parse_tx_out(Byte *ptrBuffer, TxOut *ptrTxOut);
uint64_t serialize_tx_out(TxOut *ptrTxOut, Byte *ptrBuffer);
void hash_tx(TxPayload *ptrTx, SHA256_HASH result);
bool is_outpoint_empty(Outpoint *ptrOutpoint);
