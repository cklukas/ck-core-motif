#!/usr/bin/env python3
"""Check that MWM application shell creation is guarded."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmInitWs.c").read_text()


def main() -> None:
    assert "if (wmGD.topLevelW == NULL)" in SOURCE
    assert 'Warning ("Cannot create the MWM application shell")' in SOURCE
    assert "if (!wmGD.display1)" in SOURCE
    assert "if (wmGD.topLevelW1 == NULL)" in SOURCE
    assert 'Warning ("Cannot create the secondary MWM application shell")' in SOURCE


if __name__ == "__main__":
    main()
