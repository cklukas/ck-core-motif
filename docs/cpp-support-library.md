# Optional C++17 support library

CK Core Motif can install an optional, header-only C++17 support layer as
`ckmotif++`.  It is an ownership and exception-boundary aid for the existing
Xt/Motif ABI, not a second widget toolkit.

## Installation and use

The support layer is opt-in at build/install time through the normal headers
and pkg-config metadata.  A consumer can use:

```sh
c++ -std=c++17 widget.cpp $(pkg-config --cflags --libs ckmotif++-8)
```

The installed header is `ckmotif++/motif_support.hpp`; the package metadata is
`ckmotif++-8.pc`.  The C++ layer adds no required dependency to a C-only
`libXm` build and leaves the Motif ABI unchanged.

## Contract

- `motif::string` owns one `XmString`; copies duplicate it and moves transfer
  ownership.  Destruction calls `XmStringFree` once.
- `motif::args` copies resource names and stores `XtArgVal` values in order.
  `apply()` constructs a temporary native `Arg` array, seals the list only
  after construction succeeds, and requires the callback not to retain that
  array after it returns.
- `motif::widget_ref` is a non-owning view.  It never destroys or registers a
  global wrapper for a `Widget`.
- `motif::callback_connection` owns one Xt callback registration and installs
  a destroy callback.  Widget-first destruction invalidates the connection;
  reset from inside a callback defers state deletion until the active
  trampoline returns.
- The callback trampoline catches both the user callback and error-sink
  exceptions before returning through Xt's C ABI.

All widget, display, callback-registration and wrapper-destruction operations
must run on the owning Xt application thread.  The layer does not provide
thread-safe widgets, asynchronous dispatch, private-record access or an
implicit event loop.

## Qualification

`tests/automated/run-cpp-support-prototype.py` compiles the installed header,
checks that the executable resolves `libXm.so.8` from the selected prefix and
runs the lifecycle fixture under Xvfb.  The accepted release matrix covers a
fresh installed prefix with GCC and Clang, plus AddressSanitizer/Undefined
BehaviorSanitizer runs with leak detection enabled.  The fixture exercises
copy/move ownership, ordered argument storage, mixed C/C++ widget use,
callback exception capture, callback continuation, widget-first destruction
and explicit connection reset.

The matrix is native AArch64 Linux on `uconsole`; x86_64 remains a separate
toolkit-wide qualification gate.  No claim is made that the wrappers make
Xt thread-safe or that a system Xt leak outside this support layer is owned by
the C++ package.
