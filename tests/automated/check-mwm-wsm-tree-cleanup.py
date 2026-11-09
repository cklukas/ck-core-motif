from pathlib import Path

source = Path("clients/mwm/WmWsm.c").read_text(encoding="utf-8")
start = source.index("GetPCD (")
body = source[start:source.index("return (pCD);", start)]

assert body.count("XFree ((char *)children)") >= 2
assert "if (children != NULL)" in body

events = Path("clients/mwm/WmCEvent.c").read_text(encoding="utf-8")
start = events.index("Window GetParentWindow (Window window)")
body = events[start:events.index("} /* END OF FUNCTION GetParentWindow */", start)]
assert "if (children != NULL)" in body
assert "if (nchildren)" not in body

print("PASS MWM parent lookups free XQueryTree child lists by pointer")
