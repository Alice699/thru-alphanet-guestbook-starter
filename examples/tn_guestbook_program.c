#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>

#include "tn_guestbook_program.h"

static ushort read_u16_le(uchar const *data) {
    return (ushort)data[0]
        | ((ushort)data[1] << 8);
}

static uint read_u32_le(uchar const *data) {
    return ((uint)data[0])
        | (((uint)data[1]) << 8)
        | (((uint)data[2]) << 16)
        | (((uint)data[3]) << 24);
}

static void handle_create(
    uchar const *instruction_data,
    ulong instruction_size
) {
    if (instruction_size < 42UL) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    ushort account_index = read_u16_le(instruction_data + 4);

    uchar const *seed = instruction_data + 6;

    uint proof_size = read_u32_le(instruction_data + 38);

    ulong expected_size = 42UL + (ulong)proof_size;

    if (instruction_size != expected_size) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uchar const *proof_data = NULL;

    if (proof_size > 0U) {
        proof_data = instruction_data + 42;
    }

    ulong result = tsys_account_create(
        account_index,
        seed,
        proof_data,
        proof_size
    );

    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_CREATE_FAILED);
    }

    result = tsys_set_account_data_writable(account_index);

    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    result = tsys_account_resize(
        account_index,
        sizeof(tn_guestbook_account_t)
    );

    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_RESIZE_FAILED);
    }

    tn_guestbook_account_t *account =
        (tn_guestbook_account_t *)tsdk_get_account_data_ptr(account_index);

    if (account == NULL) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_ACCESS_FAILED);
    }

    account->message_count = 0UL;
    account->message_len = 0U;

    for (uint i = 0U; i < TN_GUESTBOOK_MAX_MESSAGE_LEN; i++) {
        account->message[i] = 0U;
    }

    tsdk_return(TSDK_SUCCESS);
}

static void handle_post(
    uchar const *instruction_data,
    ulong instruction_size
) {
    if (instruction_size < 10UL) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    ushort account_index = read_u16_le(instruction_data + 4);

    uint message_len = read_u32_le(instruction_data + 6);

    if (message_len > TN_GUESTBOOK_MAX_MESSAGE_LEN) {
        tsdk_revert(TN_GUESTBOOK_ERR_MESSAGE_TOO_LONG);
    }

    if (instruction_size != 10UL + (ulong)message_len) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    tn_guestbook_account_t *account =
        (tn_guestbook_account_t *)tsdk_get_account_data_ptr(account_index);

    if (account == NULL) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_ACCESS_FAILED);
    }

    ulong result = tsys_set_account_data_writable(account_index);

    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    account->message_count++;
    account->message_len = message_len;

    for (uint i = 0U; i < TN_GUESTBOOK_MAX_MESSAGE_LEN; i++) {
        account->message[i] = 0U;
    }

    for (uint i = 0U; i < message_len; i++) {
        account->message[i] = instruction_data[10 + i];
    }

    uchar event_data[sizeof(ulong) + TN_GUESTBOOK_MAX_MESSAGE_LEN];

    ulong event_type = 2UL;
    uchar const *event_type_bytes = (uchar const *)&event_type;

    for (uint i = 0U; i < sizeof(ulong); i++) {
        event_data[i] = event_type_bytes[i];
    }

    for (uint i = 0U; i < message_len; i++) {
        event_data[sizeof(ulong) + i] = account->message[i];
    }

    tsys_emit_event(
        event_data,
        sizeof(ulong) + message_len
    );

    tsdk_return(TSDK_SUCCESS);
}

TSDK_ENTRYPOINT_FN void start(void) {
    tsdk_txn_t const *txn = tsdk_get_txn();

    uchar const *instruction_data = tsdk_txn_get_instr_data(txn);
    ulong instruction_size = tsdk_txn_get_instr_data_sz(txn);

if (instruction_size < 4UL) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint instruction_type = read_u32_le(instruction_data);

    switch (instruction_type) {
        case TN_GUESTBOOK_INSTRUCTION_CREATE:
            handle_create(instruction_data, instruction_size);
            break;

        case TN_GUESTBOOK_INSTRUCTION_POST:
            handle_post(instruction_data, instruction_size);
            break;

        default:
            tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE);
    }

    tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE);
}
