# C++ support-library prototype (WP-08)

This document records the consumer-side fixture that drove the optional C++
support layer. The public header is now installed as
`ckmotif++/motif_support.hpp`; this fixture remains the lifecycle proof against
a real installed prefix and does not change `libXm`, Xt resources, UIL,
callbacks or widget ownership.

## Supported prototype contract

- `motif::string` owns one `XmString`; copies duplicate the toolkit value and
  moves transfer it. Destruction calls `XmStringFree` exactly once.
- `motif::args` owns copied resource names and `XtArgVal` values. `apply()`
  builds a temporary native `Arg` array for one Xt call and seals the list so
  its storage cannot be mutated after hand-off.
- `motif::widget_ref` is a non-owning view. It never destroys or registers a
  global wrapper for the `Widget`.
- `motif::callback_connection` owns one Xt callback registration and a
  destroy callback that invalidates the connection when the widget is
  destroyed first. Its C callback trampoline catches the callback and error
  sink exceptions before returning through Xt.

All operations remain on the owning Xt application thread. The support layer
does not provide asynchronous dispatch, thread safety, private-record access
or an implicit event loop. See the [installed support-library reference](cpp-support-library.md)
for the release contract and package metadata.

## Installed-prefix check

Run it against a fresh ABI-matching install:

```sh
python3 tests/automated/run-cpp-support-prototype.py \
  --prefix /absolute/installed/prefix \
  --out /absolute/fresh/cpp-support-results
```

The runner records source/header hashes, compiler/linker commands, loaded
`libXm.so.8` identity and the Xvfb result in `receipt.json`. The fixture
covers copied/moved `XmString` values, one-shot argument storage, raw-widget
interoperation, move-only connection ownership, callback exception capture,
callback continuation after the exception, and destruction-before-connection-
release. Pass `--sanitize` for AddressSanitizer/UBSan with leak detection.
