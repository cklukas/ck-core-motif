# PushButton menu unhighlight guards

Menu `XmPushButton` unhighlighting either fills the whole widget for etched-in
menus or clears the inner border.  The path previously assumed a background GC
and subtracted highlight margins even during small geometry transitions.

The fill and clear requests now validate their respective resources and
geometry.  Label redraw and disarm callbacks still run in the same order.
