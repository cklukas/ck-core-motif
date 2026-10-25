#!/usr/bin/env python3
"""Check that active XmString rendition growth is transactional."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static void\nbegin_context_rends(")
end = source.index("\n/*\n * end_context_rends", start)
region = source[start:end]

assert "if (count <= 0)" in region
assert "SIZE_MAX - add_count" in region
assert "new_tags = (XmStringTag *)XtRealloc" in region
assert "if (!new_tags)" in region
assert "_XmStrContError(context) = True;" in region
assert region.index("if (!new_tags)") < region.index("memcpy")
print("PASS XmString context rendition growth is transactional")
