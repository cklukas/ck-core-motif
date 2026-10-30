#!/usr/bin/env python3
"""Check Chip and IconLabel expose handlers require their drawing GCs."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
contracts = {
    "Chip.c": "if (!window || !chip->chip.foreground_gc) return;",
    "IconLabel.c": "if (!window || !label->icon_label.foreground_gc) return;",
}

for filename, guard in contracts.items():
    source = (root / filename).read_text(encoding="utf-8")
    start = source.rindex("static void Redisplay(")
    end = source.find("\nstatic ", start + len("static void Redisplay("))
    region = source[start:] if end < 0 else source[start:end]
    if guard not in region:
        raise SystemExit(f"{filename} expose rendering must reject a missing GC")
    if region.index("XClearWindow", region.index(guard)) < region.index(guard):
        raise SystemExit(f"{filename} GC guard must precede window clearing")

print("PASS Chip and IconLabel rendering skip missing graphics contexts")
