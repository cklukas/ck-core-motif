"""Exact, annotated demo excerpts shared by the application and documentation."""
from __future__ import annotations

from dataclasses import dataclass
import re
import textwrap


@dataclass(frozen=True)
class Excerpt:
    first: int
    last: int
    text: str


class GallerySource:
    def __init__(self, source: str):
        self.named = self._parse(source, "EXAMPLE")
        self.parts = self._parse(source, "SOURCE")

    @staticmethod
    def _parse(source: str, family: str) -> dict[str, Excerpt]:
        result = {}
        active = None
        lines = source.splitlines()
        pattern = re.compile(r"\s*/\* GALLERY-" + family +
                             r"-(BEGIN|END)(?:: (.*?))? \*/\s*")
        for number, line in enumerate(lines, 1):
            match = pattern.fullmatch(line)
            if not match:
                if "GALLERY-" + family + "-" in line:
                    raise ValueError(f"malformed {family} annotation at line {number}")
                continue
            action, key = match.groups()
            if action == "BEGIN":
                if active or not key:
                    raise ValueError(f"nested or unnamed {family} block at line {number}")
                active = (key, number + 1)
                continue
            if not active or (family == "EXAMPLE" and key != active[0]) or (
                    family == "SOURCE" and key is not None):
                raise ValueError(f"unmatched {family} end at line {number}")
            name, first = active
            last = number - 1
            while first <= last and not lines[first - 1].strip():
                first += 1
            while last >= first and not lines[last - 1].strip():
                last -= 1
            if first > last:
                raise ValueError(f"empty {family} block: {name}")
            excerpt = Excerpt(first, last,
                              textwrap.dedent("\n".join(lines[first - 1:last])))
            keys = [name]
            if family == "SOURCE":
                kind, *symbols = name.split()
                if kind not in ("setup", "construction", "states", "labels") or not symbols:
                    raise ValueError(f"invalid source selector: {name}")
                keys = [kind + " " + symbol for symbol in symbols]
            for item in keys:
                if item in result:
                    raise ValueError(f"duplicate {family} selector: {item}")
                result[item] = excerpt
            active = None
        if active:
            raise ValueError(f"unclosed {family} block: {active[0]}")
        return result

    def excerpts(self, entry: dict) -> list[Excerpt]:
        if not entry["constructible"]:
            return []
        symbol = entry["class_symbol"]
        special = ("segmented-control-construction" if symbol == "xmSegmentedControlWidgetClass"
                   else "drag-public-construction" if symbol == "xmDragOverShellWidgetClass"
                   else "shell-public-construction" if "Shell" in symbol else None)
        if special:
            return [self.named[special]]
        construction = self.parts.get("construction " + symbol)
        if construction is None:
            construction = self.parts["construction *"]
        setup = self.parts.get("setup " + symbol)
        return ([setup] if setup else []) + [construction] + [self.parts[kind + " " + symbol]
                                for kind in ("states", "labels")
                                if kind + " " + symbol in self.parts]

    @staticmethod
    def context(entry: dict) -> str:
        if not entry["constructible"]:
            return ("This is a service or abstract class, not a standalone control.\n"
                    "The Demo tab explains its role in the toolkit.")
        if "Shell" in entry["class_symbol"] or entry["class_symbol"] == "xmSegmentedControlWidgetClass":
            return (f"Actual gallery factory for {entry['class_symbol']}.\n"
                    "The gallery host supplies the function arguments.\n"
                    "Helpers are defined in gallery_examples.c.")
        return (f"Actual gallery excerpt: class = {entry['class_symbol']}.\n"
                "The host supplies parent, name, callback and client_data.\n"
                "Shared factory locals: Widget example; XmString label = NULL.\n"
                "The shared factory frees label, attaches callbacks and sizes the preview.\n"
                "Helpers and the complete factory are in gallery_examples.c.")

    def text(self, entry: dict) -> str:
        context = "\n".join(" * " + line for line in self.context(entry).splitlines())
        pieces = ["/*\n" + context + "\n */"]
        pieces.extend(part.text for part in self.excerpts(entry))
        pieces.append("/* Public header: " + entry["header"] + " */")
        return "\n\n".join(pieces)
