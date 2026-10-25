#!/usr/bin/env python3
"""Check that XmStringUnparse validates parse-table inputs."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

helper_start = source.index("unparse_components(char          **result,", source.index("unparse_components(char          **result,") + 1)
helper_end = source.index("\n}\n", helper_start)
helper = source[helper_start:helper_end]
assert "if (pat == NULL)" in helper
assert "continue;" in helper

unparse_start = source.index("XmStringUnparse(XmString          string,")
unparse_end = source.index("\n}\n", unparse_start)
unparse = source[unparse_start:unparse_end]
assert "if (parse_count != 0 && parse_table == NULL)" in unparse
assert "return NULL;" in unparse

print("XmStringUnparse validates parse-table inputs")
