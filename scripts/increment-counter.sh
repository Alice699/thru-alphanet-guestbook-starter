#!/usr/bin/env bash
set -euo pipefail

if [ -z "${PROGRAM:-}" ]; then
  echo "Missing PROGRAM env var"
  exit 1
fi

if [ -z "${COUNTER:-}" ]; then
  echo "Missing COUNTER env var"
  exit 1
fi

thru txn execute \
  --fee 0 \
  --readwrite-accounts "$COUNTER" \
  "$PROGRAM" \
  010000000200
