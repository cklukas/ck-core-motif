#!/usr/bin/env python3
"""Check that MWM property arrays guard multiplication and Xt width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    protocols_start = text.index("pCD->clientProtocols =")
    protocols = text[protocols_start : text.index("pCD->mwmMessages =", protocols_start)]
    messages = text
    assert "nitems > SIZE_MAX / sizeof(Atom)" in protocols
    assert "nitems > UINT_MAX / sizeof(Atom)" in protocols
    assert "(Cardinal)(nitems * sizeof(Atom))" in protocols
    assert "nitems > SIZE_MAX / sizeof(long)" in messages
    assert "nitems > UINT_MAX / sizeof(long)" in messages
    assert "(Cardinal)(nitems * sizeof(long))" in messages
    print("MWM property arrays are multiplication-checked")


if __name__ == "__main__":
    main()
