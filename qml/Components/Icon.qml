import QtQuick
import QtQuick.Shapes

// Единый набор иконок плеера (взяты из yandex-music-client / mashiro).
// Каждый глиф — один или несколько SVG-path в координатах 24×24.
// filled-глифы (play/pause/next/prev/heartFilled/mini) рисуются заливкой,
// остальные — штрихом. Все path-ы глифа склеиваются в одну строку и
// отдаются единственному PathSvg, который понимает несколько M-команд.
Item {
    id: root

    // Имя глифа: play, pause, next, prev, shuffle, repeat, repeatOne,
    // chevronLeft, chevronRight, chevronDown, heart, heartFilled, heartOff,
    // lyrics, queue, expand, mini, close, note.
    property string glyph: ""
    property color color: AppTheme.textPrimary
    property real size: 20
    property real strokeWidth: 1.7

    readonly property bool filled:
        root.glyph === "play" || root.glyph === "pause" ||
        root.glyph === "next" || root.glyph === "prev" ||
        root.glyph === "heartFilled" || root.glyph === "mini"

    readonly property string pathData:
        (glyphMap[root.glyph] || []).join(" ")

    width: root.size
    height: root.size

    readonly property var glyphMap: ({
        play: ["M8.5 5.2 19 12 8.5 18.8z"],
        pause: ["M8.5 5h2.6v14H8.5zM12.9 5h2.6v14h-2.6z"],
        next: ["M5.5 5.8v12.4L15 12zM16.8 5h2.2v14h-2.2z"],
        prev: ["M18.5 5.8v12.4L9 12zM5 5h2.2v14H5z"],
        shuffle: ["M4 7h3.2L19 19 15.5 19H19v-3.5 M4 19h3.2l3.3-3.4 M14 9.8 17 7h2 M15.5 5v3.5H19"],
        repeat: ["M16.8 4.5 19.8 7.5l-3 3 M19.8 7.5H8.2A4.2 4.2 0 0 0 4 11.7v1.1 M7.2 19.5 4.2 16.5l3-3 M4.2 16.5h11.6a4.2 4.2 0 0 0 4.2-4.2v-1.1"],
        repeatOne: ["M16.8 4.5 19.8 7.5l-3 3 M19.8 7.5H8.2A4.2 4.2 0 0 0 4 11.7v1.1 M7.2 19.5 4.2 16.5l3-3 M4.2 16.5h11.6a4.2 4.2 0 0 0 4.2-4.2v-1.1 M11 10.6 12.6 9.6v4.8"],
        chevronLeft: ["M14.5 5 8 12l6.5 7"],
        chevronRight: ["M9.5 5 16 12l-6.5 7"],
        chevronDown: ["M5 9l7 7 7-7"],
        heart: ["M12 20c-1.2-.9-7.5-5-7.5-9.6A3.9 3.9 0 0 1 12 7.8a3.9 3.9 0 0 1 7.5 2.6c0 4.6-6.3 8.7-7.5 9.6z"],
        heartFilled: ["M12 20.2c-1.3-.9-7.7-5.2-7.7-9.8A4.1 4.1 0 0 1 12 7.6a4.1 4.1 0 0 1 7.7 2.8c0 4.6-6.4 8.9-7.7 9.8z"],
        heartOff: ["M12 20c-1.2-.9-7.5-5-7.5-9.6A3.9 3.9 0 0 1 12 7.8a3.9 3.9 0 0 1 7.5 2.6c0 4.6-6.3 8.7-7.5 9.6z M4 3.5 20 20.5"],
        lyrics: ["M4 5.6h12 M10 5.6v10.3 M13.8 10h6.2 M13.8 14.1h6.2 M8.4 18.4h11.6"],
        queue: ["M4 7h11 M4 12h11 M4 17h7 M17.5 17.5a2 2 0 1 0 4 0V7l-4 1.2z"],
        expand: ["M4 9.5V4h5.5 M20 14.5V20h-5.5 M20 9.5V4h-5.5 M4 14.5V20h5.5 M4 4l6 6 M20 20l-6-6 M20 4l-6 6 M4 20l6-6"],
        mini: ["M4.6 5.6h14.8v12.8H4.6z M12.4 12.4h6v5h-6z"],
        close: ["M6.4 6.4 17.6 17.6 M17.6 6.4 6.4 17.6"],
        note: ["M9.5 17.5V6.4L18 4.5v11.1 M7 20a2.5 2.5 0 1 0 2.5-2.5 M15.5 18.1a2.5 2.5 0 1 0 2.5-2.5"],
        artist: ["M12 14.2a3.1 3.1 0 0 0 3.1-3.1V6.6a3.1 3.1 0 0 0-6.2 0v4.5a3.1 3.1 0 0 0 3.1 3.1z", "M6.8 11.4a5.2 5.2 0 0 0 10.4 0", "M12 17.4v3"]
    })

    Shape {
        anchors.centerIn: parent

        width: 24
        height: 24

        antialiasing: true
        smooth: true

        transform: Scale {
            origin.x: 12
            origin.y: 12
            xScale: root.width / 24
            yScale: root.height / 24
        }

        ShapePath {
            strokeWidth: root.strokeWidth
            joinStyle: ShapePath.RoundJoin
            capStyle: ShapePath.RoundCap

            strokeColor: root.filled ? "transparent" : root.color
            fillColor: root.filled ? root.color : "transparent"

            PathSvg { path: root.pathData }
        }
    }
}