#!/usr/bin/env python3
"""Check MWM reserves preprocessor output before invoking its command."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients/mwm/WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nPreprocessConfigFile (void)")
end = source.index("\n}\n\n\f", start) + 2
body = source[start:end]

required = (
    'const char *tempDir = getenv("TMPDIR")',
    "tempDir = P_tmpdir;",
    'sizeof("/mwm-cpp-XXXXXX")',
    "tempFd = mkstemp(pConfigStackTop->cppName);",
    "if (tempFd < 0)",
    "if (close(tempFd) != 0)",
    "(void)unlink(pConfigStackTop->cppName);",
    "SystemCmd(pchCmd);",
)
for needle in required:
    if needle not in body:
        raise SystemExit(f"missing secure MWM preprocessor tempfile guard: {needle}")
if "(void) tmpnam" in body or "CPP_NAME_SIZE" in body:
    raise SystemExit("MWM preprocessor must not use an unreserved temporary name")
if body.index("tempFd = mkstemp") > body.index("SystemCmd(pchCmd);"):
    raise SystemExit("MWM must reserve the temporary output before running cpp")

print("PASS MWM preprocessor output uses an atomically reserved tempfile")
