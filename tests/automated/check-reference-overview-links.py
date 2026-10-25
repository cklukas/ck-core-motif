#!/usr/bin/env python3
"""Ensure the topical reference overview only links to tracked docs pages."""
from pathlib import Path
import re

root = Path(__file__).parents[2]
overview = (root / "docs/reference-overview.md").read_text(encoding="utf-8")
links = re.findall(r"\]\(([^)#]+\.md)\)", overview)
assert links, "reference overview must contain documentation links"
missing = sorted({link for link in links if not (root / "docs" / link).is_file()})
assert not missing, "reference overview links missing pages: " + ", ".join(missing)
assert len(links) == len(set(links)), "reference overview contains duplicate links"
print(f"PASS reference overview validates {len(links)} documentation links")
