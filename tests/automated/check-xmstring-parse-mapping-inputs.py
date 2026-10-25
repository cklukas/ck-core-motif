#!/usr/bin/env python3
"""Check XmString parse-mapping setters validate argument inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmParseMappingSetValues(")
region = source[start:]
assert "mapping == NULL || (arg_count != 0 && arg_list == NULL)" in region
assert "if (arg_name == NULL)" in region
assert "unknown++;" in region
assert "continue;" in region
print("PASS XmString parse-mapping inputs are validated")
