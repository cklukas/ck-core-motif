#!/usr/bin/env python3
"""Check MWM startup and named-icon helpers reject incomplete inputs."""
from pathlib import Path

root = Path(__file__).parents[2]
main = (root / "clients" / "mwm" / "WmMain.c").read_text(encoding="utf-8")
init = (root / "clients" / "mwm" / "WmInitWs.c").read_text(encoding="utf-8")
image = (root / "clients" / "mwm" / "WmImage.c").read_text(encoding="utf-8")

main_region = main[main.index("main (int argc"):main.index("setlocale(LC_ALL, \"\");")]
assert "argc <= 0 || argv == NULL || argv[0] == NULL" in main_region
assert "return (WM_ERROR_EXIT_VALUE);" in main_region

copy_start = init.index("void CopyArgv")
copy_region = init[copy_start:init.index("} /* END OF FUNCTION CopyArgv */", copy_start)]
assert "argc < 0 || (argc > 0 && argv == NULL)" in copy_region
assert "wmGD.argv = NULL;" in copy_region
assert "dpy2Argv = NULL;" in copy_region
assert "dpy2Argc = 0;" in copy_region

image_start = image.index("Pixmap GetNamedPixmap")
image_region = image[image_start:image.index("\n}\n", image_start) + 2]
assert "if (scr != NULL && iconName)" in image_region

print("PASS MWM startup, argv-copy, and named-icon inputs are guarded")
