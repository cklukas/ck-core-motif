"""Link native fixtures to the font libraries selected by the built toolkit."""
from pathlib import Path
import subprocess

def font_libraries(toolkit):
    names = ("libXft.", "libfontconfig.", "libfreetype.")
    found = {}
    output = subprocess.check_output(["otool", "-L", str(toolkit)], text=True)
    for line in output.splitlines():
        path = line.strip().split(" (", 1)[0]
        for name in names:
            if path.startswith("/") and Path(path).name.startswith(name):
                found[name] = path
    if set(found) != set(names):
        raise RuntimeError("toolkit must expose one coherent Xft/Fontconfig/FreeType cohort")
    return [found[name] for name in names]
