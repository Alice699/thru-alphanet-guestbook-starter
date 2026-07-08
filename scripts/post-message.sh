#!/usr/bin/env bash
set -euo pipefail

if [ -z "${GUESTBOOK_PROGRAM:-}" ]; then
  echo "Missing GUESTBOOK_PROGRAM env var"
  echo "Example:"
  echo "export GUESTBOOK_PROGRAM=\"your_guestbook_program_account\""
  exit 1
fi

if [ "$#" -lt 1 ]; then
  echo "Usage:"
  echo "./scripts/post-message.sh \"hello thru\""
  exit 1
fi

MESSAGE="$*"

POST_HEX=$(python3 - "$MESSAGE" <<'PY'
import sys

message = sys.argv[1].encode("utf-8")

if len(message) > 256:
    raise SystemExit("Message too long. Max 256 bytes.")

instruction_type = (1).to_bytes(4, "little")
message_len = len(message).to_bytes(4, "little")

print((instruction_type + message_len + message).hex())
PY
)

echo "Message: $MESSAGE"
echo "POST_HEX: $POST_HEX"

thru txn execute \
  --fee 0 \
  "$GUESTBOOK_PROGRAM" \
  "$POST_HEX"
