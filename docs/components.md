# Reusable native components

Reusable widgets belong in the toolkit when their interaction, geometry,
keyboard behavior and semantic contract are useful across independent Xt
applications. An application-specific document rule or workflow belongs with
its model or application owner.

## Component requirements

A native component needs a public resource/callback contract, explicit child
ownership, geometry negotiation, keyboard and focus behavior, lifecycle cleanup
and an accessibility projection. Its tests should exercise actual widget
behavior, multiple instances, resizing and destruction; a screenshot or an
unused API declaration is insufficient.

Unicode paragraph/capture facilities and accessibility providers are examples
of generic reusable services already implemented in the development source.
They separate immutable values and lifetime rules from application behavior.

## Current and future widgets

The development [disclosure section API](disclosure.md) provides a stable
content host, explicit expansion state and transition callbacks. Its public
contract and installed native ARM64 fixture results are documented together.
Application usability and other-architecture acceptance remain separate.

Other recurring patterns can justify future shared components: labelled field
rows, searchable selectors, notification/status areas and split-pane navigation.
These are design candidates, not available APIs or promised release features.
Each should be implemented only with a concrete reusable contract and
meaningful native tests. Prefer composition over copying application-specific
panels into the toolkit.
