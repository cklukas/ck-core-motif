#!/usr/bin/env python3
"""Build the pinned Xt dependency with its INCR ownership correction on Linux.

Supply an existing archive; this tool never downloads, installs system packages,
or changes the system Xt. All outputs and temporary files use explicit paths.
"""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import tarfile
import time

ARCHIVE_SHA256 = "e0a774b33324f4d4c05b199ea45050f87206586d81655f8bef4dba434d931288"
p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--archive", type=Path, required=True)
p.add_argument("--output", type=Path, required=True)
p.add_argument("--prefix", type=Path)
p.add_argument("--cc", choices=("gcc", "clang"), default="gcc")
p.add_argument("--jobs", type=int, default=2)
a = p.parse_args()
if platform.system() != "Linux":
    p.error("run the native dependency build on Linux")
if not a.archive.is_absolute() or not a.output.is_absolute():
    p.error("archive and output must be explicit absolute paths")
if a.prefix and not a.prefix.is_absolute():
    p.error("prefix must be an explicit absolute path")
if not 1 <= a.jobs <= 4:
    p.error("jobs must be between one and four")
root = a.output
prefix = a.prefix or root / "prefix"
if prefix.exists() and any(prefix.iterdir()):
    p.error("dependency prefix must be fresh or empty")
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
if sha(a.archive) != ARCHIVE_SHA256:
    p.error("archive must be the pinned upstream libXt-1.3.1.tar.xz")
patch = Path(__file__).resolve().parent.parent / "patches/libXt-incr-lifetime.patch"
root.mkdir(parents=True, exist_ok=False)
for name in ("tmp", "source", "build", "logs"):
    (root / name).mkdir()
env = dict(os.environ, TMPDIR=str(root / "tmp"), LC_ALL="C.UTF-8",
           CC=a.cc, CFLAGS="-O2 -g", LDFLAGS="")
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "archive_sha256": ARCHIVE_SHA256, "upstream": "libXt 1.3.1",
    "source_url": "https://www.x.org/releases/individual/lib/libXt-1.3.1.tar.xz",
    "patch_sha256": sha(patch), "prefix": str(prefix), "output": str(root),
    "compiler": a.cc, "jobs": a.jobs, "architecture": platform.machine(),
    "configuration": "shared; O2/g; native Xt; no static library or documentation build",
    "instrumentation": "none; toolkit and consumer sanitizer profiles are separate",
    "baseline": "First pinned corrected Xt profile; earlier different-version diagnostic builds are not comparable",
    "steps": [], "success": False}
start = time.monotonic()


def run(name, command, cwd):
    before = time.monotonic()
    with (root / "logs" / (name + ".log")).open("w") as out:
        result = subprocess.run([str(v) for v in command], cwd=cwd, env=env,
                                stdout=out, stderr=subprocess.STDOUT)
    receipt["steps"].append({"name": name, "command": [str(v) for v in command],
        "seconds": time.monotonic() - before, "exit": result.returncode})
    print(name, result.returncode, flush=True)
    if result.returncode:
        raise RuntimeError("see " + str(root / "logs" / (name + ".log")))


def extract_archive(archive, destination):
    """Extract a pinned source archive without relying on Python 3.12 APIs.

    ``TarFile.extractall(filter="data")`` is not available on Python 3.10,
    which is still common on supported Linux build hosts.  The dependency
    archive is expected to contain regular files and directories only.  We
    validate every member before extraction and reject links rather than
    weakening the path-safety contract on older Python versions.
    """
    destination = destination.resolve()
    with tarfile.open(archive) as source_archive:
        members = source_archive.getmembers()
        for member in members:
            relative = Path(member.name)
            if (not member.name or relative.is_absolute() or
                    ".." in relative.parts):
                raise RuntimeError("unsafe dependency archive path: " + member.name)
            if member.issym() or member.islnk():
                raise RuntimeError("dependency archive links are unsupported: " +
                                   member.name)
            target = (destination / relative).resolve()
            try:
                target.relative_to(destination)
            except ValueError as error:
                raise RuntimeError("dependency archive escapes destination: " +
                                   member.name) from error
        for member in members:
            source_archive.extract(member, destination)


try:
    extract_archive(a.archive, root / "source")
    source = root / "source/libXt-1.3.1"
    receipt["selection_before_sha256"] = sha(source / "src/Selection.c")
    run("patch", ["patch", "--batch", "--fuzz=0", "-p1", "-i", patch], source)
    receipt["selection_after_sha256"] = sha(source / "src/Selection.c")
    run("configure", [source / "configure", "--prefix=" + str(prefix),
        "--disable-static", "--disable-specs", "--without-xmlto", "--without-fop"], root / "build")
    receipt["compile_started_ns"] = time.time_ns()
    run("compile", ["make", "-j" + str(a.jobs)], root / "build")
    receipt["compile_ended_ns"] = time.time_ns()
    run("install", ["make", "install"], root / "build")
    receipt["libraries"] = {str(path): sha(path) for path in sorted((prefix / "lib").glob("libXt.so.*"))
                            if not path.is_symlink()}
    receipt["headers"] = {str(path.relative_to(prefix)): sha(path)
        for path in sorted((prefix / "include").rglob("*.h"))}
    if not receipt["libraries"] or not receipt["headers"]:
        raise RuntimeError("dependency installation is incomplete")
    receipt["success"] = True
finally:
    receipt.update(seconds=time.monotonic() - start,
                   ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
    (root / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
