# Thru Alphanet Guestbook Starter

A minimal experimental starter project for building and testing C programs on Thru Alphanet.

This project demonstrates:

- Installing and using the Thru CLI
- Building ThruVM C programs
- Deploying programs to Thru Alphanet
- Executing transactions
- Emitting readable event data
- Creating a simple guestbook-style message flow

## Status

Experimental alphanet demo. Not production-ready.

## Programs

### Counter Program

The counter program was used to verify the full Thru developer workflow:

- Compile C program
- Deploy program
- Create counter account
- Execute increment transaction
- Emit counter event

### Guestbook V1

Guestbook V1 is an event-only guestbook program.

It accepts a message and emits it as a readable event.

Instruction format:

instruction_type: u32 little-endian  
message_len: u32 little-endian  
message: UTF-8 bytes

For posting a message:

instruction_type = 1

Expected event output:

Event type: 1  
Data string: "gm thru from indonesia"

## Build

`bash
make clean
make
# thru-alphanet-guestbook-starter
