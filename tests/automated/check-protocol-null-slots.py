#!/usr/bin/env python3
"""Check that protocol teardown tolerates null slots."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
needle = """          if (p_mgr->protocols[i] == NULL)
            continue;
          _XmRemoveAllCallbacks(
"""
if needle not in text:
    raise SystemExit("protocol null-slot guard is missing")

print("protocol null-slot teardown guard present")
