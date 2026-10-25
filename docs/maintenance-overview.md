---
nav_exclude: true
---

# Maintenance overview

This page groups issue-focused notes by the kind of Motif behavior they affect.
Start here when reviewing a fix, then follow the focused page for its contract
and regression check.

## Build, ABI, and qualification

- [Build and test workflow](build.md)
- [ABI and compatibility policy](abi.md)
- [Current validation status](status.md)
- [Documentation build and publication](documentation.md)

## Input, text, and Unicode

- [Unicode support](unicode.md)
- [Non-text Unicode coverage](nontext-unicode.md)
- [Text-source bounds](text-source-bounds.md)
- [Input-method rollback](vendor-im-string-rollback.md)
- [Clipboard and selection targets](motif-textsel-targets.md)

## Rendering, images, and display metrics

- [Image scaling](image-scaling.md)
- [Image allocation guards](image-scaling-allocations.md)
- [Image coordinate bounds](image-scaling-coordinate-bounds.md)
- [DPI control metrics](dpi-control-metrics.md)
- [Theme configuration](theme-configuration.md)

## Accessibility and interaction

- [Accessibility support](accessibility.md)
- [Accessibility journey matrix](accessibility-journey-matrix.md)
- [Container tabs](container-tabs.md)
- [Keyboard and focus behavior](paned-keyboard.md)

The [360-degree review](360-review.md) records remaining qualification gaps
and separates open evidence work from known source defects.
