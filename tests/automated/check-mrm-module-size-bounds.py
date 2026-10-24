#!/usr/bin/env python3
"""Check Mrm module descriptor sizing before legacy narrowing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmmodule.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIFMInitModule (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "size_t\t\tdescsiz",
    "(SIZE_MAX - sizeof(RGMModuleDesc)) / sizeof(RGMTopmostDesc)",
    "descsiz > MrmMaxResourceSize",
    "descsiz > SIZE_MAX - _slm",
):
    if needle not in region:
        raise SystemExit(f"Mrm module sizing guard is missing: {needle}")

print("Mrm module descriptor sizing is bounded")
