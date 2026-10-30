#!/usr/bin/env python3
"""Check Avatar expose rendering skips work when its graphics context is absent."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void Redisplay(")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]

needle = "if (!window || !avatar->avatar.foreground_gc) return;"
if needle not in region:
    raise SystemExit("Avatar rendering must reject a missing graphics context")
if region.index("XClearWindow", region.index(needle)) < region.index(needle):
    raise SystemExit("Avatar graphics-context guard must precede window clearing")

print("PASS Avatar rendering skips missing graphics contexts")
