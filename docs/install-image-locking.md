---
nav_exclude: true
---
# XmInstallImage locking

`XmInstallImage` now delegates directly to `_XmInstallImage`, which owns the
process-locking and allocation contract. The public wrapper no longer acquires
the same process lock around the synchronized helper, avoiding a deadlock when
Xt is configured with a non-recursive process lock.

`XmGetPixmap` follows the same rule: it calls the synchronized scaled-pixmap
helper directly instead of calling `XmGetPixmapByDepth` while holding the
application lock. This prevents a second lock acquisition on the public
compatibility wrapper.

`XmeGetMask` follows the same rule when converting the generated mask name.
It now invokes the synchronized helper directly rather than reacquiring the
application lock through `XmGetPixmapByDepth`.

The focused regressions are `tests/automated/check-xm-install-locking.py` and
`tests/automated/check-xm-get-pixmap-locking.py`.

The same ownership rule applies to text-field insertion: the
`XmTextFieldInsert` wrappers do not acquire the application lock before
delegating to the public replacement wrappers, which already own that lock.
This is covered by `tests/automated/check-textfield-insert-locking.py`.

Command-dialog text convenience wrappers follow the same ownership rule:
`XmCommandSetValue` delegates to `XmTextFieldSetString`, and
`XmCommandAppendValue` delegates to the text-field getters/replacement APIs,
while `XmCommandError` delegates to the public list APIs, without taking an
outer application lock. Their focused check is
`tests/automated/check-command-locking.py`.

`XmTextFieldSetStringWcs` also unlocks before delegating to the public narrow
setter. Invalid wide-character conversion now clears the allocated buffer
instead of replacing it with, and later freeing, a string literal. The
conversion result is retained as `size_t`; the regression is
`tests/automated/check-textfield-wide-set-locking.py`, which also verifies the
temporary conversion allocation is checked.

Selection extraction now checks position ordering, `Cardinal`-sized allocation
limits, conversion-output bounds, and allocator results for both narrow and
wide output. The focused source check is
`tests/automated/check-textfield-selection-allocation.py`.

The full string getters apply the same checked allocation and conversion
termination rules; their focused check is
`tests/automated/check-textfield-string-allocation.py`.

That check also covers the empty-wide-string allocation path, which now
returns `NULL` cleanly if the one-character allocation fails.

The wide modify-verify path in `XmTextFieldSetString` now uses checked
`size_t` arithmetic and handles temporary-buffer allocation failure; this is
covered by `tests/automated/check-textfield-set-string-allocation.py`.

The setter also rejects a narrow or multibyte input whose byte count cannot be
represented by its callback `int` length before modifying the widget. This is
covered by `tests/automated/check-textfield-set-string-range.py`.

Conversion result counts are retained as `size_t` through the getter and
substring paths, so large successful conversions cannot wrap through an
intermediate `int`.

Substring extraction now saturates a source range larger than the public
`int` count at `INT_MAX` and reports truncation instead of narrowing a large
`XmTextPosition`. The focused check is
`tests/automated/check-textfield-substring-range.py`.

The multibyte byte-count helper also saturates instead of wrapping its `int`
result; this is covered by
`tests/automated/check-textfield-byte-count-bounds.py`.
