#!/usr/bin/env python3
"""Check that CutPaste conversion guards failed atom-name lookups."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    target = SOURCE.index("format_name = XGetAtomName")
    assert "if (format_name == NULL)" in SOURCE[target:target + 180]
    type_lookup = SOURCE.index("temp = XGetAtomName")
    assert "if (temp == NULL)" in SOURCE[type_lookup:type_lookup + 180]
    assert "*type = loc_type;" in SOURCE[type_lookup:type_lookup + 220]


if __name__ == "__main__":
    main()
