#!/usr/bin/env python3
"""Check that copied MWM exclusions are built transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("DuplicateMenuExclusions (MenuExclusion *exclusions)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (newMenuExclusion == (MenuExclusion *) NULL)" in region
assert "if (newMenuExclusion->command_string == NULL)" in region
assert "goto failure;" in region
assert "XtFree((char *)returnMenuExclusion->command_string);" in region
assert "return (NULL);" in region
print("PASS MWM exclusion copying is transactional")
