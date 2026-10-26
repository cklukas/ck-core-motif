from pathlib import Path

source = Path("lib/Xm/ColorObj.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nColorCachePropertyExists")
end = source.index("/**********************************************************************/", start)
region = source[start:end]

assert "format != 8" in region
assert "value[length - 1]" in region
print("PASS ColorObj property parsing requires 8-bit X11 data")
