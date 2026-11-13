# Continuous integration

The workflow `.github/workflows/build.yml` runs on every push to `master`, on
pull requests and on request (`workflow_dispatch`). Pushes that change only the
documentation inputs do not start it; the [documentation
workflow](build.md#hosted-documentation-workflow) handles those. The rpi4
(`ckgit`) pipeline in `.ckgit/ci.yml` runs the fuzz corpus replay too.

## Build and test

The `build-test` job runs once with GCC 14 and once with Clang 19, on an ARM64
Ubuntu 24.04 runner. Each run

1. configures an out-of-tree build with `--enable-hardening` at `-O2` and
   `-std=c23` (the [hardening baseline](hardening.md) describes the switch),
   with Xft, Unicode text, AT-SPI and SVG support and without the demos;
2. builds and installs the components `bindings`, `bitmaps`, `config`,
   `localized`, `lib`, `include`, `tools/wml`, `clients/xmbind`, `clients/uil`
   and `clients/mwm` into a private prefix, with at most six parallel jobs;
3. runs `tests/automated/run-suite.py` against that prefix.

The driver runs the `run-*.py` scripts of `tests/automated` one at a time. The
scripts compile and link against the installed headers and libraries, so they
test what `make install` produces and not a build tree. A script is run when it
takes only the prefix and a results directory; scripts that need other inputs
(a build tree, a binary, a display) are listed as skipped, and the driver prints
how many there were. Each script starts its own X server with `xvfb-run`.
The exit status is 1 when any script that ran fails or exceeds its timeout
(900 seconds by default). Run it locally the same way:

```sh
python3 tests/automated/run-suite.py PREFIX RESULTS --only action-icons
```

`--only` limits the run to the named tests and `--resume` continues a run that
stopped. `RESULTS/summary.tsv` has one line per script (name, status, seconds)
and `RESULTS/NAME.log` holds its output. A failed job uploads these files and the
configure and build logs as the artifact `logs-<compiler>`.

## Fuzz corpus replay

The `fuzz-replay` job installs Clang 19 with libFuzzer and runs

```sh
python3 tests/fuzz/run-fuzz.py --all --replay
```

Each target runs once over every file of its corpus directory without mutation
(libFuzzer `-runs=0`), built with AddressSanitizer and UndefinedBehaviorSanitizer
that stop at the first report. The corpora hold the regression input of every
fixed defect, so a failure means that a fixed defect has returned. The command
prints one line per target and exits 1 on a crash or a build failure. The runner
reads the compiler from `CLANG` and takes the X11 headers from `pkg-config` on
Linux; see `tests/fuzz/README` for the requirements and for fuzzing runs of
longer duration.
