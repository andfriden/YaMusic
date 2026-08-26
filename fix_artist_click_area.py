#!/usr/bin/env python3

from pathlib import Path
import re
import sys


PROJECT_ROOT = Path(__file__).resolve().parent
QML_ROOT = PROJECT_ROOT / "qml"


def replace_artist_mouse_area(text: str, path: Path) -> tuple[str, int]:
    """
    Replace the artist click area width from the whole trackInfo
    to the actual artist label width.

    Only MouseArea blocks with:
        id: artistMouseArea
    are modified.
    """

    pattern = re.compile(
        r"""
        (
            MouseArea
            \s*\{
            .*?
            \bid\s*:\s*artistMouseArea\b
            .*?
        )
        (
            \bwidth\s*:\s*
            trackInfo\.width
        )
        """,
        re.DOTALL | re.VERBOSE,
        )

    def replacement(match: re.Match[str]) -> str:
        before = match.group(1)
        width_expression = match.group(2)

        # Already correct.
        if "artistLabel.width" in width_expression:
            return before + width_expression

        return before + "width:\n                                    artistLabel.width"

    new_text, count = pattern.subn(replacement, text)

    if count:
        print(f"[FIXED] {path} -> {count} artist click area(s)")

    return new_text, count


def process_file(path: Path) -> int:
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        print(f"[SKIP] {path}: {exc}")
        return 0

    new_text, count = replace_artist_mouse_area(text, path)

    if count:
        backup = path.with_suffix(path.suffix + ".bak")

        try:
            backup.write_text(text, encoding="utf-8")
            path.write_text(new_text, encoding="utf-8")
        except OSError as exc:
            print(f"[ERROR] {path}: {exc}")
            return 0

        print(f"        backup: {backup}")

    return count


def main() -> int:
    if not QML_ROOT.exists():
        print(f"[ERROR] QML directory not found: {QML_ROOT}")
        return 1

    total_files = 0
    total_changes = 0

    for path in sorted(QML_ROOT.rglob("*.qml")):
        changes = process_file(path)

        if changes:
            total_files += 1
            total_changes += changes

    print()
    print("========================================")
    print("Artist click area fix")
    print("========================================")
    print(f"Files changed: {total_files}")
    print(f"Areas changed: {total_changes}")
    print("========================================")

    return 0


if __name__ == "__main__":
    sys.exit(main())