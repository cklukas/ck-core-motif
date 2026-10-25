# Reference by topic

This page is the starting point for the detailed Motif maintenance notes.
The site navigation intentionally stays short; individual fix notes remain
available through search and the generated site index, while these topics
provide stable paths to the material most useful when integrating or
debugging a client.

For a broader route through the issue notes, use the [maintenance overview](maintenance-overview.md)
or the [MWM and window-management overview](mwm-overview.md).

## Build, compatibility, and qualification

- [Build and test workflow](build.md) — configure, build, test, and inspect an installed library.
- [ABI and compatibility](abi.md) — compatibility promises and boundaries used by this fork.
- [Maintenance review](maintenance-review.md) — current review scope and remaining qualification work.
- [Current status](status.md) — platform and feature qualification status.

## Text, Unicode, and input

- [Unicode support](unicode.md) — locale, composition, shaping, and input behavior.
- [Non-text Unicode coverage](nontext-unicode.md) — multilingual labels, lists, menus, and other widgets.
- [Text-source bounds](text-source-bounds.md) — safe range and size handling in text sources.
- [Input-method rollback](vendor-im-string-rollback.md) — failure cleanup at the input-method boundary.

## Rendering, images, and display metrics

- [Image scaling](image-scaling.md) — scaling behavior and compatibility constraints.
- [Image scaling allocation guards](image-scaling-allocations.md) — malformed or oversized image handling.
- [Image scaling coordinate bounds](image-scaling-coordinate-bounds.md) — coordinate conversion limits.
- [DPI control metrics](dpi-control-metrics.md) — display-aware sizing rules.
- [Theme configuration](theme-configuration.md) — classic and optional visual configuration.
- [Desktop color-cell allocation](desktop-color-cells-allocation.md) — safe icon-color population under colormap pressure.

## Selection, drag-and-drop, and data conversion

- [Motif text selection targets](motif-textsel-targets.md) — target discovery and conversion behavior.
- [Selection-box compound text](motif-selectionbox-compound-text.md) — compound-string selection data.
- [Drag-and-drop target allocation](dragbs-targets-allocation-guards.md) — allocation failure handling.
- [Container tab conversion](container-tabs.md) — container data conversion and tab behavior.

## MWM and window-management protocol

- [MWM protocol overview](mwm-protocol-oom.md) — protocol conversion and allocation-failure behavior.
- [Workspace-management command messages](command-message-argument-allocation.md) — bounded command-message construction.
- [WSM string unpack allocation](wsm-unpack-string-oom.md) — safe failure handling for decoded strings.
- [WSM configuration-string failure](wsm-config-string-oom.md) — rollback of partial format updates.
- [WSM window-entry allocation](wsm-win-entry-oom.md) — safe decoding of variable-length entry arrays.
- [WSM window-list rollback](wsm-windows-rollback.md) — cleanup of partial request decoding.
- [WSM window-data alignment](wsm-windata-alignment.md) — preserve stream offsets on data OOM.
- [WSM message packing allocation](wsm-pack-oom.md) — reject non-empty buffer OOM safely.
- [WSM window-entry reply sizing](wsm-win-entry-size.md) — account for both variable arrays.
- [WSM window-request ownership](wsm-windows-owned.md) — track decoded array ownership correctly.
- [WSM nested window-array ownership](wsm-windows-nested-ownership.md) — initialize and release nested property arrays.
- [WSM cleanup idempotence](wsm-free-idempotent.md) — prevent repeated cleanup from double-freeing.
- [WSM reply callback lifecycle](wsm-reply-callback-lifecycle.md) — guard optional callbacks and failure cleanup.
- [WSM null transfer data](wsm-reply-null-data.md) — handle malformed selection callbacks safely.
- [WSM null request context](wsm-reply-null-context.md) — guard missing pending-request state.
- [WSM configuration rollback pointers](wsm-config-clear-pointers.md) — clear freed attribute-array references.
- [WSM configuration pointer lifetime](wsm-config-pointer-lifecycle.md) — initialize and release arrays independently of counts.
- [WSM target conversion allocation failure](wsm-target-conversion-null.md) — handle missing display metadata safely.
- [WSM packer configuration failure](wsm-packer-config-null.md) — reject missing format metadata safely.
- [WSM WM-target mapping](wsm-wm-target-mapping.md) — map WM-specific atoms in both directions.
- [WSM window-data lookup](wsm-win-data-lookup-null.md) — reject null or empty data lists safely.
- [WSM atom initialization rollback](wsm-atom-init-rollback.md) — discard partial display metadata safely.
- [WSM configuration quark serialization](wsm-quark-string-null.md) — handle invalid quarks without crashing.
- [WSM string packing](wsm-pack-string-null.md) — serialize null strings safely.
- [WSM extension sizing](wsm-extension-size-null.md) — size null entries consistently.
- [WSM configuration reply arrays](wsm-config-reply-arrays.md) — validate count/array consistency.
- [WSM reply array state](wsm-reply-array-state.md) — validate state-list pointers before sizing.
- [WSM extension array state](wsm-extension-array-state.md) — validate extension-list pointers.
- [WSM request array state](wsm-request-array-state.md) — validate request arrays before packing.
- [WSM nested array state](wsm-nested-array-state.md) — validate per-record lists before sizing.
- [WSM property wire width](wsm-wire-width.md) — size serialized properties at protocol width.
- [WSM request counts](wsm-request-counts.md) — reject negative list counts before packing.
- [WSM reply counts](wsm-reply-counts.md) — reject negative reply list counts before packing.
- [WSM reply configuration metadata](wsm-reply-config-null.md) — reject unavailable formats safely.
- [WM state read guards](wmstate-read-guards.md) — safe handling of window-manager state properties.

## Accessibility and diagnostics

- [Accessibility support](accessibility.md) — AT-SPI integration and qualification boundaries.
- [UIL listing safety](uil-listing.md) — bounded compiler listing output.
- [Documentation build](documentation.md) — generated reference and hosted-site behavior.

The detailed notes are intentionally small and issue-focused. When a topic
needs a broader design decision, the review and status pages above are the
authoritative summaries.
