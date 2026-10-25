#!/usr/bin/env python3
"""Guard the Unicode configure probe against C23-only atomic breakage."""

from pathlib import Path


source = Path(__file__).resolve().parents[2] / "configure.ac"
text = source.read_text(encoding="utf-8")
if "atomic_init(&epoch, 0);" not in text:
    raise SystemExit("configure probe must initialize atomics with atomic_init")
if "ATOMIC_VAR_INIT" in text:
    raise SystemExit("configure probe must not use removed ATOMIC_VAR_INIT")
print("PASS configure atomic probe uses C11/C23-compatible initialization")
