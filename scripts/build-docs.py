#!/usr/bin/env python3
"""Build documentation with the same pinned source tool on both hosts."""
import argparse
import datetime
import hashlib
import json
import os
import re
import stat
from pathlib import Path
import subprocess
import sys
import time

TOOL_COMMIT = "5019729cd37c06ae4e0c1d2c07e4127f3a72394d"
TOOL_VERSION = "0.8.1"
TOOL_TREE = "4105740b7e806ecf3eba8ae5c739fc8bafbd9122"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def git_text(source, env, *arguments):
    return subprocess.check_output(["git", "-C", str(source), *arguments], env=env, text=True).strip()


def archive_tree(source):
    """Reconstruct Git object identity; normalize only the verified export stamp."""
    def object_id(kind, data):
        return hashlib.sha1(kind + b" " + str(len(data)).encode("ascii") + b"\0" + data).digest()

    def tree(directory):
        entries = []
        for path in directory.iterdir():
            name = os.fsencode(path.name)
            mode = path.lstat().st_mode
            if stat.S_ISDIR(mode):
                entries.append((name + b"/", b"40000 " + name + b"\0" + tree(path)))
            elif stat.S_ISLNK(mode):
                entries.append((name, b"120000 " + name + b"\0" + object_id(b"blob", os.fsencode(os.readlink(path)))))
            elif stat.S_ISREG(mode):
                data = path.read_bytes()
                if path.relative_to(source).as_posix() == ".ckgit/build-commit":
                    data = b"$Format:%H$\n"
                file_mode = b"100755" if mode & stat.S_IXUSR else b"100644"
                entries.append((name, file_mode + b" " + name + b"\0" + object_id(b"blob", data)))
            else:
                raise ValueError("unsupported archive entry: " + str(path))
        return object_id(b"tree", b"".join(value for _, value in sorted(entries)))
    return tree(source).hex()


def stamp(source):
    path = source / ".ckgit/build-commit"
    value = path.read_text().strip() if path.is_file() else ""
    return value if re.fullmatch(r"[0-9a-f]{40}", value) else None


def tool_provenance(tool, env):
    if (tool / "local.mk").exists():
        raise ValueError("tool source must not contain local.mk, including an ignored override")
    if (tool / "VERSION").read_text().strip() != TOOL_VERSION:
        raise ValueError("tool source VERSION does not match the pin")
    if (tool / ".git").exists():
        commit = git_text(tool, env, "rev-parse", "HEAD")
        tree = git_text(tool, env, "rev-parse", "HEAD^{tree}")
        if commit != TOOL_COMMIT or tree != TOOL_TREE:
            raise ValueError("tool Git checkout does not match the exact commit/tree pin")
        if git_text(tool, env, "status", "--porcelain", "--untracked-files=all"):
            raise ValueError("tool Git source must be clean, including untracked inputs")
        return dict(kind="clean-git", commit=commit, tree=tree)
    if not (tool / ".ckgit/build-commit").is_file() or (tool / ".ckgit/build-commit").read_bytes() != (TOOL_COMMIT + "\n").encode("ascii"):
        raise ValueError("tool archive needs the exact export-subst commit stamp")
    tree = archive_tree(tool)
    if tree != TOOL_TREE:
        raise ValueError("tool archive bytes/modes/paths do not match the pinned Git tree")
    return dict(kind="verified-export-archive", commit=TOOL_COMMIT, tree=tree)


def source_provenance(source, env):
    if (source / ".git").exists():
        return dict(kind="git", commit=git_text(source, env, "rev-parse", "HEAD"),
                    status=git_text(source, env, "status", "--porcelain", "--untracked-files=all"))
    exported = stamp(source)
    runner = env.get("CKGIT_COMMIT", "") if env.get("CKGIT_CI") == "1" else ""
    if runner and not re.fullmatch(r"[0-9a-f]{40}", runner):
        raise ValueError("invalid authoritative CI source commit")
    if exported and runner and exported != runner:
        raise ValueError("source archive stamp conflicts with authoritative CI commit")
    if not exported and not runner:
        raise ValueError("source archive needs an export stamp or authoritative CI commit")
    return dict(kind="export-archive" if exported else "ci-archive", commit=exported or runner,
                status="Git dirty state unavailable; exact documentation input hashes recorded")


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
    # A sister may be a clean checkout or a byte-verified exported archive.
    try:
        tool_origin = tool_provenance(tool, env)
        source_origin = source_provenance(root, env)
    except ValueError as error:
        parser.error(str(error))
    inputs = [root / "ckdocs.yml", root / "scripts/build-docs.py",
              root / "scripts/refresh-gallery-screenshots.py"]
    inputs.extend(sorted(path for path in (root / "docs").rglob("*") if path.is_file()))
    inputs.extend([root / ".ckgit/ci.yml", root / ".github/workflows/pages.yml",
                   root / ".gitattributes", root / ".ckgit/build-commit"])
    fingerprints = {str(path.relative_to(root)): digest(path) for path in inputs}
    receipt = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "target": "documentation site",
        "configuration": "source-pinned ckdocs; tool target only; no network downloads",
        "tool_commit": TOOL_COMMIT,
        "tool_version": TOOL_VERSION,
        "tool_provenance": tool_origin,
        "source_provenance": source_origin,
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
                      "BUILD_DIR=" + str(work / "build"),
                      "CKGIT_BUILD_VERSION=" + TOOL_VERSION + "+g" + TOOL_COMMIT[:12], str(binary)]),
            ("site", [str(binary), "build", "--root", str(root), "--out", str(output),
                      "--site-url", args.site_url]),
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
        if tool_provenance(tool, env) != tool_origin:
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
    print("Verified documentation at", output)
    print("Build receipt:", work / "receipt.json")


if __name__ == "__main__":
    main()
