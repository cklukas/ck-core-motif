#!/usr/bin/env python3
"""Check every tracked documentation page is named by the site navigation."""
import re
from pathlib import Path

root = Path(__file__).parents[2]
config = (root / "ckdocs.yml").read_text(encoding="utf-8")
listed = set(re.findall(r"(?m)^\s+- (docs/[^\s#]+\.md)\s*$", config))
pages = {path.relative_to(root).as_posix(): path for path in (root / "docs").glob("*.md")}
missing = sorted(
    name for name, path in pages.items()
    if name not in listed and "nav_exclude: true" not in path.read_text(encoding="utf-8")[:512]
)
assert not missing, "documentation pages missing from nav or nav_exclude: " + ", ".join(missing[:8])
excluded = len(pages) - len(listed & pages.keys())
print(f"PASS documentation nav covers {len(pages) - excluded} pages and excludes {excluded} reference pages")
