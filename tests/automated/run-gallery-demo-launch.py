#!/usr/bin/env python3
"""Exercise actual resolution, exec, asset cwd, errors and child reaping without X."""
import ctypes
import errno
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix="gallery-demo-launch-") as temporary:
    work = Path(temporary)
    source = root / "demos/programs/ckmotif-gallery"
    library = work / "launch.so"
    subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
                    "-shared", "-fPIC", str(source / "demo_launch.c"), "-o", str(library)], check=True)
    api = ctypes.CDLL(str(library))
    api.gallery_demo_path.argtypes = [ctypes.c_char_p] * 6 + [ctypes.c_size_t]
    api.gallery_demo_launch.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    build, install = work / "build", work / "install"
    for directory in (build / "demo", install / "Demo"):
        directory.mkdir(parents=True)
    name = "example ; touch INJECTED"
    buffer = ctypes.create_string_buffer(4096)
    def resolve(size=4096):
        return api.gallery_demo_path(os.fsencode(build), os.fsencode(install), b"demo", b"Demo",
                                     os.fsencode(name), buffer, size)
    assert resolve() == errno.ENOENT
    assert buffer.value.decode() == str(install / "Demo" / name)
    assert resolve(4) == errno.ENAMETOOLONG
    executable = install / "Demo" / name
    executable.write_text("#!/bin/sh\nprintf '%s\\n' \"$PWD\" \"$DISPLAY\" \"$UIDPATH\" > result\n")
    assert resolve() == errno.ENOENT  # a non-executable file is unavailable
    executable.chmod(0o755)
    assert resolve() == 0 and buffer.value.decode() == str(executable)
    os.environ["DISPLAY"] = ":private-test-display"
    os.environ["UIDPATH"] = "/custom/uid/%U"
    pid = ctypes.c_int()
    assert api.gallery_demo_launch(buffer.value, ctypes.byref(pid)) == 0
    assert os.waitpid(pid.value, 0)[1] == 0
    assert os.environ["UIDPATH"] == "/custom/uid/%U"
    reported = (executable.parent / "result").read_text().splitlines()
    assert reported[:2] == [str(executable.parent), ":private-test-display"]
    assert reported[2] == "/custom/uid/%U:" + ":".join(str(executable.parent) + suffix for suffix in
        ("/%U", "/%L/uid/%U", "/%l/uid/%U", "/C/uid/%U"))
    assert not list(work.rglob("INJECTED"))
    candidate = build / "demo" / name
    candidate.write_bytes(b"invalid executable")
    candidate.chmod(0o755)
    assert resolve() == 0 and buffer.value.decode() == str(candidate)
    assert api.gallery_demo_launch(buffer.value, ctypes.byref(pid)) == errno.ENOEXEC
    candidate.unlink()
    executable.unlink()
    assert api.gallery_demo_launch(os.fsencode(executable), ctypes.byref(pid)) == errno.ENOENT
    try:
        os.waitpid(-1, os.WNOHANG)
        raise AssertionError("failed exec left an unreaped child")
    except ChildProcessError:
        pass
print("PASS demo resolution, installation fallback, literal exec, asset cwd, display, exec errors and reaping")
