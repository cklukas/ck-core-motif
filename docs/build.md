# Build and install

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
