#!/usr/bin/env python3
"""Check that clipboard format enumeration handles missing atom names."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("str =  XGetAtomName")
    end = SOURCE.index("temp = strlen(str);", start)
    body = SOURCE[start:end]
    assert "if (str == NULL)" in body
    assert "atomptr++;" in body
    assert "continue;" in body


if __name__ == "__main__":
    main()
