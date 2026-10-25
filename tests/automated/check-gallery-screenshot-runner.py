#!/usr/bin/env python3
"""Check the display-backed gallery screenshot runner's safety contract."""
from pathlib import Path


source = (Path(__file__).parents[2] /
          "tests" / "automated" / "run-gallery-screenshots.py").read_text()

assert 'WINDOW_NAME = "ckmotif-gallery"' in source
assert "xwininfo" in source and '"-root", "-tree"' in source
assert "width > 1 and height > 1" in source
assert "max(candidates)" in source
assert '"-id", window_id' in source
assert "xwdtopnm" in source and "pnmtopng" in source
assert "process_failure" in source and "process.communicate" in source
assert '"result": "fail"' in source
assert 'receipt["result"] = "skip"' in source
assert 'receipt["result"] = "pass"' in source
assert 'receipt["current_entry"]' in source
print("PASS gallery screenshot runner contract")
