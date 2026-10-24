# UIL debug listing output

The debug build of `uil` writes symbol and machine-code details through a
small line accumulator.  The formatter now measures each variadic message,
formats it into owned storage, and emits it in bounded chunks.  Long comments,
symbol names, and other generated text therefore cannot overwrite adjacent
listing state.

Run the regression against a debug-enabled binary with:

```sh
python3 tests/automated/check-uil-listing.py /absolute/path/to/uil /absolute/path/to/results
```

The normal release build keeps the historical listing format and command-line
options; this change only makes the debug output path bounded.
