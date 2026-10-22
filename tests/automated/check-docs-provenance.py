#!/usr/bin/env python3
"""Verify pinned source-archive provenance without compiling or rendering docs.

Supply a git-archive tar of the pinned tool revision and a fresh absolute scratch
--work path. No network access, documentation build or tool build occurs.
"""
import argparse
import importlib.util
import os
from pathlib import Path
import sys
import tarfile

sys.dont_write_bytecode = True
root = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("docs_build", root / "scripts/build-docs.py")
build = importlib.util.module_from_spec(spec)
spec.loader.exec_module(build)


def rejected(operation):
    try:
        operation()
    except ValueError:
        return
    raise AssertionError("unverified source was accepted")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tool-archive", required=True, type=Path)
    parser.add_argument("--work", required=True, type=Path)
    args = parser.parse_args()
    work = args.work.resolve()
    if not args.work.is_absolute() or work.exists():
        parser.error("--work must be an explicit fresh absolute scratch path")
    if sys.platform == "darwin" and not any(base.parent.is_mount() and work.is_relative_to(base)
            for base in (Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp"))):
        parser.error("select a mounted scratch volume's tmp path on macOS")
    work.mkdir(parents=True)
    source = work / "tool"
    source.mkdir()
    with tarfile.open(args.tool_archive) as archive:
        archive.extractall(source, filter="data")
    env = dict(os.environ, TMPDIR=str(work))
    origin = build.tool_provenance(source, env)
    assert origin == dict(kind="verified-export-archive", commit=build.TOOL_COMMIT, tree=build.TOOL_TREE)
    for name in ("VERSION", "src/ckdocs/main.cpp", ".ckgit/build-commit"):
        path = source / name
        before = path.read_bytes()
        path.write_bytes(before + b"\n")
        rejected(lambda: build.tool_provenance(source, env))
        path.write_bytes(before)
    path = source / "src/ckdocs/main.cpp"
    before = path.read_bytes()
    path.unlink()
    rejected(lambda: build.tool_provenance(source, env))
    path.write_bytes(before)
    mode = path.stat().st_mode
    path.chmod(mode ^ 0o100)
    rejected(lambda: build.tool_provenance(source, env))
    path.chmod(mode)
    for name in ("local.mk", "unknown.cpp"):
        path = source / name
        path.write_text("extra input\n")
        rejected(lambda: build.tool_provenance(source, env))
        path.unlink()
    project = work / "project"
    (project / ".ckgit").mkdir(parents=True)
    stamp = project / ".ckgit/build-commit"
    stamp.write_text("a" * 40 + "\n")
    assert build.source_provenance(project, {})["commit"] == "a" * 40
    assert build.source_provenance(project, dict(CKGIT_CI="1", CKGIT_COMMIT="a" * 40))["commit"] == "a" * 40
    rejected(lambda: build.source_provenance(project, dict(CKGIT_CI="1", CKGIT_COMMIT="b" * 40)))
    stamp.unlink()
    rejected(lambda: build.source_provenance(project, {}))
    assert build.source_provenance(project, dict(CKGIT_CI="1", CKGIT_COMMIT="b" * 40))["kind"] == "ci-archive"
    print("Exact tool tree accepted; modified/extra/override inputs and unknown/conflicting project identities refused")


if __name__ == "__main__":
    main()
