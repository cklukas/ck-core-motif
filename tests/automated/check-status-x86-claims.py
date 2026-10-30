#!/usr/bin/env python3
"""Keep bounded x86_64 evidence distinct from full qualification claims."""

from pathlib import Path

text = (Path(__file__).parents[2] / "docs" / "status.md").read_text(encoding="utf-8")

required = (
    "A bounded installed x86_64 cohort is recorded above",
    "complete architecture-wide\nvalidation remains a separate gate",
    "Final matrix-wide x86_64 qualification",
)
for phrase in required:
    if phrase not in text:
        raise SystemExit(f"missing current x86_64 status wording: {phrase!r}")

for stale in (
    "All x86_64 compilation and runtime validation remain pending.",
    "Final x86_64 validation of this candidate remains pending.",
    "final x86_64 validation remain separate",
):
    if stale in text:
        raise SystemExit(f"stale contradictory x86_64 claim remains: {stale!r}")

print("PASS x86_64 status distinguishes bounded evidence from full qualification")
