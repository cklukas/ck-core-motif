#!/usr/bin/env python3
"""Check that MWM menu exclusions are added transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")

parse_start = source.index(
    "static Boolean ParseClientCommand", source.index("enum { PRS_NO_STATE")
)
parse_end = source.index("\n}\n#endif", parse_start)
parse = source[parse_start:parse_end]
assert "String exclusion_text = NULL;" in parse
assert "if (exclusion_text != NULL)" in parse
assert "StoreExclusion(menuSpec, exclusion_text);" in parse

start = source.index("static void StoreExclusion")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (menuSpec == NULL || string == NULL)" in region
assert "if (exclusion == NULL)" in region
assert "if (exclusion->command_string == NULL)" in region
assert "XtFree((char *)exclusion);" in region
assert "menuSpec->exclusions = exclusion;" in region
print("PASS MWM exclusion allocation is transactional")
