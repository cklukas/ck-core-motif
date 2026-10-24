# ComboBox selection after item deletion

When `XmComboBoxDeletePos` deletes the selected item and other items remain,
the ComboBox selects its successor. If the deleted item was last, it selects
the new last item instead. The list selection, displayed text, selected-item
and selected-position resources, and selection callback describe that surviving
item. Previously, final-item deletion attempted to select the old last index,
which was beyond the shortened list, leaving the deleted value displayed and
omitting the replacement-selection callback.

This correction applies to all three ComboBox types. Deleting an unselected
item keeps the existing selection and does not send a selection callback.
The deletion API continues to use one-based positions, with zero denoting the
last item, regardless of `XmNpositionMode`. Selected-position resources and
callback positions still follow `XmZERO_BASED` or `XmONE_BASED` as configured.
The behavior when deleting the sole remaining item is unchanged. Public
signatures, resources, callback structures, widget layouts and ABI are unchanged.

## Native regression

`tests/automated/check-combo-delete.c` exercises every selected position and
every deletion position in lists of two through five items, including the
zero-position alias, for `XmCOMBO_BOX`, `XmDROP_DOWN_COMBO_BOX` and
`XmDROP_DOWN_LIST`, in both position modes: 408 cases. It checks surviving item
order, list selection, selected resources, actual TextField text, callback
counts, callback item and position, reason, and programmatic event identity.
This installed-library fixture runs under Xvfb; it does not qualify physical
input, popup placement or a consuming application's window.

Compile and run on Linux against an absolute installed prefix, using an
explicit writable task temporary directory:

```sh
export TMPDIR=/absolute/task/tmp
prefix=/absolute/toolkit/prefix
cc -std=c11 -Wall -Wextra -Werror -g -O2 \
  -I"$prefix/include" -L"$prefix/lib" -Wl,-rpath,"$prefix/lib" \
  tests/automated/check-combo-delete.c -lXm -lXt -lX11 \
  -o "$TMPDIR/check-combo-delete"
LD_LIBRARY_PATH="$prefix/lib" xvfb-run -a "$TMPDIR/check-combo-delete"
```

Native AArch64 validation passes all 408 cases and 5,880 checks. The unchanged
prior library fails the same fixture when deleting the selected last item.
Installed ABI, existing SpinBox, List Unicode and DPI regressions, the
1,072-check PanedWindow regression and the 630-check button-timer regression
also pass against the independent fixed build.
