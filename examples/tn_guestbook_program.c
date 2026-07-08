#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>
#include "tn_guestbook_program.h"

static uint read_u32_le(uchar const *data) {
    return ((uint)data[0])
        | (((uint)data[1]) << 8)
        | (((uint)data[2]) << 16)
        | (((uint)data[3]) << 24);
}

TSDK_ENTRYPOINT_FN void start(void) {
    tsdk_txn_t const *txn = tsdk_get_txn();
    uchar const *instruction_data = tsdk_txn_get_instr_data(txn);
    ulong instruction_data_sz = tsdk_txn_get_instr_data_sz(txn);

    if (instruction_data_sz < 8UL) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint instruction_type = read_u32_le(instruction_data);

    if (instruction_type != TN_GUESTBOOK_INSTRUCTION_POST) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE);
    }

    uint message_len = read_u32_le(instruction_data + 4);

    if (message_len > TN_GUESTBOOK_MAX_MESSAGE_LEN) {
        tsdk_revert(TN_GUESTBOOK_ERR_MESSAGE_TOO_LONG);
    }

    if (instruction_data_sz != 8UL + (ulong)message_len) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uchar const *message = instruction_data + 8;

    uchar event_data[sizeof(ulong) + TN_GUESTBOOK_MAX_MESSAGE_LEN];

    ulong event_type = 1UL;
    uchar const *event_type_bytes = (uchar const *)&event_type;

    for (uint i = 0U; i < sizeof(ulong); i++) {
        event_data[i] = event_type_bytes[i];
    }

    for (uint i = 0U; i < message_len; i++) {
        event_data[sizeof(ulong) + i] = message[i];
    }

    tsys_emit_event(event_data, sizeof(ulong) + message_len);

    tsdk_return(TSDK_SUCCESS);
}
