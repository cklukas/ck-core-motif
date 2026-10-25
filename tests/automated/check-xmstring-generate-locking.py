#!/usr/bin/env python3
"""Check XmStringGenerate delegates locking to parse/string helpers."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringGenerate(XtPointer")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "XmStringParseText" in function
assert "XmStringComponentCreate" in function
assert "XmStringConcatAndFree" in function

print("XmStringGenerate avoids recursive process locking")
