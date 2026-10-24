# WP-29 modern-layout requirements corpus

WP-29 asks whether an additive layout family is justified by installed-client
evidence.  The corpus is deliberately independent of any private application
and reuses the qualified WP-10 consumer fixture:

```sh
MOTIF_TEST_PREFIX=/path/to/prefix \
  python3 tests/automated/run-form-layout-corpus.py /path/to/prefix results
```

The fixture covers nested `XmForm` and `XmScrolledWindow` composition,
`XmRowColumn`, resize storms, map/unmap, post-realization insertion and
removal, manage/unmanage, callback-triggered mutation, destruction, and the
same-value negative control.  The WP-11 decision recorded two identical runs:
the result hashes and geometry/redraw/event metrics matched, and the no-op
control produced zero additional geometry requests or Configure events.

## Requirements matrix

| Requirement | Existing evidence | Candidate contract implication |
| --- | --- | --- |
| Main-axis stacking and gaps | `XmRowColumn` composition is deterministic | Stack would need direction, gap, padding, alignment, visibility, RTL, focus order, and Xt negotiation resources |
| Cross-axis grow/shrink and wrapping | No installed consumer demonstrates a missing requirement | Flex/Flow would need bounded basis/grow/shrink, wrapping, rounding, and a documented non-CSS subset |
| Explicit tracks and dense forms | `XmForm` and nested managers cover the qualified corpus | Grid would need track definitions, constraints, mutation, focus, accessibility, and a second geometry contract |
| User-adjustable split panes | `XmPanedWindow` already owns this policy | No candidate should duplicate sash semantics |
| Accessibility and reentrancy | WP-10 covers callback mutation and lifecycle; no new manager need is shown | Any future manager must expose stable child semantics and survive destruction during callbacks |
| RTL and keyboard traversal | Existing corpus and control fixtures provide baseline coverage | A new manager must specify logical direction and traversal before implementation |

## Independent decisions

* **Stack: no-go now.** The available corpus does not show a recurring stack
  requirement that `XmRowColumn` composition cannot express safely.
* **Flex/Flow: no-go now.** There is no measured client need for grow/shrink or
  wrapping, and adding a CSS-like vocabulary without a consumer would create
  an unsupported compatibility promise.
* **Grid: no-go now.** The corpus does not demonstrate a dense-track layout
  that cannot be composed from `XmForm`, `XmRowColumn`, and existing
  containers; a second negotiation contract would be speculative.

These are scope decisions, not claims that Motif layout APIs are pleasant or
that every application layout is easy.  They preserve existing geometry
negotiation and avoid adding three public managers without a reproducible
consumer requirement.

## Reopen criteria

Reopen an individual candidate only when an installed consumer supplies a
reproducible layout corpus showing a requirement that existing managers cannot
represent.  The corpus must include final geometry, resize storms, child
mutation, destruction/reentrancy, RTL direction, keyboard focus order,
accessibility semantics, Xt geometry negotiation, and comparative metrics
against the current managers.  A new package must begin with the smallest
public contract and an independent installed fixture; it must not grow from
private `XmForm` records or browser/CSS compatibility assumptions.
