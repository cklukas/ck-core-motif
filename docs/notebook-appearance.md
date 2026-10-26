# Notebook page connections and appearance

Notebook tabs represent page flaps. For the standard PushButton and
PushButtonGadget tab implementations, the edge facing the page is open in every
appearance profile. Classic tabs retain their bevel colors; modern tabs use
restrained, flat borders. Custom tabs implementing the JoinSide trait retain
control of their own drawing.

The selected tab and page form one outlined region. This also applies when
`frameShadowThickness` is zero: the historical two-rectangle drawing left a
closed seam. The region includes the complete tab extent, and its surface is
painted before its outline so underlying back-page lines cannot cross the join.
Inactive gadget tabs clear the full page-facing border band before drawing
their three exposed edges.

Modern light and dark defaults use the same surface color for the page frame,
footer and tab faces. Back-page outlines use a subdued border color rather than
black, and the small page stack remains visible. The frame has a one-logical-
pixel outline and comfortable/compact inner padding of eight/four logical
pixels. The decorative binding width defaults to zero in modern profiles.
High contrast uses black page surfaces and white outlines. Explicit application
resources remain authoritative, including frame color and inner margins.

An expose no longer forcibly unhighlights gadget tabs. Their existing redisplay
method preserves the focused state and redraws the focus indicator. Major and
minor tab-scroller visibility is counted independently.

## Tab placement

`backPagePlacement` names the corner showing the stacked page edges, not the
first tab's corner. With horizontal orientation, `XmBOTTOM_RIGHT` places major
tabs on the right, ordered from top to bottom; this is the normal Periodic demo
layout. `XmTOP_RIGHT` reverses that order, starting at the bottom. Both are
supported layouts and must maintain the same page connections. The page-number
navigator is a separate control, not a tab.

## Native validation

Run `tests/automated/run-notebook-appearance.py --prefix INSTALL --out FRESH_DIR`
against a coherent installed toolkit. The fixture checks actual X pixels for
open page-facing edges, outer tab edges and selected-page connections. It covers
major/minor tabs, widget/gadget tabs, all four placements, both orientations,
1×/2× scales, classic/light/dark/high-contrast profiles, zero-depth frames,
selection changes, live profile changes, focus preservation and explicit
resource overrides. The connection probes use inner padding so page-content
windows cannot obscure the Notebook pixels being measured.

The fixture reproduces a closed page-facing bevel in the prior library. Both
Xft/SVG and core-font/non-SVG configurations are included in qualification.
The isolated ABI 8 Xft/SVG patch also passes all 128 configurations, independently
of the development typography changes. The complete modern visual overhaul has
a separate acceptance matrix in
[Modern visual overhaul](modern-visual-overhaul.md).
