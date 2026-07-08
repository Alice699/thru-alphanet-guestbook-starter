# Thru Alphanet Deployment Notes

This file records the deployed Thru Alphanet programs, test transactions, and ABI publishing status for this starter project.

The goal of this document is to make the deployment reproducible and easy to verify from the CLI or Thru explorer.

---

## Network

```text
Network: Thru Alphanet
Status: Experimental / pre-production
```

This project was tested against Thru Alphanet using the Thru CLI.

Verify CLI connectivity:

```bash
thru getversion
thru --json getversion
```

---

## Local Environment

Tested with:

```text
Node.js: v22.22.2
npm:     10.9.7
```

Installed tools:

```bash
npm i -g thru
thru dev toolchain install
thru dev sdk install c
```

Build command:

```bash
make clean
make
```

Expected build outputs:

```text
build/thruvm/bin/tn_counter_program_c.bin
build/thruvm/bin/tn_guestbook_program_c.bin
```

---

## Local Account

A local CLI key and alphanet account were created with:

```bash
thru keys generate default
thru account create default
```

Do not commit or share private keys.

The local CLI key config is stored under:

```text
~/.thru/cli/config.yaml
```

---

## Counter Program

The counter program was deployed first to verify the full Thru developer workflow before building the Guestbook program.

### Program Account

```text
taUrjDxskuPtXAsXWgVWqHy-yc37G7JmV8m9uDiPMvEn5B
```

### Counter State Account

```text
tasGhF_XtO_s-H_9imDSXm5ON8iFeSIQpJ94hOxaDIwRov
```

### Deploy Command

```bash
thru program create counter_program ./build/thruvm/bin/tn_counter_program_c.bin
```

### Derive Counter Account

```bash
export PROGRAM="taUrjDxskuPtXAsXWgVWqHy-yc37G7JmV8m9uDiPMvEn5B"

thru program derive-address $PROGRAM count_acc
```

Result:

```text
tasGhF_XtO_s-H_9imDSXm5ON8iFeSIQpJ94hOxaDIwRov
```

### Create Counter State Account

The counter account was created using:

```bash
export COUNTER="tasGhF_XtO_s-H_9imDSXm5ON8iFeSIQpJ94hOxaDIwRov"

SEED_HEX=$(thru program seed-to-hex count_acc | sed -n 's/^Padded Hex: //p')
PROOF_HEX=$(thru txn make-state-proof creating "$COUNTER" | grep -i "Proof Data (hex)" | sed -E 's/^.*Proof Data \([Hh]ex\):[[:space:]]*//' | tr -d '[:space:]')

CREATE_HEX="000000000200${SEED_HEX}68000000${PROOF_HEX}"

thru txn execute \
  --fee 0 \
  --readwrite-accounts "$COUNTER" \
  "$PROGRAM" \
  "$CREATE_HEX"
```

### Increment Counter

Increment command:

```bash
./scripts/increment-counter.sh
```

Raw instruction data:

```text
010000000200
```

Meaning:

```text
01000000 = instruction_type 1
0200     = account index 2
```

Observed event output:

```text
Data (hex): 0200000000000000
```

This represents the counter value `2` in little-endian format.

### Counter Status

```text
Compiled: yes
Deployed: yes
State account created: yes
Increment transaction executed: yes
Event emitted: yes
```

---

## Guestbook V1

Guestbook V1 is an event-only guestbook program.

It accepts a UTF-8 message as instruction data and emits a readable event.

### Program Account

```text
taBD7C5jeKOFKP85IXLXai5hyK6bhzrYJ9m8eohJK3akt9
```

### Deploy Command

```bash
thru program create guestbook_v1 ./build/thruvm/bin/tn_guestbook_program_c.bin
```

After deployment:

```bash
export GUESTBOOK_PROGRAM="taBD7C5jeKOFKP85IXLXai5hyK6bhzrYJ9m8eohJK3akt9"
```

---

## Guestbook Instruction Format

Guestbook V1 accepts:

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

Message length:

```text
22 bytes
```

Encoded payload:

```text
0100000016000000676d20746872752066726f6d20696e646f6e65736961
```

Breakdown:

```text
01000000                                      instruction_type = 1
16000000                                      message_len = 22
676d20746872752066726f6d20696e646f6e65736961 message bytes
```

---

## Post Message Transaction

Helper script:

```bash
./scripts/post-message.sh "gm thru from indonesia"
```

Expected successful output:

```text
Success: Transaction executed successfully
VM Error: 0
User Error Code: 0
Events Count: 1
Event type: 1
Data (string): "gm thru from indonesia"
```

### Guestbook Event Format

Guestbook V1 emits:

```text
event_type: u64 little-endian
message:    UTF-8 bytes
```

For this deployment:

```text
event_type = 1
message    = user-submitted string
```

Observed output:

```text
Event type: 1
Data (string): "gm thru from indonesia"
```

### Guestbook Status

```text
Compiled: yes
Deployed: yes
Post message transaction executed: yes
Readable event emitted: yes
ABI published: yes
ABI visible in explorer: yes
```

---

## ABI

The ABI files for Guestbook V1 are stored in:

```text
abi/guestbook_v1.abi.yaml
abi/guestbook_v1.publish.abi.yaml
```

### Analyze ABI

```bash
thru abi analyze --files abi/guestbook_v1.abi.yaml
```

Expected result:

```text
Analysis complete!
```

### Prepare ABI for Publish

```bash
thru abi prep-for-publish \
  --file abi/guestbook_v1.abi.yaml \
  --target-network testnet \
  --output abi/guestbook_v1.publish.abi.yaml
```

### Publish ABI

```bash
thru abi account create guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

If the ABI account already exists:

```bash
thru abi account upgrade guestbook_v1 abi/guestbook_v1.publish.abi.yaml
```

### ABI Status

```text
Source ABI created: yes
Publish-ready ABI generated: yes
ABI account published: yes
ABI visible in explorer: yes
```

---

## Explorer Verification

The Guestbook program can be checked in the Thru explorer using the program account:

```text
taBD7C5jeKOFKP85IXLXai5hyK6bhzrYJ9m8eohJK3akt9
```

Verified explorer state:

```text
Program account visible: yes
Transactions visible: yes
Data tab visible: yes
ABI tab populated: yes
```

---

## GitHub

Repository:

```text
https://github.com/Alice699/thru-alphanet-guestbook-starter
```

Repo status:

```text
README updated: yes
DEPLOYMENT.md updated: yes
ABI files committed: yes
Pushed to GitHub: yes
```

---

## Final Status

This deployment verifies a real Thru Alphanet builder flow:

```text
C source code
→ ThruVM binary
→ program deployment
→ transaction execution
→ readable event output
→ ABI publishing
→ explorer verification
→ GitHub documentation
```

Final result:

```text
Counter program: deployed and tested
Guestbook V1: deployed and tested
Message event: emitted successfully
ABI: published and visible in explorer
Repo: pushed to GitHub
```

This is a real alphanet experiment.

It is not a fake demo, not a mock app, and not a production dapp.
