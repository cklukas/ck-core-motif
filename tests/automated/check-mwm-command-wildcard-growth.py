#!/usr/bin/env python3
"""Check MWM client-command matching grows wildcard state safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
helper_start = source.index("static Boolean\nAppendClientCommandWildcard")
helper_end = source.index("\n#define RETURN_CLIENT_COMMAND_MATCH", helper_start)
helper = source[helper_start:helper_end]
match_start = source.index("static MenuItem *PerformClientCommandMatch", helper_end)
match_end = source.index("\n#undef RETURN_CLIENT_COMMAND_MATCH", match_start)
match = source[match_start:match_end]

assert "#define MAXSEGMENTS" not in source
assert "XtMalloc" in helper
assert "XtRealloc" in helper
assert "UINT_MAX / sizeof(**positions)" in helper
assert "*capacity = next_capacity;" in helper
assert "AppendClientCommandWildcard(&wildcardPositions" in match
assert "AppendClientCommandWildcard(&bestWildcardPositions" in match
assert "size_t *wildcardPositions = NULL;" in match
assert "size_t *bestWildcardPositions = NULL;" in match
assert "XtFree((char *)wildcardPositions);" in source
assert "XtFree((char *)bestWildcardPositions);" in source
assert "return(" not in match
print("PASS MWM client-command wildcards grow without a fixed stack limit")
