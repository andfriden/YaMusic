#!/usr/bin/env python3

from pathlib import Path
import argparse
import re
import sys


# =============================================================
# Color mapping
# =============================================================

COLOR_MAP = {
    "#f5f5f5": "AppTheme.backgroundPrimary",

    "#ededed": "AppTheme.backgroundSecondary",
    "#e9e9e9": "AppTheme.panel",
    "#f1f1f1": "AppTheme.panelSecondary",

    "#dcdcdc": "AppTheme.panelActive",
    "#dddddd": "AppTheme.panelHover",
    "#e4e4e4": "AppTheme.panelHover",

    "#d0d0d0": "AppTheme.surface",

    "#d6d6d6": "AppTheme.borderSubtle",
    "#d7d7d7": "AppTheme.borderSubtle",
    "#dfdfdf": "AppTheme.border",
    "#cfcfcf": "AppTheme.border",
    "#c8c8c8": "AppTheme.borderStrong",
    "#cccccc": "AppTheme.borderStrong",
    "#d5d5d5": "AppTheme.divider",

    "#202020": "AppTheme.textPrimary",
    "#555555": "AppTheme.textSecondary",
    "#666666": "AppTheme.textSecondary",
    "#777777": "AppTheme.textMuted",
    "#888888": "AppTheme.textMuted",
    "#999999": "AppTheme.textDisabled",

    "#2468d7": "AppTheme.accent",
}


# =============================================================
# Regex
# =============================================================

HEX_COLOR_RE = re.compile(
    r'"(#[0-9a-fA-F]{6})"'
)


# =============================================================
# Find project root
# =============================================================

def find_project_root():
    """
    Find YaMusic project root by searching upwards
    for CMakeLists.txt containing project(YaMusic).
    """

    script_dir = Path(__file__).resolve().parent

    candidates = [
        script_dir,
        *script_dir.parents
    ]

    for directory in candidates:

        cmake_file = directory / "CMakeLists.txt"

        if not cmake_file.exists():
            continue

        try:
            content = cmake_file.read_text(
                encoding="utf-8"
            )
        except OSError:
            continue

        if re.search(
                r'\bproject\s*\(\s*YaMusic\b',
                content,
                re.IGNORECASE
        ):
            return directory

    print(
        "ERROR: Could not find YaMusic project root."
    )

    print()
    print(
        "The script searched upwards from:"
    )

    print(
        f"  {script_dir}"
    )

    print()

    print(
        "Make sure the script is located somewhere "
        "inside the YaMusic project."
    )

    sys.exit(1)


# =============================================================
# Helpers
# =============================================================

def normalize_color(color):
    return color.lower()


def find_qml_files(qml_root):

    if not qml_root.exists():

        print(
            "ERROR: QML directory not found:"
        )

        print(
            f"  {qml_root}"
        )

        sys.exit(1)

    return sorted(
        qml_root.rglob("*.qml")
    )


def replace_colors(text):

    changed = 0

    statistics = {}

    def replacement(match):

        nonlocal changed

        original = normalize_color(
            match.group(1)
        )

        if original not in COLOR_MAP:
            return match.group(0)

        target = COLOR_MAP[original]

        changed += 1

        key = (
            original,
            target
        )

        statistics[key] = (
                statistics.get(key, 0) + 1
        )

        return target

    result = HEX_COLOR_RE.sub(
        replacement,
        text
    )

    return (
        result,
        changed,
        statistics
    )


# =============================================================
# Main
# =============================================================

def main():

    parser = argparse.ArgumentParser(
        description=(
            "Migrate hardcoded QML colors "
            "to AppTheme."
        )
    )

    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually modify QML files."
    )

    args = parser.parse_args()

    # ---------------------------------------------------------
    # Locate project
    # ---------------------------------------------------------

    project_root = find_project_root()

    qml_root = project_root / "qml"

    qml_files = find_qml_files(
        qml_root
    )

    # ---------------------------------------------------------
    # Header
    # ---------------------------------------------------------

    print()

    print(
        "=" * 70
    )

    print(
        "YaMusic QML Color Migration"
    )

    print(
        "=" * 70
    )

    print()

    print(
        f"Project root:"
    )

    print(
        f"  {project_root}"
    )

    print()

    print(
        f"QML root:"
    )

    print(
        f"  {qml_root}"
    )

    print()

    print(
        f"QML files found: {len(qml_files)}"
    )

    print()

    # ---------------------------------------------------------
    # Statistics
    # ---------------------------------------------------------

    total_changes = 0

    total_unknown = {}

    modified_files = []

    # ---------------------------------------------------------
    # Process files
    # ---------------------------------------------------------

    for path in qml_files:

        # Never modify AppTheme itself.
        if path.name == "AppTheme.qml":
            continue

        try:

            text = path.read_text(
                encoding="utf-8"
            )

        except UnicodeDecodeError:

            print(
                f"WARNING: cannot read:"
            )

            print(
                f"  {path}"
            )

            continue

        # -----------------------------------------------------
        # Find unknown colors
        # -----------------------------------------------------

        for match in HEX_COLOR_RE.finditer(
                text
        ):

            color = normalize_color(
                match.group(1)
            )

            if color not in COLOR_MAP:

                total_unknown[color] = (
                        total_unknown.get(
                            color,
                            0
                        ) + 1
                )

        # -----------------------------------------------------
        # Replace known colors
        # -----------------------------------------------------

        new_text, changed, statistics = (
            replace_colors(text)
        )

        if changed == 0:
            continue

        relative = path.relative_to(
            project_root
        )

        print(
            relative
        )

        for (old, new), count in statistics.items():

            print(
                f"  {old:10} -> "
                f"{new:35} x{count}"
            )

        print()

        total_changes += changed

        modified_files.append(
            (
                path,
                new_text
            )
        )

    # ---------------------------------------------------------
    # Summary
    # ---------------------------------------------------------

    print(
        "=" * 70
    )

    print(
        "SUMMARY"
    )

    print(
        "=" * 70
    )

    print()

    print(
        f"Files to change:   {len(modified_files)}"
    )

    print(
        f"Color replacements: {total_changes}"
    )

    print()

    # ---------------------------------------------------------
    # Unknown colors
    # ---------------------------------------------------------

    if total_unknown:

        print(
            "=" * 70
        )

        print(
            "UNKNOWN COLORS"
        )

        print(
            "=" * 70
        )

        print()

        for color, count in sorted(
                total_unknown.items()
        ):

            print(
                f"  {color:10} x{count}"
            )

        print()

        print(
            "These colors were NOT changed."
        )

        print()

    else:

        print(
            "No unknown hardcoded colors found."
        )

        print()

    # ---------------------------------------------------------
    # Dry run
    # ---------------------------------------------------------

    if not args.apply:

        print(
            "=" * 70
        )

        print(
            "DRY RUN"
        )

        print(
            "=" * 70
        )

        print()

        print(
            "No files were modified."
        )

        print()

        if modified_files:

            print(
                "To apply the changes run:"
            )

            print()

            print(
                "  python3 "
                "tools/migrate_qml_colors.py "
                "--apply"
            )

            print()

        return

    # ---------------------------------------------------------
    # Apply
    # ---------------------------------------------------------

    print(
        "=" * 70
    )

    print(
        "APPLYING CHANGES"
    )

    print(
        "=" * 70
    )

    print()

    successful = 0

    for path, new_text in modified_files:

        try:

            path.write_text(
                new_text,
                encoding="utf-8"
            )

            print(
                f"UPDATED: "
                f"{path.relative_to(project_root)}"
            )

            successful += 1

        except OSError as error:

            print(
                f"ERROR: could not write:"
            )

            print(
                f"  {path}"
            )

            print(
                f"  {error}"
            )

    print()

    print(
        "=" * 70
    )

    print(
        "DONE"
    )

    print(
        "=" * 70
    )

    print()

    print(
        f"Updated files: {successful}"
    )

    print(
        f"Replacements:  {total_changes}"
    )

    print()


if __name__ == "__main__":
    main()