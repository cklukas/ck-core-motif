#!/usr/bin/env python3
"""WP-45 regression check: declarative Xrm startup appearance and density."""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]


def check(cond: bool, msg: str) -> None:
    if not cond:
        print(f"FAIL: {msg}", file=sys.stderr)
        raise SystemExit(1)


def main() -> int:
    header = (ROOT / "lib/Xm/Appearance.h").read_text(encoding="utf-8")
    for macro in (
        '#define XmNappearanceProfile "appearanceProfile"',
        '#define XmCAppearanceProfile "AppearanceProfile"',
        '#define XmRAppearanceProfile "AppearanceProfile"',
        '#define XmNappearanceDensity "appearanceDensity"',
        '#define XmCAppearanceDensity "AppearanceDensity"',
        '#define XmRAppearanceDensity "AppearanceDensity"',
    ):
        check(macro in header, f"lib/Xm/Appearance.h missing {macro}")

    impl = (ROOT / "lib/Xm/Appearance.c").read_text(encoding="utf-8")
    check("_XmAppearanceInitializeDisplay(Widget xm_display)" in impl,
          "lib/Xm/Appearance.c missing _XmAppearanceInitializeDisplay")
    check("lookup_appearance_resource(" in impl,
          "lib/Xm/Appearance.c missing lookup_appearance_resource")
    check("Cannot convert appearanceProfile resource to XmRAppearanceProfile; falling back to classic" in impl,
          "lib/Xm/Appearance.c missing appearanceProfile conversion warning")
    check("Cannot convert appearanceDensity resource to XmRAppearanceDensity; falling back to comfortable" in impl,
          "lib/Xm/Appearance.c missing appearanceDensity conversion warning")

    display_c = (ROOT / "lib/Xm/Display.c").read_text(encoding="utf-8")
    check("_XmAppearanceInitializeDisplay(new_widget);" in display_c,
          "lib/Xm/Display.c missing _XmAppearanceInitializeDisplay call")

    for rel in (
        "tests/automated/check-xrm-appearance-startup.c",
        "tests/automated/run-xrm-appearance-startup.py",
    ):
        check((ROOT / rel).is_file(), f"missing {rel}")

    print("ok: WP-45 declarative Xrm startup appearance and density verified")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
