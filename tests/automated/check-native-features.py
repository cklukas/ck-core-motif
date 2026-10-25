#!/usr/bin/env python3
"""Compile and run native Linux feature fixtures against one installed prefix.

Requires an ABI 8 Unicode/Xft/AT-SPI build for the complete suite, a native C
compiler, pkg-config development modules, Xvfb/xauth and real AT-SPI services.
No toolkit build, package installation or documentation rendering is performed.
"""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import signal
import struct
import subprocess
import sys
import time

FIXTURES = {
    "abi": ("tests/Toolkit/ABI/cohort.c", False),
    "xmstring-empty-concat": ("tests/XmString/empty_concat.c", False),
    "xmstring-empty-components": ("tests/XmString/empty_components.c", False),
    "incremental-map": ("tests/Toolkit/Unicode/incremental_map.c", False),
    "incremental-widgets": ("tests/Toolkit/Unicode/incremental_widgets.c", True),
    "incremental-layout": ("tests/Toolkit/Unicode/incremental_layout.c", True),
    "preedit": ("tests/Toolkit/Im/preedit_ranges.c", False),
    "preedit-state": ("tests/Toolkit/Im/preedit_state.c", True),
    "local-compose": ("tests/Toolkit/Im/local_compose.c", False),
    "primary-modality": ("tests/Toolkit/Message/primary_modality.c", True),
    "accessibility-modality": ("tests/accessibility/modality.c", True),
    "layout": ("tests/Toolkit/Unicode/layout.c", True),
    "line-context": ("tests/Toolkit/Unicode/line_context.c", True),
    "unicode-widgets": ("tests/Toolkit/Unicode/text_unicode.c", True),
    "font-resize": ("tests/Toolkit/Unicode/font_resize.c", True),
    "font-lifetime": ("tests/Toolkit/Unicode/font_lifetime.c", True),
    "default-font-cache": ("tests/Toolkit/Unicode/default_font_cache.c", True),
    "label-gc": ("tests/Toolkit/Unicode/label_gc.c", True),
    "color-display-lifetime": ("tests/Toolkit/Unicode/color_display_lifetime.c", True),
    "large-text": ("tests/Toolkit/Unicode/large_text.c", True),
    "clipboard": ("tests/Toolkit/CutPaste/clipboard_unicode.c", True),
    "clipboard-lifetime": ("tests/Toolkit/CutPaste/clipboard_unicode.c", True),
    "disclosure": ("tests/disclosure/widget.c", True),
    "accessibility": ("tests/accessibility/provider.c", True),
    "accessibility-clipping": ("tests/accessibility/clipping.c", True),
    "accessibility-coordinates": ("tests/accessibility/coordinates.c", True),
    "accessibility-clipping-events": ("tests/accessibility/clipping_events.c", True),
    "scrollbar-accessibility": ("tests/accessibility/scrollbar.c", True),
    "text-events": ("tests/accessibility/text_events.c", True),
    "key-events": ("tests/accessibility/key_events.c", True),
    "list-lifetime": ("tests/accessibility/list_lifetime.c", True),
    "selection-reentrancy": ("tests/accessibility/selection_reentrancy.c", True),
    "list-accessibility": ("tests/accessibility/list.c", True),
    "list-allocation-faults": ("tests/accessibility/list.c", True),
    "menu-accessibility": ("tests/accessibility/menu.c", True),
    "compound-traversal": ("tests/accessibility/compound_traversal.c", True),
    "xim-service": ("tests/Toolkit/Im/service.c", True),
}
MODULES = ("xft", "fontconfig", "harfbuzz", "icu-uc", "icu-i18n")


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def installed_headers(prefix):
    headers = {str(path.relative_to(prefix)): sha(path)
               for path in sorted((prefix / "include/Xm").rglob("*.h"))}
    if "include/Xm/Xm.h" not in headers:
        raise RuntimeError("selected prefix has no installed Xm/Xm.h")
    for name in headers:
        if not (prefix / name).resolve().is_relative_to(prefix):
            raise RuntimeError("installed header escapes selected prefix: " + name)
    return headers


def installed_library(prefix, library_dir):
    path = (library_dir / "libXm.so.8").resolve(strict=True)
    if not path.is_relative_to(prefix):
        raise RuntimeError("installed libXm.so.8 escapes selected prefix")
    linker = (library_dir / "libXm.so").resolve(strict=True)
    if linker != path:
        raise RuntimeError("linker libXm.so and runtime libXm.so.8 do not identify the same installed file")
    return dict(path=str(path), sha256=sha(path), linker_name=str(library_dir / "libXm.so"))


def verify_headers(dependency_file, prefix):
    # -MD captures the actual include graph, including system headers. Make's
    # backslash-newline continuation and escaped filename spaces are decoded.
    text = dependency_file.read_text().replace("\\\n", " ")
    _, separator, dependencies = text.partition(":")
    if not separator:
        raise RuntimeError("compiler did not produce a dependency graph")
    observed = {}
    for name in shlex.split(dependencies):
        path = Path(name)
        if "Xm" not in path.parts or path.suffix != ".h":
            continue
        path = path.resolve(strict=True)
        if not path.is_relative_to(prefix / "include/Xm"):
            raise RuntimeError("fixture included a foreign Xm header: " + str(path))
        observed[str(path.relative_to(prefix))] = sha(path)
    if not observed:
        raise RuntimeError("fixture has no proven installed Xm header dependencies")
    return observed


def verify_native_elf(binary):
    machines = {"aarch64": 183, "arm64": 183, "x86_64": 62, "amd64": 62,
                "i386": 3, "i686": 3, "armv7l": 40, "armv6l": 40,
                "riscv64": 243, "ppc64": 21, "ppc64le": 21, "s390x": 22,
                "loongarch64": 258}
    header = binary.read_bytes()[:20]
    if len(header) < 20 or header[:4] != b"\x7fELF" or header[5] not in (1, 2):
        raise RuntimeError("fixture is not an ELF executable")
    bits = {1: 32, 2: 64}.get(header[4])
    machine = struct.unpack("<H" if header[5] == 1 else ">H", header[18:20])[0]
    expected = machines.get(platform.machine().lower())
    if expected is None or machine != expected or bits != struct.calcsize("P") * 8:
        raise RuntimeError("fixture architecture does not match the native host/Python ABI")
    return dict(machine=machine, bits=bits)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path, help="absolute installed toolkit prefix")
    parser.add_argument("--out", required=True, type=Path, help="fresh absolute result/scratch directory")
    parser.add_argument("--libdir", type=Path, help="absolute library directory inside prefix (default: prefix/lib)")
    parser.add_argument("--runtime-library-dir", action="append", type=Path,
                        help="additional absolute dependency library directory; may be repeated")
    parser.add_argument("--fontconfig-file", type=Path,
                        help="absolute fontconfig file to use and fingerprint for text fixtures")
    parser.add_argument("--cc", default="cc", help="native C compiler executable")
    parser.add_argument("--sanitize", action="store_true",
                        help="instrument fixtures for an ASan/UBSan toolkit prefix; strict leak/error checks")
    parser.add_argument("--accessibility-runtime", type=Path,
                        help="fresh short absolute private runtime for clipping event fixture")
    parser.add_argument("--only", action="append", choices=FIXTURES,
                        help="restrict feature fixtures; the installed ABI check always runs")
    parser.add_argument("--timeout", type=int, default=90, help="seconds per command (default: 90)")
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("this runner validates native Linux installations only")
    if not args.prefix.is_absolute() or not args.out.is_absolute():
        parser.error("--prefix and --out must be explicit absolute paths")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    library_dir = (args.libdir or prefix / "lib").resolve(strict=True)
    if not library_dir.is_relative_to(prefix) or (args.libdir and not args.libdir.is_absolute()):
        parser.error("--libdir must be an absolute directory inside the selected prefix")
    runtime_library_dirs = []
    for path in args.runtime_library_dir or []:
        if not path.is_absolute():
            parser.error("--runtime-library-dir must be absolute")
        path = path.resolve(strict=True)
        if ":" in str(path):
            parser.error("runtime library directories must have no colon")
        runtime_library_dirs.append(path)
    fontconfig_file = None
    if args.fontconfig_file:
        if not args.fontconfig_file.is_absolute():
            parser.error("--fontconfig-file must be absolute")
        fontconfig_file = args.fontconfig_file.resolve(strict=True)
    if ":" in str(library_dir) or args.timeout < 1:
        parser.error("library directory must have no colon; timeout must be positive")
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be fresh and separate from the installed prefix")
    source = Path(__file__).resolve().parents[2]
    selected = [name for name in FIXTURES if name == "abi" or
                (name in args.only if args.only else name != "xim-service")]
    output.mkdir(parents=True)
    (output / "tmp").mkdir()
    runtime_search_path = os.pathsep.join(
        [str(library_dir), *(str(path) for path in runtime_library_dirs)])
    env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
               LD_LIBRARY_PATH=runtime_search_path, PYTHONDONTWRITEBYTECODE="1")
    if fontconfig_file:
        env["FONTCONFIG_FILE"] = str(fontconfig_file)
    instrumentation_flags = []
    if args.sanitize:
        instrumentation_flags = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        env.update(ASAN_OPTIONS="detect_leaks=1:abort_on_error=1",
                   UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
        env.pop("LSAN_OPTIONS", None)
    for name in ("LD_PRELOAD", "LD_AUDIT", "DISPLAY"):
        env.pop(name, None)
    receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   prefix=str(prefix), library_dir=str(library_dir),
                   runtime_library_dirs=[str(path) for path in runtime_library_dirs],
                   fontconfig_file=(dict(path=str(fontconfig_file), sha256=sha(fontconfig_file))
                                    if fontconfig_file else None),
                   output=str(output),
                   fixture_source=str(source), requested_fixtures=selected,
                   fixture_instrumentation=instrumentation_flags,
                   runtime_options={name: env[name] for name in ("ASAN_OPTIONS", "UBSAN_OPTIONS") if name in env},
                   configuration="native installed ABI8 consumers; strict C11 O2; actual Xvfb and AT-SPI observers",
                   baseline="No duration baseline assumed; compare retained equivalent suite results.",
                   limitation="Fingerprints identify tested inputs; this runner does not attest which source built the installed prefix.",
                   host=dict(system=platform.system(), machine=platform.machine(), release=platform.release()),
                   tools={}, sources={}, binaries={}, steps=[], success=False)
    began = time.monotonic()

    def run(name, command):
        command = [str(value) for value in command]
        started = time.monotonic()
        started_ns = time.time_ns()
        log_path = output / (name + ".log")
        timed_out = False
        def terminate_group(process):
            try:
                os.killpg(process.pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                process.wait()

        with log_path.open("w") as log:
            process = subprocess.Popen(command, cwd=output, env=env, stdout=log,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            try:
                code = process.wait(timeout=args.timeout)
            except subprocess.TimeoutExpired:
                timed_out = True
                terminate_group(process)
                code = process.returncode
            except BaseException:
                terminate_group(process)
                raise
        receipt["steps"].append(dict(name=name, command=command, exit=code,
            started_ns=started_ns, ended_ns=time.time_ns(), seconds=time.monotonic() - started,
            timed_out=timed_out, log=str(log_path)))
        print(name, "timeout" if timed_out else code, flush=True)
        if code or timed_out:
            print("\n".join(log_path.read_text(errors="replace").splitlines()[-40:]), file=sys.stderr)
            raise RuntimeError("failed " + name + "; see " + str(log_path))
        return log_path.read_text()

    def remember(path):
        receipt["sources"][str(path.relative_to(source))] = sha(path)

    def display(command):
        return ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", "--", *command]

    try:
        required = [args.cc, "pkg-config", "ldd", "readelf"]
        if any(FIXTURES[name][1] for name in selected):
            required += ["xvfb-run", "Xvfb", "xauth"]
        if any(name in selected for name in ("disclosure", "accessibility", "scrollbar-accessibility", "text-events", "key-events", "accessibility-clipping-events", "accessibility-coordinates")):
            required += ["dbus-run-session", "gdbus", "stdbuf"]
        if "accessibility-coordinates" in selected:
            required += ["scrot"]
        if "scrollbar-accessibility" in selected:
            required += ["xdotool"]
        if "xim-service" in selected:
            required += ["xdotool", "ibus", "ibus-daemon", "dbus-run-session", "xprop", "dpkg-query"]
        if "local-compose" in selected:
            required += ["Xvfb", "openbox", "xdotool", "xdpyinfo", "xprop"]
        for executable in required:
            located = shutil.which(executable)
            if not located:
                raise RuntimeError("missing prerequisite executable: " + executable)
            path = Path(located).resolve(strict=True)
            receipt["tools"][executable] = dict(path=str(path), sha256=sha(path))
        receipt["tools"]["python"] = dict(path=sys.executable, version=sys.version)
        run("compiler-version", [args.cc, "--version"])
        run("compiler-target", [args.cc, "-dumpmachine"])
        run("loader-version", ["ldd", "--version"])
        receipt["headers"] = installed_headers(prefix)
        receipt["library"] = installed_library(prefix, library_dir)
        dynamic = run("installed-library-dynamic", ["readelf", "-d", receipt["library"]["path"]])
        if not re.search(r"\(SONAME\).*\[libXm\.so\.8\]", dynamic):
            raise RuntimeError("installed library SONAME is not libXm.so.8")
        remember(Path(__file__).resolve())
        flags = shlex.split(run("pkg-flags", ["pkg-config", "--cflags", "--libs", *MODULES]))
        run("pkg-versions", ["pkg-config", "--modversion", *MODULES])
        for name in selected:
            relative, needs_display = FIXTURES[name]
            fixture = source / relative
            remember(fixture)
            binary = output / name
            dependencies = output / (name + ".d")
            extra_flags = []
            runtime_link_flags = ["-L" + str(path) for path in runtime_library_dirs]
            if name == "color-display-lifetime":
                extra_flags = ["-ldl"]
            if name == "primary-modality":
                extra_flags = shlex.split(run("xtst-flags", ["pkg-config", "--cflags", "--libs", "xtst"]))
                run("xtst-version", ["pkg-config", "--modversion", "xtst"])
            before = time.time_ns()
            run("compile-" + name, [args.cc, "-std=c11", "-O2", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-D_POSIX_C_SOURCE=200809L", "-I" + str(prefix / "include"), "-MD", "-MF", dependencies,
                *instrumentation_flags, fixture, "-L" + str(library_dir), "-Wl,-rpath," + str(library_dir),
                *runtime_link_flags, "-lXm", "-lXt", "-lX11", *flags, *extra_flags,
                "-lm", "-o", binary])
            if not before <= binary.stat().st_mtime_ns <= time.time_ns():
                raise RuntimeError("compiled fixture timestamp does not prove freshness")
            header_inputs = verify_headers(dependencies, prefix)
            for header, fingerprint in header_inputs.items():
                if receipt["headers"].get(header) != fingerprint:
                    raise RuntimeError("installed header changed or was not recorded: " + header)
            elf = verify_native_elf(binary)
            dynamic = run("dynamic-" + name, ["readelf", "-d", binary])
            if not re.search(r"\(NEEDED\).*\[libXm\.so\.8\]", dynamic):
                raise RuntimeError("fixture does not depend on ABI8 libXm")
            links = run("link-" + name, ["ldd", binary])
            match = re.search(r"^\s*libXm\.so\.8\s+=>\s+(.+?)\s+\(0x[0-9a-f]+\)", links, re.MULTILINE)
            foreign_abi = re.search(r"^\s*libXm\.so\.(?!8\s)[^\s]+", links, re.MULTILINE)
            if "not found" in links or foreign_abi or not match or Path(match.group(1)).resolve() != Path(receipt["library"]["path"]):
                raise RuntimeError("fixture loader did not select the exact installed libXm")
            receipt["binaries"][name] = dict(path=str(binary), sha256=sha(binary),
                mtime_ns=binary.stat().st_mtime_ns, elf=elf, installed_headers=header_inputs)
            if name in ("primary-modality", "accessibility-modality"):
                run(name, display([binary, "--isolated-display"]))
            elif name in ("clipboard", "clipboard-lifetime"):
                observer = source / ("tests/Toolkit/CutPaste/clipboard_unicode.py" if name == "clipboard"
                                     else "tests/Toolkit/CutPaste/clipboard_lifetime.py")
                remember(observer)
                run(name, display([sys.executable, observer, binary,
                                   output / (name + "-observer")]))
            elif name == "local-compose":
                observer = source / "tests/Toolkit/Im/local_compose.py"
                remember(observer)
                remember(source / "tests/Toolkit/Im/local_compose.Compose")
                observer_command = [sys.executable, observer, binary,
                                    output / "local-compose-observer", "--prefix", prefix]
                for path in runtime_library_dirs:
                    observer_command += ["--runtime-library-dir", path]
                run(name, observer_command)
            elif name == "xim-service":
                observer = source / "tests/Toolkit/Im/service.py"
                remember(observer)
                run(name, display(["dbus-run-session", "--", sys.executable, observer,
                                   binary, output / "xim-service-observer"]))
            elif name == "list-allocation-faults":
                observer = source / "tests/accessibility/observe_list.py"
                remember(observer)
                fault_source = source / "tests/accessibility/list_faults.c"
                remember(fault_source)
                interposer = output / "list-faults.so"
                run("compile-list-faults", [args.cc, "-std=c11", "-O2", "-Wall", "-Wextra",
                    "-Wpedantic", "-Werror", "-shared", "-fPIC", fault_source, "-ldl", "-o", interposer])
                receipt["binaries"]["list-faults"] = dict(path=str(interposer), sha256=sha(interposer),
                    mtime_ns=interposer.stat().st_mtime_ns, elf=verify_native_elf(interposer))
                run("list-faults", display(["dbus-run-session", "--", sys.executable, observer,
                    binary, output / "list-faults-observer.json", interposer]))
            elif name in ("accessibility-clipping-events", "accessibility-coordinates"):
                stem = "coordinates" if name == "accessibility-coordinates" else "clipping_events"
                observer = source / ("tests/accessibility/observe_" + stem + ".py")
                remember(observer)
                runtime = args.accessibility_runtime or output / "clipping-events-runtime"
                if name == "accessibility-coordinates":
                    runtime = Path(str(args.accessibility_runtime) + "-coords") if args.accessibility_runtime else output / "coordinates-runtime"
                if not runtime.is_absolute() or runtime.exists() or len(os.fsencode(runtime / "at-spi/bus")) >= 100:
                    raise RuntimeError("supply --accessibility-runtime with a fresh short absolute path for the private Unix socket")
                runtime.mkdir(mode=0o700)
                run(name, display(["env", "-u", "AT_SPI_BUS_ADDRESS",
                    "XDG_RUNTIME_DIR=" + str(runtime), "dbus-run-session", "--",
                    sys.executable, observer, binary, output / ("coordinates-observer.json" if name == "accessibility-coordinates" else "clipping-events-observer.json")]))
            elif name in ("accessibility", "scrollbar-accessibility", "text-events", "key-events", "list-accessibility"):
                observer = source / {"accessibility": "tests/accessibility/observe.py",
                                     "scrollbar-accessibility": "tests/accessibility/observe_scrollbar.py",
                                     "text-events": "tests/accessibility/observe_text_events.py",
                                     "key-events": "tests/accessibility/observe_key_events.py",
                                     "list-accessibility": "tests/accessibility/observe_list.py"}[name]
                remember(observer)
                run(name, display(["dbus-run-session", "--", sys.executable, observer, binary,
                                   output / (name + "-observer.json")]))
            else:
                run(name, display([binary]) if needs_display else [binary])
            if name == "text-events":
                fault_source = source / "tests/accessibility/text_event_faults.c"
                fault_observer = source / "tests/accessibility/observe_text_faults.py"
                remember(fault_source)
                remember(fault_observer)
                interposer = output / "text-event-faults.so"
                dbus_flags = shlex.split(run("dbus-fault-flags", ["pkg-config", "--cflags", "--libs", "dbus-1"]))
                run("compile-text-event-faults", [args.cc, "-std=c11", "-O2", "-Wall", "-Wextra",
                    "-Wpedantic", "-Werror", "-shared", "-fPIC", fault_source, *dbus_flags,
                    "-ldl", "-o", interposer])
                receipt["binaries"]["text-event-faults"] = dict(path=str(interposer), sha256=sha(interposer),
                    mtime_ns=interposer.stat().st_mtime_ns, elf=verify_native_elf(interposer))
                run("text-event-faults", display(["dbus-run-session", "--", sys.executable, fault_observer,
                    binary, interposer, output / "text-event-faults-observer.json"]))
            if name == "disclosure":
                observer = source / "tests/disclosure/observe.py"
                remember(observer)
                run("disclosure-protocol", display(["dbus-run-session", "--", sys.executable,
                    observer, binary, output / "disclosure-observer.json"]))
        if receipt["headers"] != installed_headers(prefix) or receipt["library"] != installed_library(prefix, library_dir):
            raise RuntimeError("installed toolkit changed during validation")
        for relative, fingerprint in receipt["sources"].items():
            if sha(source / relative) != fingerprint:
                raise RuntimeError("fixture source changed during validation: " + relative)
        receipt["success"] = True
    finally:
        receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                       seconds=time.monotonic() - began)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print("Native installed toolkit fixtures passed; evidence:", output / "receipt.json")


if __name__ == "__main__":
    main()
