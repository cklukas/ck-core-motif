# DataField wide-text conversion

DataField supports the public access-text trait for callers that exchange
`XmFORMAT_WCS` text. If a supplied wide string cannot be represented in the
current locale, the existing DataField behavior is to set an empty value.
The conversion path now keeps its allocated empty buffer until it finishes,
instead of replacing it with a static string and freeing that static storage.
This lets callers recover from invalid wide input without terminating their
process.

Valid wide input keeps its existing locale conversion behavior. The trait
format, DataField value resources, callbacks, widget layout and ABI remain
unchanged.

## Native regression

`tests/automated/check-datafield-wcs.c` gets the standard
`XmQTaccessTextual` trait from a DataField, verifies its normal wide-text
conversion, then sends an invalid UTF-8-wide scalar. It checks that the field
becomes empty and the client remains usable. The fixture runs against an
installed library under Xvfb; it does not qualify a live input method or
application-level data validation.

Compile and run on Linux against an absolute installed prefix, using a writable
task temporary directory:

```sh
export TMPDIR=/absolute/task/tmp
prefix=/absolute/toolkit/prefix
cc -std=c11 -Wall -Wextra -Werror -g -O2 \
  -I"$prefix/include" -L"$prefix/lib" -Wl,-rpath,"$prefix/lib" \
  tests/automated/check-datafield-wcs.c -lXm -lXt -lX11 \
  -o "$TMPDIR/check-datafield-wcs"
LD_LIBRARY_PATH="$prefix/lib" xvfb-run -a "$TMPDIR/check-datafield-wcs"
```

Native AArch64 validation passes the five access-text checks above, the
installed ABI check, and the existing SpinBox, List Unicode and DPI widget
regressions. The unchanged prior library aborts on the invalid-wide-character
case.
