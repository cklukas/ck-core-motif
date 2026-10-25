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
- [WM state read guards](wmstate-read-guards.md) — safe handling of window-manager state properties.

## Accessibility and diagnostics

- [Accessibility support](accessibility.md) — AT-SPI integration and qualification boundaries.
- [UIL listing safety](uil-listing.md) — bounded compiler listing output.
- [Documentation build](documentation.md) — generated reference and hosted-site behavior.

The detailed notes are intentionally small and issue-focused. When a topic
needs a broader design decision, the review and status pages above are the
authoritative summaries.
