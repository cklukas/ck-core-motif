# Native ABI cohort

Development version **2.4.0** uses native ABI epoch **9**. The suite's libtool
version is `9:0:0`, yielding `libXm.so.9`, `libMrm.so.9` and `libUil.so.9` on ELF
systems. There is no alias or compatibility shim for the preceding SONAME.

Adding a public function is not the only ABI concern. Cached Unicode maps,
text-field layout state and text-output cache state change native widget/source
records. The line-table width changes from `Dimension` to `int`, avoiding
16-bit truncation. Code that subclasses widgets, embeds native records or
accesses `P.h`/`I.h` members is a binary consumer of those layouts.

The first published 2.4 development cohort used epoch 5. Widget font-context
fields and public accessibility info/provider value fields change layouts again;
epoch 6 separates these consumers explicitly. Rebuild custom semantic providers,
not only applications and widget subclasses. No old-SONAME alias is provided.

## Required rebuild

Rebuild the toolkit suite, custom widgets and subclass libraries, Mrm/Uil
integrations, resource-compiler consumers and applications against one exact
installed cohort. Remove stale generated objects first. Static linkage also
requires a rebuild. The `libXm` static link interface explicitly carries its
math-library dependency (`-lm`), which is needed by the text metrics
implementation. Never mix headers from one prefix with another prefix's
runtime library.

```c
#include <Xm/Xm.h>

if (XmToolkitABI() != XmABI_VERSION ||
    xmUseVersion != XmVersion) {
    /* Refuse this installation before creating native widgets. */
}
```

`XmVERSION`, `XmREVISION` and `XmUPDATE_LEVEL` describe source version;
`XmABI_VERSION` and `XmToolkitABI()` identify native layout epoch. Matching
numbers are necessary but insufficient: optional features and exact source
bytes can still differ. Prove compilation header paths and runtime-loaded
library paths, retain the configure manifest and hashes, and run the cohort
fixture against the installed prefix before selecting it.

Keep an older verified prefix isolated while its consumers remain in use.
Retire it after they have moved to the rebuilt, verified replacement. The
repository's root `ABI.md` is the source policy for this cohort.

## Epoch 9 semantic appearance roles

Primitive, Manager and Gadget records now store the public `appearanceRole`
resource. These base-record size changes affect every derived widget and gadget.
Rebuild Xm, Mrm, Uil, generated resource consumers, native subclasses and
applications together. The role's Xt resource representation is an unsigned
byte; its public enum constants are also available to UIL. No epoch 8 objects
may be linked into this cohort. Qualification of the new cohort remains a
separate gate from implementing the records and resources.

The ComboBox record also tracks whether each dimension is natural or explicitly
supplied. Font and appearance updates recompute natural geometry even before
realization; parent layout allocations do not become application size overrides.
Setting a dimension to zero restores natural sizing.

## Epoch 8 incremental native Unicode source maps

TextField now owns an immutable Unicode source-map reference alongside its
layout capture. Native shared Text sources retain maps through approved edits.
The TextField private record changes size, so the whole toolkit and all native
consumers must rebuild for epoch 8. No alias or mixed epoch layout is provided.
The new map API retains unchanged LF-delimited paragraph bodies and exposes
construction-work accounting. Captures may retain old immutable maps across edits.
Fresh installed-prefix/runtime qualification is separate from source implementation.

## Epoch 7 native list and selection records

Native List items now own monotonic, non-reused identities, independent of
positions and labels. The native List record owns its next identity and coherent
change callback. Public accessibility info/provider records add selection
capability and typed selection callbacks. These change both native private
records and custom provider layout. Epoch 7 requires a coherent rebuild of
Xm/Mrm/Uil, subclasses, custom providers and applications. The earlier epoch 6
prefix remains valid only for its separately qualified consumers. No alias or
parallel compatibility layout is supplied. The fresh installed-prefix build and all 20 native ARM64 feature fixtures pass,
including the exact libXm.so.7 loader boundary and native list protocol/fault
checks. Nine focused ASan/UBSan fixtures also pass for the rebuilt toolkit, including
List protocol/lifetime and provider reentrancy. Xt/system dependencies are
uninstrumented and leak checking is disabled. Actual-application adoption and
x86_64 compilation/runtime qualification remain separate gates.

TreeView's epoch 9 private record also stores automatic row-height and overall
height ownership, plus its `selectColor` pixel. Subclasses embedding `XmTreeViewRec` must be rebuilt
with this cohort; public opaque widget users do not access these fields.

The Sidebar epoch 9 record owns its header label widget and copied title string.
Rebuild subclasses embedding `XmSidebarRec` with the matching headers.

ArrowButton and ArrowButtonGadget epoch 9 private records track pointer-inside
state independently of the armed state so expose does not repaint a dragged-out
control as pressed. Rebuild private-record consumers with this cohort.

PushButton, PushButtonGadget and DrawnButton epoch 9 private records also retain pointer-inside
state for armed drag feedback across expose. Rebuild private-record consumers.

The epoch 9 Label and LabelGadget private records also own an uncached
`insensitive_foreground` pixel. Rebuild subclasses using these records; the
label gadget cache layout does not carry this per-instance text color.

PushButton and PushButtonGadget also record the effective classic default-frame
thickness and the space included in their margins. These epoch 9 private fields
separate public default-action state from theme-dependent geometry.

The epoch 9 XmTree private record owns a coalesced connector repaint work ID.
Subclasses embedding `XmTreeRec` must rebuild with the matching private header.

The epoch 9 ProgressRing and Spinner private records track render-table alias
initialization and automatic dimension ownership. Rebuild subclasses embedding
these records with the matching private headers.

The epoch 9 Scale private record tracks automatic width and height ownership.
Rebuild subclasses embedding `XmScaleRec` with the matching private header.

The pending epoch-9 TreeView record also owns its activation callback list and
pending pointer-press identity. Rebuild TreeView subclasses with this cohort.

The pending epoch-9 ScrolledWindow record owns its wheel action hook and current
gesture window/button/claim state. Rebuild ScrolledWindow/MainWindow subclasses
and all consumers embedding these records with the same installed headers.

## Pending epoch 9 tab overflow records

The scrolling TabBox stores its logical major-axis offset as `long long`,
preventing accumulated tab widths from overflowing an `int`. TabStack adds
its `tab_arrow_placement` resource field and forwards it to the internal
TabBox. These private-record changes belong to the same pending epoch 9
cohort; rebuild subclasses and all consumers against its exact headers and
library together.

The pending epoch-9 ButtonBox constraint record stores an outstanding geometry
compromise and its original preferred dimensions. Subclasses that embed
`XmBBoxConstraintsRec` must rebuild against the matching headers and library.

The pending epoch-9 Link private record stores independent automatic width and
height ownership. Rebuild subclasses and consumers embedding XmLinkRec with this
cohort. The public Link resources remain unchanged.

The pending epoch-9 Prompt, IconLabel and Badge private records store independent
automatic width/height ownership. Rebuild subclasses and private-record
consumers with the matching headers and library. Public resources are unchanged.

The pending epoch-9 Chip private record tracks automatic dimensions and pointer
targets. It no longer stores an unused background GC. Rebuild subclasses and
private-record consumers with the matching headers and library.

The pending epoch-9 Avatar private record removes the unused core-font fallback
pointer and records automatic width/height ownership. Text uses its owned
render table; rebuild private-record consumers
against matching headers and library. Public resources are unchanged.

The pending epoch-9 Divider private record tracks independent automatic
width/height ownership. Rebuild private-record consumers with matching headers
and library. Public resources are unchanged.

The pending epoch-9 NumberBox now derives from XmManager and owns native
TextField/ArrowButton children instead of a Primitive text buffer and drawing
GCs. Its private class/instance layouts change; rebuild consumers and subclasses.
Focus enters its editor, obtainable with XmNumberBoxGetTextField.

The pending epoch-9 Stepper now derives from XmManager and owns native
PushButton/Label children instead of a Primitive drawing GC. Its private class
and instance records change; rebuild consumers and subclasses. The public numeric
API remains available; XmNspacing controls the gaps around the value.

The pending epoch-9 ColorPicker now derives from XmManager and owns native
editing controls. Its value storage is explicitly 32-bit, matching its Cardinal
resource. Rebuild private-record users and subclasses. New public helpers expose
the preview value, borrowed editor, explicit commit and revert operations.
The multi-mode extension adds mode selection and HSV/Lab preview APIs, with
private native mode controls, model coordinates and owned raster/palette caches.

The pending epoch-9 TextField private class record adds subclass decoration
insets and repaint hooks. SearchBox uses these without changing its TextField
inheritance or the public margin resources. Its private instance also tracks
clear-button press state. Rebuild TextField/SearchBox subclasses and consumers
with the matching private headers; older class records are not compatible.

The pending epoch-9 AutoSuggestBox private record owns a scrolled-list popup,
coalesced layout work, independent automatic dimensions, deferred show state,
owned cancellation and focus-settlement timers, and transient outside-click
observation records.
Rebuild subclasses and private-record users with the matching header.

The pending epoch-9 ToggleSwitch private record now tracks render-table alias
initialization, independent automatic dimensions, pointer arming and hover. It
removes unused color fields and superseded GCs; subclasses must rebuild.

The pending epoch-9 RatingControl private record tracks independent automatic
width and height ownership. Native subclasses must rebuild with this layout.
