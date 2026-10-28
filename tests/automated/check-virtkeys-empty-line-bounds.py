#!/usr/bin/env python3
"""Check virtual-key parsing never indexes before an empty input line."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("Boolean \n_XmVirtKeysLoadFileBindings")
end = source.index("static void \nLoadVendorBindings", start)
body = source[start:end]

assert "size_t line_length = strlen(line);" in body
assert "line_length != 0 && line[line_length - 1] == '\\n'" in body
assert "line[strlen(line) - 1]" not in body

print("Virtual-key parsing guards empty lines before checking their terminator")
