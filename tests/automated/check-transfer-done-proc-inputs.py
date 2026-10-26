#!/usr/bin/env python3
"""Check transfer done-proc dispatch validates its context and list."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.rindex("CallDoneProcs(Widget wid")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
for token in ("tid == NULL", "ts == NULL", "tid->numDoneProcs <= 0",
              "tid->doneProcs == NULL"):
    assert token in body, token

print("Transfer done-proc dispatch validates its context and list")
