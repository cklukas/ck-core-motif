#!/usr/bin/env python3
"""Check XmString enumeration helpers avoid recursive process locking."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()

void_start = source.index("XmStringIsVoid(")
void_end = source.index("\n}\n", void_start)
void_function = source[void_start:void_end]
assert "_XmProcessLock();" not in void_function
assert "_XmProcessUnlock();" not in void_function
assert "XmeStringGetComponent" in void_function

stream_start = source.index("XmCvtXmStringToByteStream(XmString string")
stream_end = source.index("Dimension \nXmStringBaseline", stream_start)
stream_function = source[stream_start:stream_end]
assert "_XmProcessLock();" not in stream_function
assert "_XmProcessUnlock();" not in stream_function
assert stream_function.count("XmeStringGetComponent") >= 2

print("PASS XmString enumeration helpers avoid recursive process locking")
