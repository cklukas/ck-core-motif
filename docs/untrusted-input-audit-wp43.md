---
nav_exclude: true
---

# Untrusted-input and legacy-subsystem hardening audit (WP-43)

This note records the WP-43 input-validation, integer-overflow, and
allocation/teardown-lifetime audit across the UIL binary database loader
(`clients/uil/UilDB.c`), WML generator (`tools/wml`), the remaining legacy
first-party modules in `lib/Xm`, `lib/Mrm`, and `clients/mwm`, and the
October/November 2026 image, appearance, scaling, text, and accessibility
subsystems.

## Summary of defects fixed

1. **UIL WMD binary database loader (`clients/uil/UilDB.c`)**:
   - `db_incorporate()` now verifies `dbfile != NULL` after `db_open_file()`
     and before `fclose(dbfile)`, preventing a null `FILE *` dereference if
     diagnostic handling returns without a long jump.
   - `db_read_ints_and_string()` validates `header->num_items` and
     `header->table_size` *before* calling `XtCalloc(1, (Cardinal)header->table_size)`
     so a negative `table_size` cannot wrap to a `UINT_MAX` allocation, rejects
     negative `table[i].b_length`, and verifies that each entry in
     `string_table` is NUL-terminated at `string_table[table[i].b_length]`.
   - `db_read_length_and_string()` verifies that each non-empty string in
     `string_table` is NUL-terminated at `string_table[lengths[i] - 1]` before
     comparing against `"user_defined"` or publishing the pointer into UIL
     symbol tables, and explicitly clears `table[i] = NULL` when
     `lengths[i] == 0`.
   - `db_read_int_and_shorts()` explicitly clears `table[i].values = NULL` when
     `table[i].values_cnt == 0` so raw bytes read from a damaged `.wmd` file
     cannot leave a non-null forged pointer in an empty enum set descriptor.

2. **First-party legacy modules (`lib/Xm`, `lib/Mrm`, `clients/mwm`)**:
   - `lib/Xm/ReadImage.c` (`_XmReadImageAndHotSpotFromFile`): rejects null
     `display` or `filename` inputs and frees `XReadBitmapFileData`'s `data`
     buffer with `XFree(data)` when `_XmCreateImage` fails.
   - `lib/Xm/AtomMgr.c` (`XmInternAtom`, `XmGetAtomName`): guards null `display`,
     null `name`, and `None` atoms before calling Xlib.
   - `lib/Xm/CvtUnit.c` (`XmCvtStringToUnitType`): guards null `from_val`,
     `to_val`, default `Display`, and `XmDisplay` widget before calling
     `XtConvertAndStore`.
   - `lib/Xm/Dest.c` (`_XmSetDestination`, `XmGetDestination`): guards null
     `Display` pointers and checks `dd->display.displayInfo != NULL` during
     display teardown.
   - `lib/Xm/DialogSE.c` (`DeleteWindowHandler`, `GetManagedKid`): guards null
     widget, closure, and composite child-list pointers.
   - `lib/Xm/DrHiDash.c` (`_XmDrawHighlight`) and `lib/Xm/DrPoly.c`
     (`XmeDrawPolygonShadow`): guard null `Display`, `Drawable`, `GC`, and
     `XPoint` pointers, non-positive dimensions/counts, and null `XPolygonRegion`
     returns.
   - `lib/Xm/FocusAct.c` (`_XmMenuButtonTakeFocus`, `_XmMenuButtonTakeFocusUp`):
     guards null `wid`, `XtDisplay(wid)`, and `event` pointers.
   - `lib/Xm/GetRes.c` (`XmGetSecondaryResourceData`): initializes
     `*secondaryDataRtn = NULL` and rejects null `w_class` or output pointers.
   - `lib/Xm/IconH.c` (`GetContainerParent`, `XmCreateIconHeader`): guards null
     `wid` and `parent` pointers.
   - `lib/Xm/ObsoXme.c` (`_XmVirtualToActualKeysym`, `_XmResizeObject`,
     `_XmMoveObject`): guards null display, output, and widget pointers.
   - `lib/Xm/ShellE.c` (`StructureNotifyHandler`): guards null widget, closure,
     event, `xmScreen`, and `continue_to_dispatch` pointers, and prevents
     unsigned underflow of `xmScreen->screen.numReparented`.
   - `lib/Xm/TearOffB.c` (`HeightDefault`, `Redisplay`, `BDrag`, `BActivate`,
     `KActivate`): guards unsigned `Dimension` subtraction when
     `width <= 2 * highlight_thickness` or `height <= 2 * highlight_thickness`,
     and checks parent and `XmQTmenuSystem` trait pointers before dispatch.
   - `lib/Xm/TravAct.c` (`_XmGetFocusFlag`, `_XmSetFocusFlag`,
     `_XmTrackShellFocus`): guards null widget, display, `displayInfo`, closure,
     event, and `dontSwallow` pointers.
   - `lib/Xm/ValTime.c` (`_XmValidTimestamp`): guards null `w`, `XtDisplay(w)`,
     shell-parent walk, and unrealized `XtWindow(w) == None` so an unrealized
     hierarchy returns `CurrentTime` instead of blocking in `XWindowEvent`.
   - `lib/Mrm/Mrmlwrite.c`, `lib/Mrm/Mrmwwrite.c`, and `lib/Mrm/Mrmos.c`:
     validate non-empty resource index strings before mutating context group
     metadata and guard null float/double conversion pointers.
   - `clients/mwm/WmCPlace.c` (`SetupPlacement`, `IsRepeatedKeyEvent`,
     `DoPlacement`): guards null client/event pointers and prevents unsigned
     underflow when subtracting frame offsets from `placeWidth`/`placeHeight`.

3. **October/November 2026 subsystems (`lib/Xm/Png.c`, `lib/Xm/Jpeg.c`, `lib/Xm/AppearanceDraw.c`)**:
   - `lib/Xm/Png.c` (`_XmPngGetImage`): guards null `screen`, `infile`, and
     `ximage` arguments and bounds `image_width`, `image_height`, and
     `image_rowbytes` against `INT_MAX` and `image_channels` (`3` or `4`) before
     `XCreateImage` and `png_process_image`.
   - `lib/Xm/Jpeg.c` (`load_jpeg`, `_XmJpegGetImage`): guards null `screen`,
     `infile`, and `ximage` arguments, requests `JCS_RGB` decompression for
     non-grayscale color spaces (such as CMYK/YCCK JPEGs), and verifies
     `cinfo.output_components` is `1` or `3` so scanline reads into 3-byte
     `CTable` entries cannot overflow `*image_data`.
   - `lib/Xm/AppearanceDraw.c` (`_XmAppearanceRoundedRectangleRegion`,
     `_XmAppearanceDrawButtonFocus`, `_XmAppearanceDrawButton`,
     `_XmAppearanceDrawPickerFrame`): checks `XCreateRegion()` returns for
     `NULL` before union/subtraction/destruction and guards picker chevron `x`
     coordinates against negative offsets on narrow widgets.

## Validation

`tests/automated/check-untrusted-input-audit-wp43.py` verifies the source
contracts across all audited files and compiles and runs an AddressSanitizer +
UndefinedBehaviorSanitizer harness exercising the null, bounds, and malformed
WMD/image entry points.
