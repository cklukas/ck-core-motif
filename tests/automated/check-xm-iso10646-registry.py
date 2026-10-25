#!/usr/bin/env python3
"""Check that ISO-10646 registry detection handles missing atom names."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/Xm.c").read_text()


def main() -> None:
    start = SOURCE.index("_XmIsISO10646")
    end = SOURCE.index("XChar2b*", start)
    body = SOURCE[start:end]
    assert "regname != NULL" in body
    assert "strcmp(regname, \"ISO10646\")" in body
    assert "XFree(regname);" in body


if __name__ == "__main__":
    main()
