#!/usr/bin/env python3
"""Check that MWM X-error format copies fit Xt's allocation width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmError.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("void\nWmInitErrorHandler")
    end = text.index("#endif /* DEBUG */", start)
    region = text[start:end]
    assert "strlen(buffer) >= UINT_MAX" in region
    assert "XtMalloc((Cardinal)(strlen(buffer) + 1))" in region
    assert "XtMalloc (1+strlen(buffer))" not in region
    print("MWM X-error format copies are width-checked")


if __name__ == "__main__":
    main()
