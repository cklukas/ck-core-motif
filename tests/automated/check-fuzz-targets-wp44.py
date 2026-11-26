#!/usr/bin/env python3
"""Verify WP-44 libFuzzer target expansion and parser hardening fixes."""

from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def read(rel: str) -> str:
    return (ROOT / rel).read_text(encoding="utf-8", errors="replace")


# 1. Verify all 7 libFuzzer targets are registered and have seed corpora
run_fuzz = read("tests/fuzz/run-fuzz.py")
make_seeds = read("tests/fuzz/make-seeds.py")
fuzz_readme = read("tests/fuzz/README")

expected_targets = (
    "editres-wire",
    "mrm-uid",
    "mwm-parsers",
    "uil-wmd",
    "wayland-x11-server",
    "xm-converters",
    "xmstring-stream",
    "xpm-image",
)

listed = subprocess.run(
    [sys.executable, str(ROOT / "tests/fuzz/run-fuzz.py"), "--list"],
    capture_output=True,
    text=True,
    check=True,
).stdout
listed_names = [line.split()[0] for line in listed.splitlines() if line.strip()]
assert tuple(listed_names) == expected_targets, listed_names

for target in expected_targets:
    harness = ROOT / "tests/fuzz" / f"fuzz-{target}.c"
    corpus = ROOT / "tests/fuzz/corpus" / target
    assert harness.is_file(), f"missing harness {harness}"
    assert corpus.is_dir(), f"missing corpus {corpus}"
    seeds = [p for p in corpus.iterdir() if p.is_file()]
    assert len(seeds) >= 3, f"expected >= 3 seeds in {corpus}, found {len(seeds)}"
    assert target in fuzz_readme, f"{target} missing from tests/fuzz/README"

for new_target in ("xpm-image", "mwm-parsers", "uil-wmd"):
    assert f'write("{new_target}"' in make_seeds, f"{new_target} missing from make-seeds.py"

# 2. Verify XPM parser hardening found by fuzz-xpm-image
xpmparse = read("lib/Xm/Xpmparse.c")
colors_fn = xpmparse[xpmparse.index("xpmParseColors(") : xpmparse.index("ParsePixels(")]
assert "data->type == XPMBUFFER" in colors_fn
assert "strnlen(data->cptr," in colors_fn
assert colors_fn.count("if (defaults[curkey])\n") >= 1 or colors_fn.count("if (defaults[curkey])") >= 2

pixels_fn = xpmparse[xpmparse.index("ParsePixels(") : xpmparse.index("xpmParseExtensions(")]
assert "data->type == XPMBUFFER" in pixels_fn
assert "strnlen(data->cptr," in pixels_fn

# 3. Verify MWM WSM wire packing/unpacking hardening found by fuzz-mwm-parsers
wsm_pack = read("clients/mwm/WmWsmLib/pack.c")
win_entry_fn = wsm_pack[
    wsm_pack.index("WinEntrySizePacked(Display *dpy,")
    : wsm_pack.index("WinDataSizePacked(Display *dpy,")
]
assert "size  = COUNT_SIZE;" in win_entry_fn
assert "!AddPackedSize(&size, COUNT_SIZE)" in win_entry_fn

unpack32_fn = wsm_pack[
    wsm_pack.index("UnpackCARD32(MessageData *data_ptr)")
    : wsm_pack.index("UnpackCARD16(MessageData *data_ptr)")
]
assert "((CARD32)UnpackCARD16(data_ptr)) << 16" in unpack32_fn

print("PASS WP-44 libFuzzer target expansion and parser fixes verified")
