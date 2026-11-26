#!/usr/bin/env python3
"""Verify the WP-42 hardening defect-warning ratchet and baseline ledger."""

from pathlib import Path
import json
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
M4 = (ROOT / "ck_hardening.m4").read_text(encoding="utf-8")
BASELINE_PATH = ROOT / "docs" / "hardening-baseline-validation.json"
BASELINE = json.loads(BASELINE_PATH.read_text(encoding="utf-8"))
SCRIPT = ROOT / "scripts" / "warning-baseline.py"

PROMOTED_FLAGS = [
    "-Werror=format-security",
    "-Werror=implicit-fallthrough=3",
    "-Werror=null-dereference",
    "-Werror=misleading-indentation",
    "-Werror=cast-function-type",
    "-Werror=pointer-to-int-cast",
    "-Werror=int-to-pointer-cast",
    "-Werror=maybe-uninitialized",
    "-Werror=uninitialized",
    "-Werror=address",
    "-Werror=missing-field-initializers",
    "-Werror=sign-compare",
]

assert 'CFLAGS="$CFLAGS $$2 -Werror $1"' in M4, (
    "ck_hardening.m4 must include accumulated flags when probing dependent -Werror flags"
)
for flag in PROMOTED_FLAGS:
    assert f"_CK_HARDENING_FLAG([{flag}], [ck_hardening_flags])" in M4, (
        f"missing promoted flag {flag} in ck_hardening.m4"
    )

warnings = BASELINE["warnings"]
assert warnings["defect_class_warnings"] == 0, (
    f"expected 0 defect_class_warnings, found {warnings['defect_class_warnings']}"
)
assert warnings["defect_classes"] == {}, (
    f"expected empty defect_classes, found {warnings['defect_classes']}"
)

sys.path.insert(0, str(ROOT / "scripts"))
import importlib.util

spec = importlib.util.spec_from_file_location("warning_baseline", SCRIPT)
wb = importlib.util.module_from_spec(spec)
spec.loader.exec_module(wb)

for comp, classes in warnings["components"].items():
    for cls in wb.DEFECT_CLASSES:
        assert classes.get(cls, 0) == 0, (
            f"component {comp} still records {cls}: {classes.get(cls)}"
        )

cppcheck = BASELINE["cppcheck"]
assert cppcheck["by_severity"].get("error", 0) == 0, (
    f"expected 0 cppcheck errors, found {cppcheck['by_severity']}"
)
assert cppcheck["error_ids"] == {}, (
    f"expected empty cppcheck error_ids, found {cppcheck['error_ids']}"
)
for comp, counts in cppcheck["components"].items():
    assert counts.get("error", 0) == 0, (
        f"component {comp} still records cppcheck errors: {counts}"
    )

with tempfile.TemporaryDirectory() as tmp:
    clean_log = Path(tmp) / "clean.log"
    clean_log.write_text(
        "make[1]: Entering directory '/build/lib/Xm'\n"
        "make[1]: Leaving directory '/build/lib/Xm'\n",
        encoding="utf-8",
    )
    res = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "compare",
            str(clean_log),
            str(BASELINE_PATH),
            "--strip",
            "/build",
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    assert res.returncode == 0, f"clean log failed compare: {res.stdout} {res.stderr}"

    reg_log = Path(tmp) / "reg.log"
    reg_log.write_text(
        "make[1]: Entering directory '/build/lib/Xm'\n"
        "Text.c:100:5: error: potential null pointer dereference [-Werror=null-dereference]\n"
        "make[1]: Leaving directory '/build/lib/Xm'\n",
        encoding="utf-8",
    )
    res_reg = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "compare",
            str(reg_log),
            str(BASELINE_PATH),
            "--strip",
            "/build",
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    assert res_reg.returncode == 1, "defect-class regression did not fail compare"
    assert "regression: lib/Xm -Wnull-dereference: 0 -> 1" in res_reg.stdout, (
        f"unexpected compare output: {res_reg.stdout}"
    )

print("PASS hardening defect-warning ratchet is enforced")
