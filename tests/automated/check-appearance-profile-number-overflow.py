#!/usr/bin/env python3
"""Check appearance-profile numeric conversion rejects strtoul overflow."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Appearance.c").read_text()
start = source.index("profile_property_value(")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "number = strtoul(text, &end, 10);" in body
assert "if (errno == ERANGE || *text == '\\0'" in body
assert body.index("errno == ERANGE") < body.index("*value_return = (XtArgVal)(Dimension)number;")
print("PASS appearance-profile numbers reject conversion overflow")
