from pathlib import Path

source = Path("clients/mwm/WmWsm.c").read_text(encoding="utf-8")
start = source.index("GetPCD (")
body = source[start:source.index("return (pCD);", start)]

assert body.count("XFree ((char *)children)") >= 2
assert "if (children != NULL)" in body
print("PASS MWM parent lookup frees XQueryTree child lists")
