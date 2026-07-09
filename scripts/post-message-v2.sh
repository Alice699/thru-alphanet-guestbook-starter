#!/usr/bin/env bash
set -euo pipefail

if [ -z "${GUESTBOOK_V2_PROGRAM:-}" ]; then
  echo "Missing GUESTBOOK_V2_PROGRAM env var"
  exit 1
fi

if [ -z "${GUESTBOOK_V2_STATE:-}" ]; then
  echo "Missing GUESTBOOK_V2_STATE env var"
  exit 1
fi

if [ "$#" -lt 1 ]; then
  echo "Usage:"
  echo "./scripts/post-message-v2.sh \"gm thru v2\""
  exit 1
fi

MESSAGE="$*"

POST_HEX=$(python3 - "$MESSAGE" <<'PY'
import sys

message = sys.argv[1].encode("utf-8")

if len(message) == 0:
    raise SystemExit("Message cannot be empty.")

if len(message) > 256:
    raise SystemExit("Message too long. Max 256 bytes.")

instruction_type = (1).to_bytes(4, "little")
account_index = (2).to_bytes(2, "little")
message_len = len(message).to_bytes(4, "little")

print((instruction_type + account_index + message_len + message).hex())
PY
)

echo "Message: $MESSAGE"
echo "Program: $GUESTBOOK_V2_PROGRAM"
echo "State:   $GUESTBOOK_V2_STATE"
echo "POST_HEX: $POST_HEX"

thru txn execute \
  --fee 0 \
  --readwrite-accounts "$GUESTBOOK_V2_STATE" \
  "$GUESTBOOK_V2_PROGRAM" \
  "$POST_HEX"
