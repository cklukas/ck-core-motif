#!/usr/bin/env python3
"""Verify typed custom-widget hooks and timer/geometry guards in dogs."""

from pathlib import Path


root = Path(__file__).resolve().parents[2] / "demos" / "unsupported" / "dogs"
dogs = (root / "dogs.c").read_text(encoding="utf-8")
dog = (root / "Dog.c").read_text(encoding="utf-8")
square = (root / "Square.c").read_text(encoding="utf-8")

assert "main(int argc, char **argv)" in dogs
assert "create_cb(Widget w, XtPointer client_data, XtPointer call_data)" in dogs
assert "tb_cb(Widget w, XtPointer client_data, XtPointer call_data)" in dogs
assert "scale_cb(Widget w, XtPointer client_data, XtPointer call_data)" in dogs
assert "!tag || !cb" in dogs
assert "!dog" in dogs
assert "cannot load dogs.uid" in dogs

assert "DogCreate(Widget parent, char *name, ArgList arglist, Cardinal nargs)" in dog
assert "Initialize(Widget request, Widget new_widget, ArgList args," in dog
assert "bark_dog(Widget widget, XEvent *event, String *params" in dog
assert "end_bark(XtPointer client_data, XtIntervalId *id)" in dog
assert "Width(w) > 2 * margin ? Width(w) - 2 * margin : 0" in dog
assert "Height(w) > 2 * margin ? Height(w) - 2 * margin : 0" in dog
assert "WagId(dw) = XtAppAddTimeOut" in dog
assert "WagId(dw) = 0;" in dog
assert "timeout_delay(int value)" in dog
assert "clamp_position(Dimension value)" in dog

assert "SquareCreate(Widget parent, char *name, ArgList arglist, Cardinal nargs)" in square
assert "ConstraintSetValues(Widget old_widget, Widget ref_widget, Widget new_widget," in square
assert "(gp.request_mode & CWWidth) != 0" in square
assert "(gp.request_mode & CWHeight) != 0" in square

print("dogs custom widgets use typed hooks and bounded timer/geometry state")
