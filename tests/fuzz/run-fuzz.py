#!/usr/bin/env python3
"""Build and run the libFuzzer targets of the CK Core Motif parsers.

run-fuzz.py --list
run-fuzz.py --target NAME --seconds N --scratch DIR [--worktree DIR]
run-fuzz.py --all --replay [--scratch DIR] [--worktree DIR]

Every target fuzzes production parser code that reads untrusted data: UID
files, the editres wire protocol, compound string external formats, and the
resource converters.  The production sources are only read, never changed.

Two ways of building are used, depending on the parser:

  - the UID file reader is the real lib/Mrm, compiled unchanged with coverage
    instrumentation and linked with the stand-ins of mrm-stubs.c for Xlib,
    the Intrinsics and the widget library;
  - the other parsers sit inside large library sources, so the runner cuts the
    needed functions out of the production text (with exact anchors, and a
    message that names the anchor if the production source changed), writes
    them into DIR/build/NAME/production.h, and compiles that header with the
    fuzz-NAME.c harness, which holds the small stand-ins.

The headers that the toolkit build generates (Xm/Xm.h and the XmStrDefs
headers) are produced below DIR/build/inc, the same way the toolkit build does
(config/util/makestrs).  No toolkit build tree is needed.

The target is built with the clang of the environment variable CLANG (default:
the Homebrew clang, /opt/homebrew/opt/llvm/bin/clang, on macOS; the first
`clang` in PATH elsewhere) and -fsanitize=fuzzer,address,undefined.  On macOS
the X11 headers and libraries come from XQuartz (/opt/X11; XINCLUDE and XLIB
override), elsewhere from the system (pkg-config).  Undefined behaviour stops the run like an
address error does (-fno-sanitize-recover=undefined).  The corpus starts from
tests/fuzz/corpus/NAME; the grown corpus, the logs and the crash inputs stay
below DIR.  The exit status is 1 when a crash, a timeout or an out-of-memory
input was found.  Only one libFuzzer process runs, on one CPU, and the build is
sequential.

--replay runs each target once over every file of its corpus directory
(libFuzzer -runs=0: no mutation, no corpus growth) and prints one result line
per target; the exit status is 1 when any input crashes or a target does not
build.  The corpora hold the regression input of every fixed defect, so a replay
is the fast check that no fixed defect has come back.  --all applies the action
(a fuzzing run of --seconds, or --replay) to every target.  All targets build on
macOS and on Linux with clang 19.
"""
import argparse
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent
IS_MACOS = sys.platform == "darwin"
MACOS_CLANG = "/opt/homebrew/opt/llvm/bin/clang"
MACOS_XINCLUDE = "/opt/X11/include"
MACOS_XLIB = "/opt/X11/lib"


def find_clang():
    """The compiler: $CLANG, else the Homebrew clang on macOS, else clang in PATH."""
    wanted = os.environ.get("CLANG") or (MACOS_CLANG if IS_MACOS else "clang")
    found = shutil.which(wanted)
    if not found:
        sys.exit(f"clang not found: {wanted} (set CLANG)")
    return found


def pkg_config(*args):
    """The words pkg-config prints, or None when it is missing or refuses."""
    try:
        r = subprocess.run(["pkg-config", *args], capture_output=True, text=True)
    except OSError:
        return None
    return shlex.split(r.stdout) if r.returncode == 0 else None


def x_include_flags():
    """Compiler flags for the X11 and Xt headers."""
    given = os.environ.get("XINCLUDE")
    if given:
        return ["-I", given]
    if IS_MACOS:
        return ["-I", MACOS_XINCLUDE]
    return pkg_config("--cflags", "x11", "xt") or []


def x_link_flags():
    """Linker flags for Xt and Xlib (XLIB names a directory that holds them)."""
    xlib = os.environ.get("XLIB") or (MACOS_XLIB if IS_MACOS else "")
    return (["-L" + xlib] if xlib else []) + ["-lXt", "-lX11"] + (["-Wl,-rpath," + xlib] if xlib else [])


def nm_undefined(obj):
    """The undefined symbols of OBJ, without the leading underscore of Mach-O names."""
    names = set()
    for line in sh(["nm", "-u", obj]).stdout.splitlines():
        if line.split():
            name = line.split()[-1]
            names.add(name[1:] if IS_MACOS else name)
    return names


# ---- extraction helpers --------------------------------------------------------

def function(text, name, source="source"):
    """Return the definition of the function NAME (not a prototype or a call):
    from its return type line to the closing brace in column 0.  The
    definition must be unique."""
    found = []
    for m in re.finditer(r"(?<![\w])" + re.escape(name) + r"[ \t]*\(", text):
        line_start = text.rfind("\n", 0, m.start()) + 1
        before = text[line_start:m.start()]
        if not re.fullmatch(r"[\w \t\*]*", before):
            continue                                   # a call or an expression
        if before.strip().startswith(("*", "/")):
            continue                                   # inside a comment
        if before.split() and before.split()[0] in ("return", "else", "case", "typedef"):
            continue
        brace = text.find("{", m.end())
        semi = text.find(";", m.end())
        if brace < 0 or (0 <= semi < brace):
            continue                                   # a prototype
        start = line_start
        if not before.strip():                         # the return type sits on the line above
            prev = text.rfind("\n", 0, start - 1) + 1
            above = text[prev:start - 1]
            if not (re.fullmatch(r"(?:static\s+)?[\w \t\*]+", above) and above.strip()):
                continue                               # a call that starts its line
            start = prev
        found.append(start)
    found = sorted(set(found))
    if len(found) != 1:
        sys.exit(f"{source}: definition of {name} found {len(found)} times, expected 1")
    start = found[0]
    end = text.index("\n}", start)
    end = text.find("\n", end + 1)
    return text[start:end if end >= 0 else len(text)] + "\n"


def functions(text, names, source):
    return "\n".join(function(text, n, source) for n in names)


def defines(text, names, source="source"):
    """Return the #define lines (with backslash continuations) of NAMES."""
    out = []
    for name in names:
        m = re.search(r"^[ \t]*#[ \t]*define[ \t]+" + re.escape(name) + r"\b(?:\([^)]*\))?(?:.*\\\n)*.*$", text, re.M)
        if not m:
            sys.exit(f"{source}: #define {name} not found")
        line = re.sub(r"/\*(?:(?!\*/).)*$", "", m.group(0))
        out.append(line.rstrip())
    return "\n".join(out) + "\n"


def between(text, start, end, source="source"):
    if text.count(start) < 1:
        sys.exit(f"{source}: anchor {start!r} not found")
    first = text.index(start)
    if end not in text[first:]:
        sys.exit(f"{source}: anchor {end!r} not found after {start!r}")
    return text[first:text.index(end, first)]


# ---- generated toolkit headers -------------------------------------------------

def sh(cmd, **kw):
    r = subprocess.run(cmd, capture_output=True, text=True, **kw)
    if r.returncode:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit("command failed: " + " ".join(shlex.quote(str(c)) for c in cmd[:3]) + " ...")
    return r


def prepare_includes(wt, scratch, clang):
    """Make DIR/build/inc: <Xm/...> and <Mrm/...> over the production headers,
    the generated Xm.h and XmStrDefs*.h, and an empty config.h.  Returns the
    path of the generated XmStrDefs.c."""
    build = scratch / "build"
    inc = build / "inc"
    gen = build / "gen"
    done = gen / "XmStrDefs.c"
    if done.exists() and (inc / "Xm" / "Xm.h").exists():
        return done
    for d in (inc / "Xm", inc / "Mrm", gen):
        d.mkdir(parents=True, exist_ok=True)
    for sub in ("Xm", "Mrm"):
        for h in sorted((wt / "lib" / sub).glob("*.h")):
            link = inc / sub / h.name
            if not link.exists() and not link.is_symlink():
                link.symlink_to(h)
    xm_h = inc / "Xm" / "Xm.h"
    if xm_h.is_symlink() or xm_h.exists():
        xm_h.unlink()
    shutil.copyfile(wt / "lib/Xm/Xm.h.in", xm_h)
    (inc / "config.h").write_text("/* generated by tests/fuzz/run-fuzz.py: nothing is configured */\n")
    for name in ("XmStrDefs.ht", "XmStrDefs22.ht", "XmStrDefs23.ht", "XmStrDefsI.ht", "XmStrDefs.ct"):
        shutil.copyfile(wt / "lib/Xm" / name, gen / name)
    # the default configuration: no compatibility with OpenMotif 2.2 names
    text = (wt / "lib/Xm/xmstring.list.in").read_text()
    text = re.sub(r"^@OM22_COMPATIBILITY_TRUE@", "!", text, flags=re.M)
    text = re.sub(r"^@OM22_COMPATIBILITY_FALSE@", "", text, flags=re.M)
    (gen / "xmstring.list").write_text(text)
    sh([clang, "-w", "-o", str(gen / "makestrs"), str(wt / "config/util/makestrs.c")])
    out = sh([str(gen / "makestrs"), "-f", "xmstring.list"], cwd=gen).stdout
    done.write_text(out)
    for h in ("XmStrDefs.h", "XmStrDefs22.h", "XmStrDefs23.h", "XmStrDefsI.h"):
        shutil.copyfile(gen / h, inc / "Xm" / h)
    return done


def cflags_common(scratch):
    return ["-fno-omit-frame-pointer", "-g", "-O1", "-std=gnu23", "-w", "-D_DEFAULT_SOURCE", "-D_DARWIN_C_SOURCE",
            "-DNO_MESSAGE_CATALOG", "-I", str(scratch / "build" / "inc"), *x_include_flags()]


def sanitize_flags(recover_ub=False):
    return ["-fsanitize=address,undefined",
            "-fsanitize-recover=undefined" if recover_ub else "-fno-sanitize-recover=undefined"]


# ---- targets -------------------------------------------------------------------

def compile_mrm_objects(wt, scratch, clang, recover_ub, fuzzer=True):
    """Compile the real lib/Mrm and the stand-ins below DIR/build/mrm-uid/obj.
    Without FUZZER the objects carry no coverage instrumentation (the tests of
    tests/automated link them with their own main).  Returns the object
    files, the compiler flags the compile and link need, and the linker flags
    for Xt and Xlib."""
    gendir = scratch / "build" / "mrm-uid"
    objdir = gendir / "obj"
    objdir.mkdir(parents=True, exist_ok=True)
    strdefs = prepare_includes(wt, scratch, clang)
    common = cflags_common(scratch)
    instr = sanitize_flags(recover_ub) + (["-fsanitize=fuzzer-no-link"] if fuzzer else [])
    objects = []

    def compile_one(src, obj, extra):
        cmd = [clang, "-c", *instr, *common, *extra, "-o", str(obj), str(src)]
        sh(cmd)
        objects.append(str(obj))

    for src in sorted((wt / "lib/Mrm").glob("*.c")):
        compile_one(src, objdir / (src.stem + ".o"), [])
    # the widget library entry points and class records that Mrm registers by name
    undefined = set()
    for obj in objects:
        undefined |= nm_undefined(obj)
    stub = ["/* generated by run-fuzz.py from the undefined symbols of lib/Mrm */",
            "#include <stddef.h>", "#include <X11/Intrinsic.h>", "#include <X11/IntrinsicP.h>",
            'Widget fuzz_new_widget(Widget);']
    for name in sorted(undefined):
        if re.fullmatch(r"XmCreate\w+", name):
            stub.append(f"Widget {name}(Widget p, String n, ArgList a, Cardinal c) "
                        "{ (void) n; (void) a; (void) c; return fuzz_new_widget(p); }")
        elif re.fullmatch(r"xm\w+ClassRec", name):
            # an opaque block, but with the class name that every real class record has:
            # the class lookup of Mrm compares names with the CoreClassPart
            class_name = "Xm" + name[2:-len("ClassRec")]
            stub.append(f'_Alignas(16) struct {{ CoreClassPart core_class; char rest[2048]; }} {name} = '
                        f'{{ {{ .class_name = "{class_name}" }} }};')
    (gendir / "widget-stubs.c").write_text("\n".join(stub) + "\n")
    compile_one(gendir / "widget-stubs.c", objdir / "widget-stubs.o", [])
    compile_one(HERE / "mrm-stubs.c", objdir / "mrm-stubs.o", ["-I", str(HERE)])
    compile_one(strdefs, objdir / "XmStrDefs.o", [])
    return objects, common, x_link_flags(), gendir


def build_mrm_uid(wt, scratch, clang, recover_ub):
    """The real lib/Mrm plus the stand-ins; returns the binary."""
    objects, common, xlink, gendir = compile_mrm_objects(wt, scratch, clang, recover_ub)
    binary = gendir / ("fuzz-mrm-uid-recover" if recover_ub else "fuzz-mrm-uid")
    cmd = [clang, "-fsanitize=fuzzer,address,undefined",
           "-fsanitize-recover=undefined" if recover_ub else "-fno-sanitize-recover=undefined",
           *common, "-I", str(HERE), "-o", str(binary), str(HERE / "fuzz-mrm-uid.c"), *objects,
           *xlink]
    sh(cmd)
    return binary


def make_mrm_seeds(wt, scratch, clang, outdir):
    """Link the seed writer against the same Mrm objects and write the seeds."""
    build_mrm_uid(wt, scratch, clang, False)
    gendir = scratch / "build" / "mrm-uid"
    objects = [str(p) for p in sorted((gendir / "obj").glob("*.o"))]
    binary = gendir / "mrm-make-seeds"
    sh([clang, *sanitize_flags(), *cflags_common(scratch), "-I", str(HERE), "-o", str(binary),
        str(HERE / "mrm-make-seeds.c"), *objects, *x_link_flags()])
    outdir.mkdir(parents=True, exist_ok=True)
    raw = scratch / "build" / "mrm-uid" / "seeds-raw"
    raw.mkdir(exist_ok=True)
    # the writer leaves padding of its buffers unwritten; zero it, so that the seeds are reproducible
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:malloc_fill_byte=0:max_malloc_fill_size=1048576")
    sh([str(binary), str(raw)], env=env)
    for uid in sorted(raw.glob("*.uid")):
        coded = rle_zeros(uid.read_bytes())
        (outdir / (uid.stem + ".uidz")).write_bytes(coded)
        print(f"{uid.name}: {uid.stat().st_size} bytes, {len(coded)} coded")


def rle_zeros(data):
    """The input encoding of fuzz-mrm-uid.c: a zero byte and a count C stand for C + 1 zero bytes."""
    out = bytearray()
    i = 0
    while i < len(data):
        if data[i] == 0:
            n = 1
            while i + n < len(data) and data[i + n] == 0 and n < 256:
                n += 1
            out += bytes([0, n - 1])
            i += n
        else:
            out.append(data[i])
            i += 1
    return bytes(out)


XM_CORE_SOURCES = ["XmString", "XmStringSeg", "XmStringObso", "XmRenderT", "XmTabList", "Direction", "Xmos", "ResEncod",
                   "ResInd"]


def compile_xm_core_objects(wt, scratch, clang, recover_ub, unity=False, fuzzer=True):
    """Compile the real compound string, render table, tab list and converter
    sources plus the stand-ins of xm-stubs.c below DIR/build/xm-core/obj.
    With UNITY the harness includes ResConvert.c itself.  Without FUZZER the
    objects carry no coverage instrumentation.  Returns the object files,
    the compiler flags the compile and link need, the linker flags for Xt and
    Xlib and the directory for generated files."""
    gendir = scratch / "build" / ("xm-unity" if unity else "xm-core")
    objdir = scratch / "build" / "xm-core" / "obj"
    objdir.mkdir(parents=True, exist_ok=True)
    gendir.mkdir(parents=True, exist_ok=True)
    strdefs = prepare_includes(wt, scratch, clang)
    # X11/Xlocale.h is not part of every X distribution; its content is <locale.h>
    shim = scratch / "build" / "shim" / "X11"
    shim.mkdir(parents=True, exist_ok=True)
    (shim / "Xlocale.h").write_text("#include <locale.h>\n")
    common = cflags_common(scratch) + ["-I", str(scratch / "build" / "shim"),
                                       "-I", str(scratch / "build" / "inc" / "Xm"), "-I", str(wt / "lib/Xm")]
    instr = sanitize_flags(recover_ub) + (["-fsanitize=fuzzer-no-link"] if fuzzer else [])
    objects = []

    def compile_one(src, obj, extra=()):
        if not obj.exists() or obj.stat().st_mtime < Path(src).stat().st_mtime:
            sh([clang, "-c", *instr, *common, *extra, "-o", str(obj), str(src)])
        objects.append(str(obj))

    # the converter harness includes ResConvert.c itself, to reach its static converters
    for stem in XM_CORE_SOURCES + ([] if unity else ["ResConvert"]):
        compile_one(wt / "lib/Xm" / f"{stem}.c", objdir / f"{stem}.o")
    compile_one(strdefs, objdir / "XmStrDefs.o")
    scan = list(objects)
    if unity:                  # the message strings that the included ResConvert.c names
        scan_obj = objdir / "ResConvert.scan.o"
        sh([clang, "-c", *common, "-o", str(scan_obj), str(wt / "lib/Xm/ResConvert.c")])
        scan.append(str(scan_obj))
    undefined = set()
    for obj in scan:
        undefined |= nm_undefined(obj)
    msgs = ["/* generated by run-fuzz.py: the message strings that the sources name */"]
    for name in sorted(undefined):
        if re.fullmatch(r"_XmMsg\w+", name):
            msgs.append(f"char {name}[] = \"{name}\";")
    (gendir / "messages.c").write_text("\n".join(msgs) + "\n")
    compile_one(gendir / "messages.c", objdir / ("messages-unity.o" if unity else "messages.o"))
    compile_one(HERE / "xm-stubs.c", objdir / "xm-stubs.o", ["-I", str(HERE)])
    compile_one(HERE / "xm-unreachable.c", objdir / "xm-unreachable.o")
    return objects, common, x_link_flags(), gendir


def build_xm_core(harness, unity=False):
    """The real compound string, render table, tab list and converter sources
    plus the stand-ins of xm-stubs.c, linked with the harness fuzz-NAME.c."""
    def build(wt, scratch, clang, recover_ub):
        objects, common, xlink, _ = compile_xm_core_objects(wt, scratch, clang, recover_ub, unity)
        gendir = scratch / "build" / harness
        gendir.mkdir(parents=True, exist_ok=True)
        binary = gendir / (f"fuzz-{harness}-recover" if recover_ub else f"fuzz-{harness}")
        cmd = [clang, "-fsanitize=fuzzer,address,undefined",
               "-fsanitize-recover=undefined" if recover_ub else "-fno-sanitize-recover=undefined",
               "-fsanitize=fuzzer", *common, "-I", str(HERE), "-I", str(wt / "lib/Xm"),
               "-o", str(binary), str(HERE / f"fuzz-{harness}.c"), *objects,
               *xlink]
        sh(cmd)
        return binary
    return build


def build_extracted(name, builder):
    def build(wt, scratch, clang, recover_ub):
        gen, extra = builder(wt)
        gendir = scratch / "build" / name
        gendir.mkdir(parents=True, exist_ok=True)
        prepare_includes(wt, scratch, clang)
        for fname, content in gen.items():
            (gendir / fname).write_text(content)
        binary = gendir / (f"fuzz-{name}-recover" if recover_ub else f"fuzz-{name}")
        cmd = [clang, "-fsanitize=fuzzer,address,undefined",
               "-fsanitize-recover=undefined" if recover_ub else "-fno-sanitize-recover=undefined",
               *cflags_common(scratch),
               f'-DFUZZ_TYPES_H="{gendir / "types.h"}"', f'-DFUZZ_PRODUCTION_H="{gendir / "production.h"}"',
               "-I", str(gendir), "-I", str(wt / "lib/Xm")]
        for d in extra.get("define", []):
            cmd += ["-D" + d]
        for inc in extra.get("include", []):
            cmd += ["-I", inc]
        cmd += [str(HERE / f"fuzz-{name}.c"), "-o", str(binary)]
        cmd += extra.get("link", [])
        sh(cmd)
        return binary
    return build


def extract_editres_wire(wt):
    src = "EditresCom.c"
    text = (wt / "lib/Xm" / src).read_text()
    types = between(text, "#define HEADER_SIZE", "#define EDITRES_MAX_REPLY", src)
    types += between(text, "#define EDITRES_MAX_CARD16", "typedef enum {\n    NormalResource", src)
    types += between(text, "/* The type of a request identifier. */", "#define streq(a,b)", src)
    types += "#include <X11/Xos.h>\n"
    prod = between(text, "#define WIRE_MIN_PATH_BYTES", "/************************************************************\n *\n * Writing a reply.", src) \
        if False else ""
    prod += functions(text, ["WireGet8", "WireGet16", "WireGetSigned16", "WireGet32", "WireGetBytes",
                             "WireGetString8"], src)
    prod += defines(text, ["WIRE_MIN_PATH_BYTES"], src)
    prod += functions(text, ["WireGetWidgetPath", "WireGetPathList"], src)
    prod += defines(text, ["ERROR_MESSAGE"], src)
    prod += functions(text, ["BuildEvent", "FreeEvent"], src)
    return {"types.h": types, "production.h": prod}, {}


TARGETS = {
    "xmstring-stream": ("XmString external format: XmCvtByteStreamToXmString with the ASN.1 length and component "
                        "readers, then walk, copy, concatenate and write back (lib/Xm/XmString.c, compiled unchanged)",
                        build_xm_core("xmstring-stream")),
    "xm-converters": ("resource converters that parse user strings: tab list, render table, font list, key symbol "
                      "and character set tables, string tables, lists, positions (lib/Xm/ResConvert.c)",
                      build_xm_core("xm-converters", unity=True)),
    "editres-wire": ("Editres wire protocol request parser: header, strings, widget paths, request decoders "
                     "(lib/Xm/EditresCom.c)", build_extracted("editres-wire", extract_editres_wire)),
    "mrm-uid": ("UID file reader of the resource manager: header, index tree, literal, colour, icon, widget "
                "records (lib/Mrm, compiled unchanged)", build_mrm_uid),
}


# ---- run -----------------------------------------------------------------------

def find_worktree(given):
    return Path(given).resolve() if given else HERE.parents[1]


def run(name, seconds, scratch, wt, clang, recover_ub=False):
    binary = TARGETS[name][1](wt, scratch, clang, recover_ub)
    seeds = HERE / "corpus" / name
    work = scratch / "corpus" / name
    crashes = scratch / "crashes" / name
    logs = scratch / "logs"
    for d in (work, crashes, logs):
        d.mkdir(parents=True, exist_ok=True)
    tmp = scratch / "tmp"
    tmp.mkdir(exist_ok=True)
    symbolizer = Path(clang).with_name("llvm-symbolizer")
    env = dict(os.environ, TMPDIR=str(tmp),
               **({"ASAN_SYMBOLIZER_PATH": str(symbolizer)} if symbolizer.exists() else {}),
               ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:symbolize=1:strict_memcmp=0",
               UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=" + ("0" if recover_ub else "1"))
    before = set(p.name for p in crashes.iterdir())
    cmd = [str(binary), "-max_len=4096", "-rss_limit_mb=2048", "-timeout=5", f"-max_total_time={seconds}",
           f"-artifact_prefix={crashes}/", "-print_final_stats=1", str(work), str(seeds)]
    print("+ " + " ".join(shlex.quote(c) for c in cmd), flush=True)
    log = logs / f"{name}.log"
    with open(log, "w") as out:
        proc = subprocess.run(cmd, env=env, stdout=out, stderr=subprocess.STDOUT, cwd=scratch)
    text = log.read_text(errors="replace")
    tail = [l for l in text.splitlines() if l.startswith("#") or l.startswith("stat::")]
    print("\n".join(tail[-14:]))
    new = sorted(p for p in crashes.iterdir() if p.name not in before and not p.name.startswith("min-"))
    for crash in new:
        minimized = crashes / ("min-" + crash.name)
        mcmd = [str(binary), "-minimize_crash=1", "-max_total_time=30", "-timeout=5", "-rss_limit_mb=2048",
                f"-exact_artifact_path={minimized}", str(crash)]
        # no symbolizer while minimizing: every run of a crashing input would wait for it
        menv = dict(env, ASAN_OPTIONS=env["ASAN_OPTIONS"].replace("symbolize=1", "symbolize=0"))
        with open(logs / f"{name}-minimize.log", "a") as mlog:
            subprocess.run(mcmd, env=menv, stdout=mlog, stderr=subprocess.STDOUT, cwd=scratch)
        print(f"crash input: {crash}" + (f" (minimized: {minimized})" if minimized.exists() else ""))
    seen = []
    if recover_ub:
        for l in text.splitlines():
            if "runtime error:" in l and l not in seen:
                seen.append(l)
        for l in seen:
            print("undefined behaviour: " + l)
    print(f"log: {log}")
    if new or proc.returncode or seen:
        print(f"{name}: FOUND {len(new)} crash input(s), libFuzzer exit status {proc.returncode}")
        return 1
    print(f"{name}: no crash in {seconds} s")
    return 0


def replay(name, scratch, wt, clang):
    """Run the target once over every file of its corpus directory, without
    mutation (libFuzzer -runs=0).  Prints one result line; returns 1 on a crash."""
    binary = TARGETS[name][1](wt, scratch, clang, False)
    seeds = HERE / "corpus" / name
    inputs = [p for p in seeds.rglob("*") if p.is_file()]
    if not inputs:
        print(f"{name}: replay FAILED: no corpus files in {seeds}")
        return 1
    crashes = scratch / "crashes" / name
    logs = scratch / "logs"
    tmp = scratch / "tmp"
    for d in (crashes, logs, tmp):
        d.mkdir(parents=True, exist_ok=True)
    symbolizer = Path(clang).with_name("llvm-symbolizer")
    env = dict(os.environ, TMPDIR=str(tmp),
               **({"ASAN_SYMBOLIZER_PATH": str(symbolizer)} if symbolizer.exists() else {}),
               ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:symbolize=1:strict_memcmp=0",
               UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1")
    # an input longer than -max_len would be cut, so the cap follows the largest file
    max_len = max(4096, max(p.stat().st_size for p in inputs))
    cmd = [str(binary), "-runs=0", f"-max_len={max_len}", "-rss_limit_mb=2048", "-timeout=5",
           f"-artifact_prefix={crashes}/", str(seeds)]
    print("+ " + " ".join(shlex.quote(c) for c in cmd), flush=True)
    log = logs / f"{name}-replay.log"
    with open(log, "w") as out:
        proc = subprocess.run(cmd, env=env, stdout=out, stderr=subprocess.STDOUT, cwd=scratch)
    if proc.returncode:
        text = log.read_text(errors="replace")
        sys.stdout.write("\n".join(text.splitlines()[-40:]) + "\n")
        print(f"{name}: replay FAILED: libFuzzer exit status {proc.returncode} over {len(inputs)} input(s), "
              f"log {log}")
        return 1
    print(f"{name}: replay ok, {len(inputs)} input(s)")
    return 0


def run_target(args, name, scratch, wt, clang):
    """Run one target for --all or --target; a failed build or extraction is a
    failure of that target only."""
    try:
        if args.replay:
            return replay(name, scratch, wt, clang)
        return run(name, args.seconds, scratch, wt, clang, args.recover_ub)
    except SystemExit as e:
        print(f"{name}: {'replay ' if args.replay else ''}FAILED: {e.code}")
        return 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--list", action="store_true", help="list the targets")
    ap.add_argument("--target", choices=sorted(TARGETS))
    ap.add_argument("--all", action="store_true", help="apply the action to every target")
    ap.add_argument("--replay", action="store_true",
                    help="run each target once over every file of its corpus directory (libFuzzer -runs=0, no "
                         "mutation) and exit 1 if any input crashes; --scratch defaults to a temporary directory")
    ap.add_argument("--seconds", type=int, default=60)
    ap.add_argument("--scratch", type=Path, help="directory for the build, corpus, logs and crashes")
    ap.add_argument("--recover-ub", action="store_true",
                    help="report undefined behaviour and go on; the first report of each location is listed")
    ap.add_argument("--make-seeds", type=Path, metavar="OUTDIR",
                    help="mrm-uid: write the seed UID files to OUTDIR with the resource manager's writer "
                         "(the committed seeds in corpus/mrm-uid were made this way)")
    ap.add_argument("--worktree", type=Path, default=None, help="checkout to read the production sources from")
    args = ap.parse_args()
    if args.list:
        for name, (description, _) in sorted(TARGETS.items()):
            print(f"{name:20} {description}")
        return 0
    if args.make_seeds:
        if not args.scratch:
            ap.error("--scratch is required")
        make_mrm_seeds(find_worktree(args.worktree), args.scratch.resolve(), find_clang(),
                       args.make_seeds.resolve())
        return 0
    if bool(args.target) == args.all:
        ap.error("exactly one of --target and --all is required")
    if not args.scratch and not args.replay:
        ap.error("--scratch is required")
    clang = find_clang()
    own_scratch = None
    if args.scratch:
        scratch = args.scratch.resolve()
        scratch.mkdir(parents=True, exist_ok=True)
    else:
        own_scratch = scratch = Path(tempfile.mkdtemp(prefix="ck-fuzz-replay-")).resolve()
    wt = find_worktree(args.worktree)
    try:
        status = 0
        for name in ([args.target] if args.target else sorted(TARGETS)):
            status |= run_target(args, name, scratch, wt, clang)
        return status
    finally:
        if own_scratch:
            shutil.rmtree(own_scratch, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
