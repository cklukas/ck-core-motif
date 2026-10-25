#!/usr/bin/env python3
"""Check that unique-event state allocation failure is handled safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "UniqueEvnt.c").read_text(encoding="utf-8")
start = source.index("static XmUniqueStamp\nGetUniqueStamp")
end = source.index("/*ARGSUSED*/\nstatic void\nUniqueStampDisplayDestroyCallback", start)
allocator = source[start:end]
assert "XtMalloc(sizeof(XmUniqueStampRec))" in allocator
assert "uniqueStamp)" in allocator and "return (XmUniqueStamp)NULL" in allocator
manipulate = source[source.index("static Boolean \nManipulateEvent"):]
assert "if ((XmUniqueStamp)NULL == uniqueStamp)" in manipulate
assert "return (False);" in manipulate
print("PASS unique-event state allocation is guarded")
