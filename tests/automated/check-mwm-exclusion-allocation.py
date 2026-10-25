#!/usr/bin/env python3
"""Check that MWM menu exclusions are added transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.index("static void StoreExclusion")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (menuSpec == NULL || string == NULL)" in region
assert "if (exclusion == NULL)" in region
assert "if (exclusion->command_string == NULL)" in region
assert "XtFree((char *)exclusion);" in region
assert "menuSpec->exclusions = exclusion;" in region
print("PASS MWM exclusion allocation is transactional")
