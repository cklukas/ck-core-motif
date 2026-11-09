#!/usr/bin/env python3
"""Check that UIL diagnostic listing messages retain complete text."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()
start = source.index("void\tlst_output_messages")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "formatted_length = snprintf(NULL, 0, \"%s (%d) %s\"" in region
assert "XtMalloc((Cardinal)((size_t)formatted_length + 1))" in region
assert "lst_output_line(buffer, FALSE);" in region
assert "sprintf(buffer, \"%s (%d) %s\"" not in region
assert "ptr_buffer[ pos++ ] = (char)(msg_no + '0');" in source
print("UIL diagnostic message listing preserves complete text")
