#!/usr/bin/env python3
"""Keep package-specific library choices ahead of broad XQuartz search paths."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

helper = Path(__file__).resolve().parents[2] / "scripts/macos-pkg-libdirs.sh"
with tempfile.TemporaryDirectory(prefix="motif-pkg-dirs-") as temporary:
    root = Path(temporary)
    quartz, brew, cairo, xft = (root / p for p in ("quartz", "brew", "cairo/lib", "xft/lib"))
    for p in (quartz, brew, cairo, xft): p.mkdir(parents=True)
    for name in ("libcairo.dylib", "libX11.dylib"): (quartz / name).write_text("old")
    (cairo / "libcairo.2.dylib").write_text("selected cairo")
    (cairo / "libcairo.dylib").symlink_to("libcairo.2.dylib")
    (brew / "libcairo.dylib").symlink_to(cairo / "libcairo.dylib")
    (xft / "libXft.dylib").write_text("selected xft")
    pkg = root / "pkg-config"
    pkg.write_text("#!" + sys.executable + "\nimport pathlib,sys\nr=pathlib.Path(__file__).parent\n"
        "if sys.argv[1]=='--variable=libdir': print(r/'quartz')\n"
        "elif sys.argv[2]=='xft': print('-L'+str(r/'xft/lib')+' -lXft -L'+str(r/'quartz')+' -lX11')\n"
        "else: print('-L'+str(r/'brew')+' -lcairo -lm')\n")
    pkg.chmod(0o755)
    result = subprocess.check_output(["sh", str(helper), str(pkg), "xft", "svg", "svg"], text=True)
    assert result.split() == ["-L" + str(xft), "-L" + str(cairo)], result
print("PASS macOS package directory precedence, symlinks, deduplication and system libraries")
