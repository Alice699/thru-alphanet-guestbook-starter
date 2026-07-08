# Thru Alphanet Guestbook Starter

A minimal, verifiable starter project for building, deploying, and testing C programs on **Thru Alphanet**.

This repo documents a working end-to-end Thru builder flow:

1. Install Thru CLI and C SDK
2. Compile C programs to ThruVM binaries
3. Deploy programs to Thru Alphanet
4. Execute transactions against deployed programs
5. Emit readable on-chain events
6. Publish ABI metadata so the explorer can decode the program

This is an **experimental alphanet project**. It is not a production dapp.

---

## Repository Structure

```text
thru-alphanet-guestbook-starter
├── abi/
│   ├── guestbook_v1.abi.yaml
│   └── guestbook_v1.publish.abi.yaml
├── examples/
│   ├── Local.mk
│   ├── tn_counter_program.c
│   ├── tn_counter_program.h
│   ├── tn_guestbook_program.c
│   └── tn_guestbook_program.h
├── scripts/
│   ├── increment-counter.sh
│   └── post-message.sh
├── DEPLOYMENT.md
├── GNUmakefile
├── README.md
└── .gitignore
```

---

## What This Project Contains

### Counter Program

The counter program is used as a baseline ThruVM test.

It verifies that the local environment can:

- build a C program
- deploy a program to Thru Alphanet
- create a program-derived state account
- execute a transaction
- mutate state
- emit an event

The counter flow is useful before building anything more complex because it confirms that the Thru CLI, C SDK, toolchain, account setup, program deployment, and transaction execution are all working.

### Guestbook V1

Guestbook V1 is a minimal event-based program.

It accepts a UTF-8 message as instruction data and emits that message as a readable event.

The goal is not to build a full production guestbook yet. The goal is to prove a clean alphanet path:

```text
C program → ThruVM binary → deployed program → transaction → event → explorer + ABI
```

---

## Requirements

You need:

- Linux, macOS, or WSL
- Node.js 18+
- npm
- Thru CLI
- Thru C SDK
- Thru toolchain

Check Node and npm:

```bash
node -v
npm -v
```

Install Thru CLI:

```bash
npm i -g thru
```

Verify the CLI:

```bash
thru --help
thru getversion
```

Install the Thru toolchain and C SDK:

```bash
thru dev toolchain install
thru dev sdk install c
```

---

## Account Setup

Generate a local key:

```bash
thru keys generate default
```

Create an account on Thru Alphanet:

```bash
thru account create default
```

Do not commit or share private keys.

The Thru CLI stores local key configuration under:

```text
~/.thru/cli/config.yaml
```

Keep that file private.

---

## Build

Build all programs:

```bash
make clean
make
```

Expected output binaries:

```text
build/thruvm/bin/tn_counter_program_c.bin
build/thruvm/bin/tn_guestbook_program_c.bin
```

The build output is ignored by git.

---

## Counter Program Flow

### Deploy Counter Program

```bash
thru program create counter_program ./build/thruvm/bin/tn_counter_program_c.bin
```

After deployment, save the program account:

```bash
export PROGRAM="your_counter_program_account"
```

### Derive Counter Account

```bash
thru program derive-address $PROGRAM count_acc
```

Save the derived address:

```bash
export COUNTER="your_counter_account"
```

### Create State Proof

```bash
thru txn make-state-proof creating $COUNTER
```

### Create Counter Account

The create instruction uses:

```text
instruction_type = 0
account_index = 2
seed = count_acc
proof_size = 104 bytes
proof_data = state proof
```

The helper command used during development was:

```bash
SEED_HEX=$(thru program seed-to-hex count_acc | sed -n 's/^Padded Hex: //p')
PROOF_HEX=$(thru txn make-state-proof creating "$COUNTER" | grep -i "Proof Data (hex)" | sed -E 's/^.*Proof Data \\([Hh]ex\\):[[:space:]]*//' | tr -d '[:space:]')

CREATE_HEX="000000000200${SEED_HEX}68000000${PROOF_HEX}"

thru txn execute \
  --fee 0 \
  --readwrite-accounts "$COUNTER" \
  "$PROGRAM" \
  "$CREATE_HEX"
```

### Increment Counter

```bash
./scripts/increment-counter.sh
```

The increment instruction is:

```text
010000000200
```

Meaning:

```text
01000000 = instruction_type 1
0200     = account index 2
```

A successful increment emits an event containing the updated counter value.

Example:

```text
Data (hex): 0200000000000000
```

This represents counter value `2` in little-endian format.

---

## Guestbook V1

Guestbook V1 is an event-only guestbook program.

It does not store a full message history in account state. Instead, it emits the submitted message as an event.

This keeps the first version simple and easy to verify on alphanet.

---

## Guestbook Instruction Format

Guestbook V1 accepts this instruction layout:

```text
instruction_type: u32 little-endian
message_len:      u32 little-endian
message:          UTF-8 bytes
```

For posting a message:

```text
instruction_type = 1
```

Example message:

```text
gm thru from indonesia
```

The script encodes it as:

```text
01000000                                      instruction_type = 1
16000000                                      message_len = 22 bytes
676d20746872752066726f6d20696e646f6e65736961 message bytes
```

Full payload:

```text
0100000016000000676d20746872752066726f6d20696e646f6e65736961
```

---

## Deploy Guestbook V1

Deploy the Guestbook V1 program:

```bash
thru program create guestbook_v1 ./build/thruvm/bin/tn_guestbook_program_c.bin
```

Save the returned program account:

```bash
export GUESTBOOK_PROGRAM="your_guestbook_program_account"
```

Example deployment used during testing:

```text
taBD7C5jeKOFKP85IXLXai5hyK6bhzrYJ9m8eohJK3akt9
```

Alphanet state may reset or change, so do not rely on this address as permanent infrastructure.

---

## Post a Message

Use the helper script:

```bash
./scripts/post-message.sh "gm thru from indonesia"
```

The script:

1. reads the message from the command line
2. encodes it as UTF-8
3. prefixes it with `instruction_type` and `message_len`
4. submits the transaction through `thru txn execute`

Successful output should include:

```text
Success: Transaction executed successfully
VM Error: 0
User Error Code: 0
Events Count: 1
Event type: 1
Data (string): "gm thru from indonesia"
```

---

## Guestbook Event Format

Guestbook V1 emits:

```text
event_type: u64 little-endian
message:    UTF-8 bytes
```

For the current demo:

```text
event_type = 1
message    = user-submitted string
```

Example explorer or CLI output:

```text
Event type: 1
Data (string): "gm thru from indonesia"
```

---

## ABI

This repo includes ABI files for Guestbook V1:

```text
abi/guestbook_v1.abi.yaml
abi/guestbook_v1.publish.abi.yaml
```

The source ABI file is:

```text
abi/guestbook_v1.abi.yaml
```

The publish-ready ABI file is:

```text
abi/guestbook_v1.publish.abi.yaml
```

Analyze ABI:

```bash
thru abi analyze --files abi/guestbook_v1.abi.yaml
```

Prepare ABI for publishing:

```bash
thru abi prep-for-publish \
  --file abi/guestbook_v1.abi.yaml \
  --target-network testnet \
  --output abi/guestbook_v1.publish.abi.yaml
```

Publish ABI account:

```bash
thru abi account create guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

If the ABI account already exists:

```bash
thru abi account upgrade guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

After publishing, the ABI should appear in the Thru explorer for the deployed Guestbook program account.

---

## Verified Alphanet Milestones

This project has verified the following on Thru Alphanet:

- Thru CLI installed successfully
- C SDK installed successfully
- Toolchain installed successfully
- Alphanet account created
- Counter program compiled
- Counter program deployed
- Counter transaction executed successfully
- Guestbook V1 compiled
- Guestbook V1 deployed
- Guestbook message transaction executed successfully
- Readable event emitted
- ABI published and visible in explorer

---

## Current Limitations

This is intentionally small.

Guestbook V1 currently:

- emits messages as events
- does not store full message history
- does not include a frontend
- does not include wallet/passkey integration
- is not production-ready
- is intended for alphanet experimentation only

The next version could add:

- persistent guestbook state
- message counter
- author/account tracking
- dynamic event ABI improvements
- frontend UI
- wallet/passkey flow
- transaction status display
- explorer links

---

## Useful Commands

Build:

```bash
make clean
make
```

Deploy Guestbook:

```bash
thru program create guestbook_v1 ./build/thruvm/bin/tn_guestbook_program_c.bin
```

Set Guestbook program account:

```bash
export GUESTBOOK_PROGRAM="your_guestbook_program_account"
```

Post message:

```bash
./scripts/post-message.sh "gm thru from indonesia"
```

Analyze ABI:

```bash
thru abi analyze --files abi/guestbook_v1.abi.yaml
```

Publish ABI:

```bash
thru abi account create guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

Upgrade ABI:

```bash
thru abi account upgrade guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

Check git status:

```bash
git status
```

---

## Safety Notes

Do not commit:

- private keys
- seed phrases
- local CLI config
- `.env` files
- `~/.thru/cli/config.yaml`

This repo intentionally ignores:

```text
build/
*.o
*.elf
*.s
node_modules/
.env
```

---

## Status

This repo is a real Thru Alphanet experiment.

It is not a fake demo, not a mock app, and not a production dapp.

It is a small starter project showing the builder path from local C code to a deployed Thru Alphanet program with readable event output and ABI support.
