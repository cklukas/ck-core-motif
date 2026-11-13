"""Build and run a check program against the production sources of lib/Xm and
lib/Mrm, compiled unchanged under AddressSanitizer and UndefinedBehaviorSanitizer.

The runners run-xmstring-stream-bounds.py, run-resconvert-list-bounds.py and
run-mrm-literal-bounds.py share this module.  It reuses the way tests/fuzz/run-fuzz.py
builds the library sources (the headers that the toolkit build generates come from
config/util/makestrs, and the stand-ins of tests/fuzz replace Xlib, the Intrinsics and
the widget library), so no toolkit build tree and no X server are needed.

A runner takes a fresh absolute RESULTS directory and an optional --source-revision REV.
Without a revision the working tree is tested and every case must pass.  With a revision
the library sources are read from that Git revision (git archive; nothing in the
repository is changed) and the runner is a negative control: every case that is marked as
a defect must fail on that revision, every other case must still pass, and the runner
exits 0 only then.  Each case runs in its own process: a case is a name that the check
program receives as its first argument.
"""
import argparse
import importlib.util
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
FUZZ = ROOT / "tests" / "fuzz"
DEFAULT_CLANG = "/opt/homebrew/opt/llvm/bin/clang"


def _load_fuzz_runner():
    spec = importlib.util.spec_from_file_location("ck_run_fuzz", FUZZ / "run-fuzz.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class Session:
    """The results directory, the source tree under test and the compiler."""

    def __init__(self, description, argv=None):
        parser = argparse.ArgumentParser(description=description)
        parser.add_argument("results", type=Path, help="fresh absolute results directory")
        parser.add_argument("--source-revision", metavar="REV",
                            help="negative control: test the library sources of this Git revision "
                                 "and require the defect cases to fail")
        args = parser.parse_args(argv)
        if not args.results.is_absolute():
            parser.error("the results directory must be absolute")
        self.revision = args.source_revision
        self.results = args.results
        self.results.mkdir(parents=True, exist_ok=False)
        (self.results / "tmp").mkdir()
        self.clang = os.environ.get("CLANG", DEFAULT_CLANG)
        if not Path(self.clang).exists():
            raise SystemExit(f"clang not found: {self.clang} (set CLANG)")
        self.fuzz = _load_fuzz_runner()
        self.tree = self._source_tree()
        self.env = dict(os.environ, TMPDIR=str(self.results / "tmp"), LC_ALL="C",
                        ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=1:strict_memcmp=0",
                        UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
        symbolizer = Path(self.clang).with_name("llvm-symbolizer")
        if symbolizer.exists():
            self.env["ASAN_SYMBOLIZER_PATH"] = str(symbolizer)

    def _source_tree(self):
        if not self.revision:
            return ROOT
        tree = self.results / "source"
        tree.mkdir()
        archive = subprocess.Popen(
            ["git", "-C", str(ROOT), "archive", self.revision, "lib/Xm", "lib/Mrm", "config/util/makestrs.c"],
            stdout=subprocess.PIPE)
        extract = subprocess.run(["tar", "-x", "-C", str(tree)], stdin=archive.stdout)
        archive.stdout.close()
        if archive.wait() or extract.returncode:
            raise SystemExit(f"cannot read the library sources of revision {self.revision}")
        return tree

    def _link(self, check, objects, flags, name, extra=()):
        binary = self.results / name
        command = [self.clang, "-fsanitize=address,undefined", "-fno-sanitize-recover=undefined",
                   *flags, *extra, "-I", str(FUZZ), "-o", str(binary), str(check), *objects]
        result = subprocess.run(command, capture_output=True, text=True)
        (self.results / (name + "-compile.log")).write_text(result.stdout + result.stderr)
        if result.returncode:
            sys.stdout.write(result.stdout + result.stderr)
            raise SystemExit("compiling the check program failed")
        return binary

    def build_xm(self, check, unity=False):
        """Link CHECK with lib/Xm compound string, render table, tab list and converter
        sources.  With UNITY the check includes ResConvert.c itself."""
        fuzz = self.fuzz
        objects, common, xlib, _ = fuzz.compile_xm_core_objects(
            self.tree, self.results, self.clang, False, unity=unity, fuzzer=False)
        return self._link(check, objects,
                          [*common, "-I", str(self.tree / "lib/Xm"), "-L" + xlib, "-lXt", "-lX11",
                           "-Wl,-rpath," + xlib], "check-xm")

    def build_mrm(self, check):
        """Link CHECK with all of lib/Mrm and the stand-ins of tests/fuzz/mrm-stubs.c."""
        fuzz = self.fuzz
        objects, common, xlib, _ = fuzz.compile_mrm_objects(
            self.tree, self.results, self.clang, False, fuzzer=False)
        return self._link(check, objects,
                          [*common, "-L" + xlib, "-lXt", "-lX11", "-Wl,-rpath," + xlib], "check-mrm")

    def run(self, binary, cases):
        """Run every case in its own process.  CASES is a list of (name, defect); returns
        a list of (name, defect, exit status, output)."""
        outcomes = []
        for name, defect in cases:
            result = subprocess.run([str(binary), name, str(self.results / "tmp")], env=self.env,
                                    capture_output=True, text=True, timeout=120)
            output = result.stdout + result.stderr
            (self.results / f"case-{name}.log").write_text(output)
            outcomes.append((name, defect, result.returncode, output))
        return outcomes

    def finish(self, outcomes, what):
        """Print the verdict and return the exit status of the runner."""
        if not self.revision:
            failed = [o for o in outcomes if o[2]]
            for name, defect, status, output in outcomes:
                print(f"{'FAIL' if status else 'PASS'} {name}")
                if status:
                    print(output)
            print(f"{what}: {len(outcomes) - len(failed)} of {len(outcomes)} cases pass on the working tree")
            return 1 if failed else 0
        wrong = []
        for name, defect, status, output in outcomes:
            expected_failure = defect
            print(f"{'fails' if status else 'passes'} {name} on {self.revision}"
                  f"{' (defect case)' if defect else ''}")
            if bool(status) != expected_failure:
                wrong.append(name)
        if wrong:
            print(f"negative control ({self.revision}): UNEXPECTED result for {', '.join(wrong)}")
            return 1
        print(f"negative control ({self.revision}): fails as expected")
        return 0
