#!/usr/bin/env python3
"""Check session command-vector growth fits Xt's Cardinal width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("iSizeArgv = ARG_AMT")
    end = text.index("pSD->pDtSessionItems[count].commandArgv = NULL", start)
    region = text[start:end]
    assert "iSizeArgv > SIZE_MAX / sizeof(char *)" in region
    assert "iSizeArgv > UINT_MAX / sizeof(char *)" in region
    assert "(Cardinal)(iSizeArgv * sizeof(char *))" in region
    assert "(size_t)argc > UINT_MAX / sizeof(char *)" in text
    assert "XtMalloc((Cardinal)((size_t)argc * sizeof(char *)))" in text
    print("MWM session command vectors are Cardinal-width checked")


if __name__ == "__main__":
    main()
