#!/usr/bin/env python3
"""Build strict documentation with the same pinned source tool on both hosts."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import time

TOOL_COMMIT = "5019729cd37c06ae4e0c1d2c07e4127f3a72394d"
TOOL_VERSION = "0.8.1"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tool-source", required=True, type=Path)
    parser.add_argument("--work", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--site-url", required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    tool = args.tool_source.resolve(strict=True)
    work = args.work.resolve()
    output = args.out.resolve()
    if not args.work.is_absolute() or not args.out.is_absolute():
        parser.error("--work and --out must be explicit absolute paths")
    if work.exists() or output.exists():
        parser.error("--work and --out must be fresh paths, without stale output")
    if sys.platform == "darwin":
        allowed = [Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp")]
        if not any(base.parent.is_mount() and work.is_relative_to(base) for base in allowed):
            parser.error("on macOS --work must be beneath a mounted scratch volume's tmp directory")
        if not any(base.parent.is_mount() and output.is_relative_to(base) for base in allowed):
            parser.error("on macOS --out must be beneath a mounted scratch volume's tmp directory")
    if work == output or work.is_relative_to(output):
        parser.error("--out must not contain --work")
    work.mkdir(parents=True)
    (work / "tmp").mkdir()
    env = dict(os.environ, GIT_OPTIONAL_LOCKS="0", TMPDIR=str(work / "tmp"))
    # Read-only sisters are supported: git queries and make never write here.
    actual = subprocess.check_output(["git", "-C", str(tool), "rev-parse", "HEAD"], env=env, text=True).strip()
    if actual != TOOL_COMMIT or (tool / "VERSION").read_text().strip() != TOOL_VERSION:
        parser.error("tool source must be the documented exact commit/version")
    dirty = subprocess.check_output(["git", "-C", str(tool), "status", "--porcelain", "--untracked-files=all"], env=env, text=True)
    if dirty:
        parser.error("tool source must be clean, including no untracked local.mk or other build inputs")
    inputs = [root / "ckdocs.yml", root / "scripts/build-docs.py"]
    inputs.extend(sorted((root / "docs").rglob("*.md")))
    inputs.extend([root / ".ckgit/ci.yml", root / ".github/workflows/pages.yml"])
    fingerprints = {str(path.relative_to(root)): digest(path) for path in inputs}
    receipt = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "target": "strict documentation site",
        "configuration": "source-pinned ckdocs; tool target only; no network downloads",
        "tool_commit": TOOL_COMMIT,
        "tool_version": TOOL_VERSION,
        "tool_tree": subprocess.check_output(["git", "-C", str(tool), "rev-parse", "HEAD^{tree}"], env=env, text=True).strip(),
        "source_commit": subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], env=env, text=True).strip(),
        "source_status": subprocess.check_output(["git", "-C", str(root), "status", "--porcelain", "--untracked-files=all"], env=env, text=True),
        "source_fingerprints": fingerprints,
        "output": str(output),
        "duration_baseline": "No automatic baseline is assumed; compare retained equivalent build receipts.",
        "steps": [],
        "success": False,
    }
    began = time.monotonic()
    binary = work / "build/bin/ckdocs"
    try:
        commands = [
            ("tool", ["make", "-j2", "-C", str(tool), "BUILD_ROOT=" + str(work),
                      "BUILD_DIR=" + str(work / "build"), str(binary)]),
            ("site", [str(binary), "build", "--root", str(root), "--out", str(output),
                      "--site-url", args.site_url, "--strict"]),
        ]
        for name, command in commands:
            step_start = time.monotonic()
            step_start_ns = time.time_ns()
            with (work / (name + ".log")).open("w") as log:
                result = subprocess.run(command, cwd=root, env=env, stdout=log, stderr=subprocess.STDOUT)
            receipt["steps"].append({"name": name, "command": command,
                "started_ns": step_start_ns, "ended_ns": time.time_ns(),
                "seconds": time.monotonic() - step_start, "exit": result.returncode})
            if result.returncode:
                print("\n".join((work / (name + ".log")).read_text(errors="replace").splitlines()[-100:]), file=sys.stderr)
                raise RuntimeError("failed " + name + "; see " + str(work / (name + ".log")))
            if name == "tool":
                if not (step_start_ns <= binary.stat().st_mtime_ns <= time.time_ns()):
                    raise RuntimeError("tool timestamp does not establish a fresh build")
                receipt["tool_binary_sha256"] = digest(binary)
                receipt["tool_reported_version"] = subprocess.check_output([str(binary), "--version"], env=env, text=True).strip()
        if subprocess.check_output(["git", "-C", str(tool), "status", "--porcelain", "--untracked-files=all"], env=env, text=True):
            raise RuntimeError("pinned tool source changed during its read-only build")
        if fingerprints != {str(path.relative_to(root)): digest(path) for path in inputs}:
            raise RuntimeError("documentation input changed during the build")
        pages = sorted(output.rglob("*.html"))
        if not pages:
            raise RuntimeError("successful command produced no HTML pages")
        # Content verification is independent of the generator's exit status.
        for phrase in ["CK Core Motif", "Native ABI cohort", "Unicode text and input", "Native accessibility"]:
            if not any(phrase in page.read_text(encoding="utf-8") for page in pages):
                raise RuntimeError("missing rendered content: " + phrase)
        receipt["output_fingerprints"] = {str(path.relative_to(output)): digest(path)
            for path in sorted(output.rglob("*")) if path.is_file()}
        receipt["success"] = True
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["elapsed_seconds"] = time.monotonic() - began
        (work / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print("Verified strict documentation at", output)
    print("Build receipt:", work / "receipt.json")


if __name__ == "__main__":
    main()
