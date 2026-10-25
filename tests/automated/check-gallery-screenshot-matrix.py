#!/usr/bin/env python3
"""Check the profile/scale screenshot matrix runner contract."""
from pathlib import Path


source = (Path(__file__).resolve().parent /
          "run-gallery-screenshot-matrix.py").read_text()
for contract in (
        "APPEARANCES", "parse_appearances", "parse_scales",
        "run-gallery-screenshots.py", '"result": "fail"',
        'receipt["result"] = "skip"', 'receipt["result"] = "pass"',
        '"runner_sha256"', '"scenarios"', 'retain_captures',
        '".xwd", ".png"', 'timeout=900'):
    assert contract in source, f"matrix runner missing {contract}"
print("PASS gallery screenshot matrix runner contract")
