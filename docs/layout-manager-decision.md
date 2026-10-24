# WP-12 layout-manager decision

WP-12 is an opt-in escape hatch, not a reason to replace `XmForm`.  The
existing managers were reviewed against the only measured layout corpus
available in this repository:

| Existing manager | Strength | Boundary relevant to this roadmap |
| --- | --- | --- |
| `XmForm` | Expresses anchored and proportional relationships; preserves established resource contracts | Attachment graphs are verbose, but WP-10/WP-11 found no reproducible redundant pass or correctness defect to fix |
| `XmRowColumn` | Simple one-dimensional stacking and menu/list composition | Does not express two-dimensional tracks or flexible cross-axis allocation |
| `XmPanedWindow` | User-adjustable split panes | Its sash model is not a general-purpose grid or flex contract |
| `XmScrolledWindow` | Viewport and scroll-bar ownership | It is a container/viewport, not a layout policy for arbitrary children |
| `XmBulletinBoard` / `XmManager` | Base geometry and child lifecycle behavior | Applications must provide positions or a specialized manager |

The WP-10 corpus demonstrates that nested Form layouts, dynamic child
lifecycle, callback mutation and resize storms complete deterministically on
the qualified AArch64 host.  WP-11 found no source-backed Form defect.  No
real client corpus in this checkout demonstrates a recurring layout need that
cannot be expressed by composing `XmForm`, `XmRowColumn`, and existing
containers.  A new grid/flex manager would therefore add a public class,
resource vocabulary, accessibility semantics, focus ordering, and another
geometry-negotiation implementation without a demonstrated consumer problem.

## Decision

**No-go for implementation in the current modernization tranche.**  Do not
add a speculative layout manager or a second declarative layout API.  Existing
Form clients remain unchanged, and the current managers continue to be the
supported composition tools.

Re-open WP-12 only when a real installed consumer supplies a reproducible
layout corpus showing a recurring requirement that cannot be represented by
those managers.  That corpus must include internationalized direction,
keyboard focus order, accessibility semantics, child destruction/reentrancy,
and Xt geometry negotiation before a new class is designed.  At that point the
design must begin with a small public contract and an independent installed
fixture; it must not grow out of private Form internals.

This is a product-scope decision, not a claim that Motif layout APIs are
pleasant or that every application layout is easy.  It records why adding
another manager now would be unjustified by the available evidence.
