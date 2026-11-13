---
nav_exclude: true
---
# Mrm icon allocation bounds

UID icon mapping validates dimensions, source strides, destination byte counts,
Xt's `Cardinal` allocation width, and descriptor raster sizes before allocating
image storage. Invalid or
unrepresentable icons return the existing Mrm failure result; ordinary icon
loading and XImage creation are unchanged.

The source-level regression is `tests/automated/check-mrm-icon-bounds.py`.

## Pixel values and the pixel size

`Urm__MapIconAllocate` replaces each pixel of an icon with the pixel value of
its colour table entry. The pixel value in the icon is data from the UID file
and can be as large as 255 for an 8 bit icon, whatever the number of entries in
the table. A value at or beyond the entry count of the table is a malformed
icon: the function frees its image storage and fails with `MrmFAILURE`, as it
does for the other malformed icons, instead of reading behind the table.

The function also reads the predefined background and foreground entries of the
table after the pixels are mapped. A missing table, or one that holds fewer than
the two predefined entries (`URMColorTableUserMin`), is rejected before any
storage is allocated.

The pixel size of an icon is one of four codes (`URMPixelSize1Bit` to
`URMPixelSize8Bit`) that select the number of bits per pixel. Any other value
used to leave that number at zero, so the loop over the bits of a byte never
advanced and the call never returned. An unknown pixel size is now rejected
before any storage is allocated. Valid icons are mapped as before.

### Test

```text
python3 tests/automated/run-mrm-icon-bounds.py RESULTS
python3 tests/automated/run-mrm-icon-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-icon-bounds.c` calls the production `Urm__MapIconAllocate` under
AddressSanitizer and UndefinedBehaviorSanitizer with icons and colour tables
that have exactly the size of their contents (the display is the stand-in of
`tests/fuzz`). Pixel values beyond the table, an unknown pixel size (bounded by
an alarm) and a missing or too small table fail cleanly; valid 1, 4 and 8 bit
icons are mapped. The crash input that found the defect is kept as a seed in
`tests/fuzz/corpus/mrm-uid`.
