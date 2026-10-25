#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static Boolean\nSpecifiedSegmentExtents")
end = source.index("\nstatic void \n_parse_locale", start)
body = source[start:end]
assert "new_tag_count" in body
assert "SIZE_MAX / sizeof(XmStringTag)" in body
assert body.count("UINT_MAX / sizeof(XmStringTag)") >= 2
assert "*new_tags;" in body
assert body.count("if (new_tags == NULL)") >= 1
assert "tags = new_tags;" in body
assert body.count("(Cardinal)(sizeof(XmStringTag) * new_tag_count)") >= 2
print("XmString rendition-stack guards present")
