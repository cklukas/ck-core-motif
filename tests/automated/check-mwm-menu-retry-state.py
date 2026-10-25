#!/usr/bin/env python3
"""Check that MWM menu construction clears retry state on child failure."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmMenu.c").read_text()


def main() -> None:
    start = SOURCE.index("Widget CreateMenuWidget")
    end = SOURCE.index("/* END OF FUNCTION CreateMenuWidget */", start)
    body = SOURCE[start:end]
    assert body.count("menuSpec->currentContext |= CR_MENU_MARK") == 1
    assert body.count("menuSpec->currentContext &= ~CR_MENU_MARK") >= 10
    assert "XmStringFree(tmpstr);" in body
    assert "if (children[n] == NULL)" in body


if __name__ == "__main__":
    main()
