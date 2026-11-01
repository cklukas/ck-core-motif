# Outline connector endpoints

With `XmNconnectNodes=True`, Outline branches meet the visible node borders.
Primitive focus margins sit outside the node face, so stopping at the X window
edge leaves a gap. Outline now extends each endpoint through that margin,
as Tree already does. Parent drawing remains clipped by children; only the
endpoint child's margin receives the extension. Active focus rings are preserved.

Incoming endpoints use the actual disclosure button geometry when present.
The outgoing ladder starts at the bottom of the corresponding node/disclosure
face. After child exposure, focus or configuration and parent redisplay, a
coalesced idle repaint restores endpoints after normal widget painting finishes.
Old gutter fragments are erased using the normal unhighlight surface contract.
Turning connections off removes endpoint fragments; destruction cancels pending
repaint work. Scheduling is private and does not change native widget records.

Modern profiles use 30 logical pixels of indentation in comfortable density,
or 24 in compact density, converted once for display scale. This leaves room
for scaled disclosure buttons. Explicit application arguments/X resources remain
authoritative, and Classic keeps its existing indentation policy.

Run `tests/automated/run-outline-connections.py --prefix PREFIX --out OUT` on
Linux or macOS/XQuartz with Xvfb installed. The native test uses a distinct
connector color to distinguish it from bevel pixels, and covers plain and
expandable nodes, both width policies, three profiles and three display scales.
It checks both endpoint gutters, window resizing, partial parent exposure,
child exposure, collapse/expand, focus-ring preservation, connection toggles
and destruction with repaint work pending. Tests run on an isolated display.
The [Mac qualification record](validation/macos-outline-20261101.json) records
source/output fingerprints and actual-window verification.
