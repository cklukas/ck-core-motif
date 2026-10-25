#!/usr/bin/env python3
"""Check XmParseMappingSetValues validates argument-list inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmParseMappingSetValues(")
end = source.index("/* Destructively truncates str", start)
region = source[start:end]
assert "(arg_count != 0 && arg_list == NULL)" in region
assert "if (arg_name == NULL)" in region
assert region.index("if (arg_name == NULL)") < region.index("strcmp(arg_name")
print("PASS XmString parse-mapping inputs are validated")
