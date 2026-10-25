#!/usr/bin/env python3
"""Check byte-stream conversion delegates locking to cache helpers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmCvtByteStreamToXmString(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "_XmStringIndexCacheTag" in function
assert "_XmStringNonOptCreate" in function

print("PASS XmString byte-stream conversion avoids recursive process locking")
