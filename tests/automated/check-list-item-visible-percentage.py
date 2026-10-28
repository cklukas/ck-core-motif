#!/usr/bin/env python3
"""Check ListItemVisible parses and applies percentage parameters safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(encoding="utf-8")
start = source.index("static void\nListItemVisible(Widget wid")
end = source.index("\n}\n", start)
region = source[start:end]

assert "errno == ERANGE" in region
assert "parsed = strtol(*params, &end, 10)" in region
assert "*end != '\\0'" in region
assert "percentage = 0" in region
assert "percentage = 100" in region
assert "(lw->list.visibleItemCount / 100) * percentage" in region
assert "sscanf(*params" not in region
print("PASS ListItemVisible percentage parsing is range-safe")
