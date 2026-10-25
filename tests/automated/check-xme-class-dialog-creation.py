#!/usr/bin/env python3
"""Check that XmeCreateClassDialog rolls back a failed child creation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/DialogS.c").read_text()


def main() -> None:
    start = SOURCE.index("XmeCreateClassDialog(")
    end = SOURCE.index("/*\tAdd callback", start)
    body = SOURCE[start:end]
    assert "bb = XtCreateWidget(name, w_class, ds" in body
    assert "if (!bb)" in body
    assert "XtDestroyWidget(ds);" in body
    assert "return NULL;" in body


if __name__ == "__main__":
    main()
