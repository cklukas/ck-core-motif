#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCMenu.c").read_text()
start = source.index("count = 0;", source.index("callbackClosure =") - 500)
end = source.index("for (i=0; i < count; i++)", start)
region = source[start:end]
assert "(size_t)count > UINT_MAX / sizeof(XtPointer)" in region
assert "XtMalloc((Cardinal)(sizeof(XtPointer) *" in region
assert "if (callbackClosure == NULL)" in region
assert region.index("if (callbackClosure == NULL)") < region.index(
    "return;", region.index("if (callbackClosure == NULL)"))
print("RCMenu callback-closure allocation guard present")
