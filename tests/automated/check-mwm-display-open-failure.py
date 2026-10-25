#!/usr/bin/env python3
"""Check that MWM does not continue after display-open failure."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmInitWs.c").read_text()


def main() -> None:
    block_start = SOURCE.index("if (!wmGD.display)")
    block_end = SOURCE.index("#if defined(sun)", block_start)
    block = SOURCE[block_start:block_end]
    assert "ExitWM (WM_ERROR_EXIT_VALUE);" in block
    assert "return;" in block


if __name__ == "__main__":
    main()
