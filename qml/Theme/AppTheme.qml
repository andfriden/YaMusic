pragma Singleton

import QtQuick

QtObject {
    id: theme

    /*
     * ============================================================
     * Theme mode
     * ============================================================
     *
     * false = Latte
     * true  = Macchiato
     */

    property bool dark: false


    /*
     * ============================================================
     * Catppuccin Latte
     * ============================================================
     */

    readonly property color latteRosewater: "#dc8a78"
    readonly property color latteFlamingo:  "#dd7878"
    readonly property color lattePink:      "#ea76cb"
    readonly property color latteMauve:     "#8839ef"
    readonly property color latteRed:       "#d20f39"
    readonly property color latteMaroon:    "#e64553"
    readonly property color lattePeach:     "#fe640b"
    readonly property color latteYellow:    "#df8e1d"
    readonly property color latteGreen:     "#40a02b"
    readonly property color latteTeal:      "#179299"
    readonly property color latteSky:       "#04a5e5"
    readonly property color latteSapphire:  "#209fb5"
    readonly property color latteBlue:      "#1e66f5"
    readonly property color latteLavender:  "#7287fd"

    readonly property color latteText:      "#4c4f69"
    readonly property color latteSubtext1:  "#5c5f77"
    readonly property color latteSubtext0:  "#6c6f85"
    readonly property color latteOverlay2:  "#7c7f93"
    readonly property color latteOverlay1:  "#8c8fa1"
    readonly property color latteOverlay0:  "#9ca0b0"

    readonly property color latteSurface2:  "#acb0be"
    readonly property color latteSurface1:  "#bcc0cc"
    readonly property color latteSurface0:  "#ccd0da"

    readonly property color latteBase:      "#eff1f5"
    readonly property color latteMantle:    "#e6e9ef"
    readonly property color latteCrust:     "#dce0e8"


    /*
     * ============================================================
     * Catppuccin Macchiato
     * ============================================================
     */

    readonly property color macchiatoRosewater: "#f4dbd6"
    readonly property color macchiatoFlamingo:  "#f0c6c6"
    readonly property color macchiatoPink:      "#f5bde6"
    readonly property color macchiatoMauve:     "#c6a0f6"
    readonly property color macchiatoRed:       "#ed8796"
    readonly property color macchiatoMaroon:    "#ee99a0"
    readonly property color macchiatoPeach:     "#f5a97f"
    readonly property color macchiatoYellow:    "#eed49f"
    readonly property color macchiatoGreen:     "#a6da95"
    readonly property color macchiatoTeal:      "#8bd5ca"
    readonly property color macchiatoSky:       "#91d7e3"
    readonly property color macchiatoSapphire:  "#7dc4e4"
    readonly property color macchiatoBlue:      "#8aadf4"
    readonly property color macchiatoLavender:  "#b7bdf8"

    readonly property color macchiatoText:      "#cad3f5"
    readonly property color macchiatoSubtext1:  "#b8c0e0"
    readonly property color macchiatoSubtext0:  "#a5adcb"
    readonly property color macchiatoOverlay2:  "#939ab7"
    readonly property color macchiatoOverlay1:  "#8087a2"
    readonly property color macchiatoOverlay0:  "#6e738d"

    readonly property color macchiatoSurface2:  "#5b6078"
    readonly property color macchiatoSurface1:  "#494d64"
    readonly property color macchiatoSurface0:  "#363a4f"

    readonly property color macchiatoBase:      "#24273a"
    readonly property color macchiatoMantle:    "#1e2030"
    readonly property color macchiatoCrust:     "#181926"


    /*
     * ============================================================
     * Background
     * ============================================================
     */

    readonly property color backgroundPrimary:
        dark
            ? macchiatoBase
            : latteBase

    readonly property color background:
        backgroundPrimary

    readonly property color backgroundSecondary:
        dark
            ? macchiatoMantle
            : latteMantle

    readonly property color backgroundTertiary:
        dark
            ? macchiatoCrust
            : latteCrust


    /*
     * ============================================================
     * Panels
     * ============================================================
     *
     * Все цвета строятся только из базовой Catppuccin-палитры.
     */

    readonly property color panel:
        dark
            ? macchiatoSurface0
            : latteMantle

    readonly property color panelSecondary:
        dark
            ? macchiatoSurface0
            : latteBase

    readonly property color panelHover:
        dark
            ? macchiatoSurface1
            : latteSurface0

    readonly property color panelActive:
        dark
            ? macchiatoSurface2
            : latteSurface1

    readonly property color surface:
        dark
            ? macchiatoSurface0
            : latteSurface0


    /*
     * ============================================================
     * Borders
     * ============================================================
     */

    readonly property color border:
        dark
            ? macchiatoSurface2
            : latteSurface2

    readonly property color borderSubtle:
        dark
            ? macchiatoSurface1
            : latteSurface0

    readonly property color borderStrong:
        dark
            ? macchiatoSurface2
            : latteSurface1


    /*
     * ============================================================
     * Text
     * ============================================================
     */

    readonly property color textPrimary:
        dark
            ? macchiatoText
            : latteText

    readonly property color textSecondary:
        dark
            ? macchiatoSubtext1
            : latteSubtext1

    readonly property color textTertiary:
        dark
            ? macchiatoSubtext0
            : latteSubtext0

    readonly property color textMuted:
        dark
            ? macchiatoOverlay1
            : latteOverlay1

    readonly property color textDisabled:
        dark
            ? macchiatoOverlay0
            : latteOverlay0


    /*
     * ============================================================
     * Placeholder
     * ============================================================
     */

    readonly property color placeholder:
        dark
            ? macchiatoSurface1
            : latteSurface1

    readonly property color artworkPlaceholder:
        placeholder


    /*
     * ============================================================
     * Accent
     * ============================================================
     */

    readonly property color accent:
        dark
            ? macchiatoBlue
            : latteBlue

    readonly property color accentHover:
        dark
            ? macchiatoLavender
            : latteLavender

    readonly property color accentSoft:
        dark
            ? macchiatoSurface0
            : latteSurface0


    /*
     * ============================================================
     * Status
     * ============================================================
     */

    readonly property color success:
        dark
            ? macchiatoGreen
            : latteGreen

    readonly property color warning:
        dark
            ? macchiatoYellow
            : latteYellow

    readonly property color error:
        dark
            ? macchiatoRed
            : latteRed

    readonly property color info:
        dark
            ? macchiatoBlue
            : latteBlue


    /*
     * ============================================================
     * Divider
     * ============================================================
     */

    readonly property color divider:
        dark
            ? macchiatoSurface1
            : latteSurface0
}