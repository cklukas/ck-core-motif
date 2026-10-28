#!/usr/bin/env python3
"""Check deferred clipboard publication and count updates share a lock."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Transfer.c").read_text()
start = source.index("/* Associate the data_id with this snapshot")
end = source.index("FreeSafeAtomName(name,howFree);", start)
region = source[start:end]

assert "_XmProcessLock();" in region
assert "req -> outstanding++;" in region
assert region.index("_XmProcessLock();") < region.index("req -> outstanding++;")
assert region.index("req -> outstanding++;") < region.index("_XmProcessUnlock();")

print("PASS deferred clipboard count update is locked with publication")
