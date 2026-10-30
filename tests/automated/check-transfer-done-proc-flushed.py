#!/usr/bin/env python3
"""Ensure flushed transfers do not accept unreachable completion callbacks."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text()
start = source.index("XmeTransferAddDoneProc(")
end = source.index("\nXmePrimarySource(", start)
region = source[start:end]

assert "if (tid -> flags & TC_FLUSHED)" in region
assert "_XmProcessUnlock();" in region
assert region.index("tid -> flags & TC_FLUSHED") < region.index("tid -> numDoneProcs == INT_MAX")

print("PASS flushed transfers reject new done callbacks")
