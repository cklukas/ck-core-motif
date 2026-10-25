#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCMenu.c").read_text()
start = source.index("callbackClosure =")
end = source.index("for (i=0; i < count; i++)", start)
region = source[start:end]
assert "XtMalloc(sizeof(XtPointer) * count)" in region
assert "if (callbackClosure == NULL)" in region
assert region.index("if (callbackClosure == NULL)") < region.index("return;")
print("RCMenu callback-closure allocation guard present")
