# C++ support-library prototype (WP-08)

This document records the consumer-only prototype for the optional C++ support
layer. It is deliberately not installed and does not change `libXm`, Xt
resources, UIL, callbacks or widget ownership. The header is a test artifact at
`tests/cpp/motif_support.hpp` so the proposal can be exercised against a real
installed prefix before an SDK package is considered.

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

All operations remain on the owning Xt application thread. The prototype does
not provide asynchronous dispatch, thread safety, private-record access or an
implicit event loop. The callback connection is intentionally a small
consumer-side experiment; it is not an installed ABI.

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
release. It does not establish a release-quality SDK, cross-compiler ABI,
sanitizer-clean Xt dependency, or thread-safe widget access; those remain WP-09
release gates.
