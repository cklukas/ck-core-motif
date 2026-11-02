#!/usr/bin/env python3
"""Exercise the configure helper against coherent and mixed dependency graphs."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

helper = Path(__file__).resolve().parents[2] / "scripts/macos-font-libs.sh"
with tempfile.TemporaryDirectory(prefix="motif-font-cohort-") as temporary:
    root = Path(temporary)
    packages = {"xft": "libXft", "freetype2": "libfreetype", "fontconfig": "libfontconfig",
                "harfbuzz": "libharfbuzz", "x11": "libX11"}
    libs = {}
    for package, name in packages.items():
        directory = root / package
        directory.mkdir()
        libs[package] = directory / (name + ".dylib")
        libs[package].write_text(package)
    pkg = root / "pkg-config"
    pkg.write_text("#!" + sys.executable + "\nimport pathlib,sys\nprint(pathlib.Path(__file__).parent/sys.argv[2])\n")
    tool = root / "otool"
    tool.write_text("#!" + sys.executable + "\nimport pathlib,sys\nprint(pathlib.Path(sys.argv[2]+'.deps').read_text())\n")
    pkg.chmod(0o755)
    tool.chmod(0o755)
    env = dict(os.environ, OTOOL=str(tool))
    def dependencies(package, *paths):
        Path(str(libs[package]) + ".deps").write_text("\n".join(str(p) + " (compatibility version 1.0.0)" for p in paths))
    def run(ok, unicode="yes"):
        result = subprocess.run(["sh", str(helper), str(pkg), unicode], env=env, text=True, capture_output=True)
        assert (result.returncode == 0) == ok, result.stderr
        if ok:
            assert "-L" + str(libs["xft"].parent) + " -lXft" in result.stdout
        else:
            assert "selected stack" in result.stderr
    dependencies("xft", libs["freetype2"], libs["fontconfig"], libs["x11"])
    dependencies("fontconfig", libs["freetype2"])
    dependencies("harfbuzz", libs["freetype2"])
    run(True)
    other = root / "other"
    other.mkdir()
    wrong = other / "libfreetype.dylib"
    wrong.write_text("a different loaded FreeType")
    dependencies("harfbuzz", wrong)
    run(False)
    run(True, "no")
    dependencies("harfbuzz", libs["freetype2"])
    dependencies("fontconfig", wrong)
    run(False)
    alias = other / "libfreetype.6.dylib"
    alias.symlink_to(libs["freetype2"])
    dependencies("fontconfig", alias)
    run(True)
    wrong_x11 = other / "libX11.dylib"
    wrong_x11.write_text("a different Xlib")
    dependencies("xft", libs["freetype2"], libs["fontconfig"], wrong_x11)
    run(False)
print("PASS macOS font dependency cohorts: 6 cases")
