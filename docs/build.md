# Build and install

## Hosted documentation workflow

The GitHub Pages workflow is intentionally scoped to documentation inputs. It
runs automatically on `master` pushes and pull requests that change `docs/`,
`ckdocs.yml`, the pinned documentation builder, its provenance inputs, or the
workflow itself. Ordinary library and client commits do not start or cancel a
documentation run. Use the workflow's **Run workflow** action
(`workflow_dispatch`) when a documentation rebuild is needed without changing
one of those files.

The detailed regression and maintenance notes are marked as built-but-unlisted
reference pages. They remain available from the generated site index and
search, while strict builds verify every tracked Markdown page without turning
the main navigation into hundreds of top-level tabs.

Use an isolated source checkout, build directory and installation prefix.
Do not overlay development headers or libraries onto an older installation.
GNU make, a C compiler, Autoconf, Automake, Libtool, Flex/Bison and the X11/Xt
development libraries are needed for a source build. Optional image features
require their corresponding development packages; inspect configure results
instead of assuming they were enabled.

## Development text and accessibility features

`--enable-xft` requires Xft, Fontconfig and FreeType development files.
`--enable-unicode-text` additionally requires ICU (`icu-uc`, `icu-i18n`) and
HarfBuzz through `pkg-config`; it requires Xft to be enabled. There is one
ICU bidi/HarfBuzz shaping pipeline, with no GTK, Pango or Raqm dependency.
`--enable-atspi` enables the optional D-Bus transport and requires `dbus-1`.
The semantic provider API remains usable without that transport.
The Unicode configure probe uses `atomic_init` rather than the removed
`ATOMIC_VAR_INIT` macro, so current GCC and Clang drivers that select C23 mode
still pass the C11 atomic capability check.

Select explicit absolute paths appropriate to the machine:

```sh
export TMPDIR="$work/tmp"
mkdir -p "$TMPDIR" "$work/build"
(cd "$source" && sh autogen.sh)
(cd "$work/build" && "$source/configure" \
  --prefix="$prefix" --enable-xft --enable-unicode-text --enable-atspi)
make -C "$work/build" -j2
make -C "$work/build" install
```

Here `source`, `work` and `prefix` are caller-selected absolute paths, and
`work` is a fresh, project-owned scratch directory. Autotools bootstrap generates
files in the source checkout, so use a disposable source copy when preserving a
clean checkout matters. Select a scratch filesystem explicitly on systems with
such a policy. The command sequence is a build example, not validation evidence.

## Xt clipboard dependency

The complete clipboard lifetime gate requires the Xt INCR ownership correction.
Uncorrected Xt can free its atomic selection value after sending the final
segment and free the same pointer again if the peer disconnects before the final
acknowledgement. This is corrected in Xt's `Selection.c`, not with a toolkit shim
or a longer timeout. `patches/libXt-incr-lifetime.patch` clears the released
pointer so timeout cleanup remains safe.

On Linux, `scripts/build-xt-dependency.py` accepts the upstream
[libXt 1.3.1 archive](https://www.x.org/releases/individual/lib/libXt-1.3.1.tar.xz),
checks SHA-256 `e0a774b33324f4d4c05b199ea45050f87206586d81655f8bef4dba434d931288`,
applies the exact patch, and builds into a fresh explicit prefix. It performs no
downloads or system installation. Pass `--archive /absolute/libXt-1.3.1.tar.xz`,
`--output /absolute/xt-build` and optionally `--prefix /absolute/combined-prefix`.
An explicit prefix must be fresh or empty. The source/build/install receipts
record hashes, flags, compiler, timing and instrumentation scope.

Build Motif into the same selected prefix afterward. Set `CPPFLAGS` to include
its `include`, `LDFLAGS` to `-L` and `-Wl,-rpath,` its `lib`, and
`PKG_CONFIG_PATH` to its `lib/pkgconfig` before configuring. Verify runtime Xt
and Xm paths as well as their SONAMEs; a corrected library installed elsewhere
does not fix a process that loads system Xt. The system installation remains
untouched. Toolkit/consumer sanitizer builds are separate from this normal Xt
dependency profile.

## Consumers

Compile and link against the selected prefix explicitly. Ensure runtime loading
also uses that prefix, rather than a system `libXm` chosen accidentally. Rebuild
subclass libraries and all consumers of `P.h`/`I.h` records. See the [ABI cohort
checks](abi.md).

## Focused fixtures

`tests/Toolkit/Unicode/layout.c` and `line_context.c` exercise the generic capture
API with an X server. `text_unicode.c` exercises actual Text and TextField
widgets. `tests/Toolkit/ABI/cohort.c` compares header/runtime epoch and version.
Xvfb is suitable for automated geometry and readback fixtures, but it does not
replace inspecting the real application window, testing an actual XIM service,
or using an assistive client.

## Installed native feature suite

On Linux, use the toolkit's generic runner after installing one isolated cohort:

```sh
python3 tests/automated/check-native-features.py \
  --prefix "$prefix" \
  --out "$work/native-features"
```

Both paths must be absolute. The output directory must be fresh and separate
from the installed prefix; the runner creates its own `TMPDIR` there. It performs
no toolkit build or package installation, and requires no external build receipt.
For a custom library location use `--libdir` with an absolute directory inside
the prefix. `--cc` selects the native C compiler. `--only layout`, for example,
restricts the feature fixtures while still running the installed ABI check.

The clipping event observer needs a private runtime whose Unix socket path fits
within 100 bytes. For long result paths, supply
`--accessibility-runtime /absolute/short/fresh/path`; the runner creates it with
mode0700 and refuses an existing directory. Use `--sanitize` for an ASan/UBSan
prefix: this instruments the fixtures too and enables strict leak/error checks.

The full suite needs `pkg-config` with Xft, Fontconfig, HarfBuzz and ICU development
modules, a C11 compiler, `readelf`, `ldd`, Xvfb, `xvfb-run`, `xauth`,
`dbus-run-session`, `gdbus`, `stdbuf`, `scrot`, Python 3 with its D-Bus and GLib bindings
(`python3-dbus` and `python3-gi` on Debian), and real AT-SPI bus/registry
services. The text fixtures require DejaVu Sans and a CJK-capable fallback font.
Unavailable services or missing font coverage fail their tests; an in-memory
semantic substitute does not count as an AT-SPI pass.

The default suite contains 35 fixtures (36 registered, including opt-in XIM),
including `font-resize`, `accessibility-modality`,
`accessibility-clipping`/`accessibility-clipping-events` and the
`xmstring-empty-concat`/`xmstring-empty-components` public string regressions.
`--only local-compose` runs the local-input regression with its own authenticated
Xvfb/Openbox rather than an ambient display. It requires Xvfb, Openbox, xdotool,
xdpyinfo, xprop and native libXtst. Its frozen standalone Compose table and the
private server's original, mapped and restored keymaps are fingerprinted.
The observer requires actual local XIM/XIC registration, then sends XTest keys
through Text/TextField under both input-sharing policies. Exact UTF-8 bytes,
scalar carets, committed-value callback counts, unfocused-widget isolation and
grapheme motion/deletion are checked for Compose, dead acute and a two-scalar
combining commit. This source contract is separate from runtime qualification;
it does not imply external-provider, physical-keyboard or Escape behavior.
`--only accessibility-coordinates` checks Component screen/window/parent
coordinates on a private AT-SPI bus across two native shells, nested semantic
providers, a moved shell, unavailable origins, integer boundaries and a target
retired by a parent query. It saves the provider PID/executable/maps and a root
PNG after the geometry checks. Its private runtime uses the `-coords` sibling
of `--accessibility-runtime`, keeping it separate from the clipping-event bus.
The string fixtures cover valid empty payloads, construction/copy/traversal,
metadata comparisons and separator tables without changing nonempty behavior.
`--only accessibility-modality` selects the native/semantic action, focus, value
and Selection modal-admission regression (plus the mandatory ABI fixture).
It runs on the runner's isolated Xvfb and requires no AT-SPI service. Select it
the same way with an instrumented compiler and installed sanitizer prefix for
ASan/UBSan validation; native allocation-fault interposition remains separate.

`--only font-resize` selects native Label/TextField/wrapped Text/PushButton
checks at explicit Xft 12, 18 and 24 point sizes. The fixture inspects the actual
loaded point sizes and metrics, derives narrow/wide window sizes from measured
advances, and checks real X/Form geometry, width-dependent wrapping, preserved
UTF-8 bytes and scalar insertion positions, retained focus through font/resize
and unmap/remap, and exactly one native Space-key activation per profile. The
key is an XSendEvent delivered through the widget's ordinary translations;
it is not physical keyboard, IME or compositor-scale acceptance. The only
resource-count bound asserted is the Form's own four child identities/count.
Explicit font profiles do not establish fractional or compositor scaling.

`--only font-lifetime` isolates Xft rendition ownership from Text selections.
It exercises public table copies and merge modes, font and non-font
copy-on-write updates, native compound-string scratch rendering, caller-supplied
Xft fonts, native Labels
and two independent display connections, including continued rendering after
closing the first connection. Each connection's application widgets and
independent `XmDisplay` application shell are destroyed before `XtCloseDisplay`,
which closes the connection but does not destroy widgets. The fixture tracks
`XmDisplay` destruction through its callback: the last VendorShell normally
retires it automatically, so explicit cleanup occurs only if it remains live.
This avoids using or destroying an already retired widget. The second connection
continues to query and render after the first connection's complete teardown.
`--only default-font-cache` exercises the installed default-table extension API
with ten concurrent display connections, interleaved teardown, surviving native
Label rendering and two reopened connections. Default lookups return borrowed
tables; the fixture frees only its explicit public copies. It checks cache reuse
and growth beyond eight entries, while destroy callbacks guard automatic
`XmDisplay` retirement. Default tables are owned by the native display lifecycle;
their original cache references are released before the connection closes.
`--only label-gc` checks native insensitive Label
GC replacement and shadow-color dependencies. Strict sanitizer qualification of
these owning-layer fixtures is separate from the broader font/resize fixture.
`--only color-display-lifetime` exercises both independent-display close orders,
screen-specific icon policy, and the first still-live default for queries that
have no display argument. It checks surviving lookups and exactly-once ColorObj
destruction. The fixture seeds existing palette state with native black/white
pixels so an optional CDE color server is not required; this does not qualify
color-server discovery or protocol behavior. It also checks invalid screen and
palette extents and unavailable queries after the final owner is destroyed.
The internal rendition record now carries explicit Xft-reference ownership.
That header is not installed; the lifetime fixture uses installed public
rendition/render-table APIs and native compound-string rendering. Public
rendition and render-table handles remain opaque.

The default fixtures cover ABI/version, XIM preedit bounds, shaped glyph readback,
paragraph/line and bounded font contexts, native Unicode and large text widgets,
Unicode clipboard/INCR and interrupted-transfer lifetime, Disclosure and numeric
accessibility. Real XIM service testing is selected explicitly with
`--only xim-service`. Disclosure and accessibility also use external D-Bus observers.
Each consumer compiles sequentially with strict C11/POSIX 2008 warnings and
optimization. Compiler dependency graphs prove that actual Xm headers came from
the selected prefix. The runner checks native ELF identity, library SONAME,
executable dependencies and the exact `ldd`-resolved installed `libXm`; it rejects
mixed header/library origins. The linker and runtime library names must resolve
to the same installed file.

`receipt.json` retains host/compiler/tool identities, exact fixture and installed
header/library hashes, executable hashes, commands, timestamps, durations and
logs. Installed inputs and fixture sources are checked again after the suite.
The first recorded suite using this runner passed on native ARM Linux; retain
and compare receipts for the particular cohort being admitted. The small fixture
compilations are separate from the toolkit build and have a different duration.
A passed suite identifies the tested bytes; it does not prove which source built
the installation, complete Unicode/AT-SPI coverage, an actual XIM service journey,
or the appearance of a separate application's window.

### Primary-window modality fixture

`--only primary-modality` selects the generic window-modality regression (the
ABI check still runs). It additionally requires the `xtst` development module
and the XTEST server extension. The runner starts its own Xvfb display; no window
manager is required. The consumer refuses to run without its explicit
`--isolated-display` argument because it raises test windows and moves keyboard
and pointer focus. Do not run it on an interactive user display.

The fixture uses actual XTEST input rather than invoking button callbacks or
calling the widget's actions directly. It checks blocked parents and usable
independent windows, nested and parallel primary dialogs, enclosing full modals,
raw widget grabs, newly realized windows and deferred modal popups. Repeated
popdown and recursive destruction must restore input and the virtual grab count
without X errors or Xt warnings. It tracks XmDisplay retirement when destroying
the last VendorShell. This establishes the tested client input contract, not a
particular desktop's focus-stealing or placement policy.

The font-lifetime fixture calls FcFini only at terminal process teardown, after
all test-owned fonts, widgets, displays and the Xt application context have been
released. This finalizes Fontconfig's process-global configuration; toolkit code
does not finalize the application-wide library. Leak detection stays enabled
without suppressions. Independent controls distinguish this configuration from
unreleased client patterns. The default-font-cache fixture uses ten simultaneous
connections to exercise cache growth, per-display retirement and survivor drawing.
