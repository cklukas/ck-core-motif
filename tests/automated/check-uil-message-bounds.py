#!/usr/bin/env python3
"""Check that UIL diagnostic listing messages fit their fixed line buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()
start = source.index("void\tlst_output_messages")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "snprintf(buffer, sizeof(buffer), \"%s (%d) %s\"" in region
assert "sprintf(buffer, \"%s (%d) %s\"" not in region
print("UIL diagnostic message listing is bounded")
