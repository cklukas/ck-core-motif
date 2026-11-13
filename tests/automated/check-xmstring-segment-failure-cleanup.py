#!/usr/bin/env python3
"""Check failed XmString segment extraction releases all owned outputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("_XmStringGetSegment(")
end = source.index("Boolean \n_XmStringGetNextSegment(", start)
body = source[start:end]
failure = body.index("if (copy_data && !result)")
cleanup = body[failure:]
# the text and the tag belong to the string until they are copied: they are not released
assert "XtFree((char *)*text);" not in cleanup
assert "XtFree((char *)*tag);" not in cleanup
assert "*text = NULL;" in cleanup
assert "*tag = NULL;" in cleanup
assert "XtFree((char *)(*rendition_tags)[cleanup]);" in cleanup
assert "*rendition_tags = NULL;" in cleanup
assert "*tag_count = 0;" in cleanup
print("PASS XmString segment failure cleanup")
