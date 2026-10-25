#!/usr/bin/env python3
"""Check active-rendition bookkeeping input guards."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

begin_start = source.index("begin_context_rends(")
begin_start = source.index("static void\nbegin_context_rends(")
begin_end = source.index("\n}\n", begin_start)
begin = source[begin_start:begin_end]
assert "if (rends == NULL)" in begin

end_start = source.index("static void\nend_context_rends(")
end_end = source.index("\n}\n", end_start)
end = source[end_start:end_end]
assert "rends == NULL" in end
assert "_XmStrContRendTags(context) == NULL" in end

print("XmString active-rendition bookkeeping guards inputs")
