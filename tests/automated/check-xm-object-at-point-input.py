#!/usr/bin/env python3
"""Check that XmObjectAtPoint validates its widget input."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/Xm.c").read_text()


def main() -> None:
    start = SOURCE.index("XmObjectAtPoint(")
    end = SOURCE.index("#ifdef FIX_1381", start)
    body = SOURCE[start:end]
    assert "if (wid == NULL)" in body
    assert "return NULL;" in body
    assert body.index("if (wid == NULL)") < body.index("XtClass(wid)")


if __name__ == "__main__":
    main()
