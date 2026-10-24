import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller

    // Страница занимает всю высоту области просмотра, а не свою контентную.
    readonly property bool fillHeight: true

    width: parent ? parent.width : 0
    height: parent ? parent.height : 700
    implicitWidth: width
    implicitHeight: height

    readonly property var stats:
        root.controller !== null && root.controller !== undefined &&
        root.controller.statsController !== undefined
            ? root.controller.statsController
            : null

    Flickable {
        id: pageFlickable

        anchors.fill: parent

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            id: contentColumn

            width: pageFlickable.width - 40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20

            spacing: 28

            Row {
                width: parent.width
                spacing: 12

                Label {
                    width: parent.width - resetButton.width - parent.spacing

                    text: "Статистика прослушивания"
                    color: AppTheme.textPrimary
                    font.pixelSize: 28
                    font.bold: true

                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap
                }

                Rectangle {
                    id: resetButton

                    width: resetLabel.implicitWidth + 24
                    height: 32

                    anchors.verticalCenter: parent.verticalCenter

                    radius: 8
                    color: resetMouse.containsMouse ? AppTheme.panelActive : AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    Label {
                        id: resetLabel

                        anchors.centerIn: parent

                        text: "Сбросить"
                        color: resetMouse.containsMouse ? AppTheme.error : AppTheme.textSecondary
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: resetMouse

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.stats) {
                                root.stats.reset()
                            }
                        }
                    }
                }
            }

            // Сводка.
            Rectangle {
                width: parent.width
                height: 100

                radius: 12
                color: AppTheme.panel
                border.width: 1
                border.color: AppTheme.borderSubtle

                Row {
                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 8

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.totalPlays : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Прослушиваний"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? AppTheme.formatTime(root.stats.totalPlayTimeMs) : "0:00"
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Времени в музыке"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.trackCount : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Треков"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.artistCount : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Исполнителей"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                }
            }

            // Минуты по дням.
            Column {
                width: parent.width
                spacing: 12

                RowLayout {
                    width: parent.width
                    spacing: 8

                    Label {
                        text: "Минуты по дням"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Label {
                        id: chartSummaryLabel

                        text: chartSummary()
                        color: AppTheme.textSecondary
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignRight

                        function chartSummary() {
                            if (!root.stats || !chartCard.hasData) return "за 30 дней"
                            return "пик " + chartCard.maxMinutes + " мин · в среднем " +
                                chartCard.avgMinutes() + " мин"
                        }
                    }
                }

                Rectangle {
                    id: chartCard

                    width: parent.width
                    height: 190

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    // Актуальные данные по дням (пересчитываются на смене темы).
                    readonly property var chartData: {
                        root.stats
                        AppTheme.dark
                        return root.stats ? root.stats.dailyMinutes(30) : []
                    }

                    // Максимум минут — для масштаба столбиков.
                    readonly property int maxMinutes: {
                        let m = 0
                        for (const row of chartCard.chartData) {
                            m = Math.max(m, Number(row.minutes))
                        }
                        return m
                    }

                    // Пусто, если за период ничего не слушали.
                    readonly property bool hasData: {
                        for (const row of chartCard.chartData) {
                            if (Number(row.minutes) > 0) return true
                        }
                        return false
                    }

                    // Сколько дней из периода было непусто.
                    function activeDays() {
                        let n = 0
                        for (const row of chartCard.chartData) {
                            if (Number(row.minutes) > 0) n++
                        }
                        return n
                    }

                    // Среднее минут в день за весь период (в том числе пустые дни).
                    function avgMinutes() {
                        const data = chartCard.chartData
                        if (data.length === 0) return 0
                        let sum = 0
                        for (const row of data) {
                            sum += Number(row.minutes)
                        }
                        return Math.round(sum / data.length)
                    }

                    // Область графика: шкала минут слева + столбики, подписи дат снизу.
                    Column {
                        id: chartArea

                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        anchors.topMargin: 24
                        anchors.bottomMargin: 6

                        spacing: 2

                        // Верх: шкала + столбики.
                        Row {
                            width: parent.width
                            height: parent.height - 14

                            spacing: 6

                            // Вертикальная шкала минут: 0 внизу, max наверху.
                            Item {
                                id: yAxis

                                width: 30
                                height: parent.height

                                Repeater {
                                    model: 3

                                    delegate: Item {
                                        required property int index

                                        anchors.fill: parent

                                        readonly property real lineY:
                                            yAxis.height / 2 * index

                                        // Линия сетки.
                                        Rectangle {
                                            anchors.left: parent.left
                                            anchors.right: parent.right

                                            height: 1
                                            y: parent.height - lineY - 1

                                            color: AppTheme.borderSubtle
                                            opacity: 0.85
                                        }

                                        // Подпись значения.
                                        Label {
                                            anchors.right: parent.right

                                            y: parent.height - lineY - 6

                                            text: {
                                                const max = chartCard.maxMinutes
                                                const steps = [0, Math.ceil(max / 2), max]
                                                return steps.length > index ? steps[index] : 0
                                            }
                                            color: AppTheme.textMuted
                                            font.pixelSize: 9
                                        }
                                    }
                                }
                            }

                            // Линейный график (плавная линия с заполнением).
                            Canvas {
                                id: lineChart

                                width: parent.width - 30 - parent.spacing
                                height: parent.height

                                antialiasing: true

                                onPaint: lineChart.drawChart()

                                onWidthChanged: lineChart.requestPaint()
                                onHeightChanged: lineChart.requestPaint()

                                Connections {
                                    target: root.stats
                                    function onStatsChanged() {
                                        lineChart.requestPaint()
                                    }
                                }

                                function drawChart() {
                                    const ctx = lineChart.getContext("2d")
                                    ctx.reset()
                                    lineChart.drawLine(ctx)
                                }

                                function drawLine(ctx) {
                                    const w = lineChart.width
                                    const h = lineChart.height
                                    const data = chartCard.chartData
                                    const n = data.length
                                    if (n <= 0) return

                                    // Максимум для масштаба.
                                    let max = chartCard.maxMinutes
                                    if (max <= 0) max = 1

                                    const padX = 2
                                    const padTop = 4
                                    const padBottom = 4
                                    const plotH = h - padTop - padBottom

                                    const stepX = n > 1
                                        ? (w - padX * 2) / (n - 1)
                                        : 0

                                    // Точки (x, y).
                                    const pts = []
                                    for (let i = 0; i < n; i++) {
                                        const v = Number(data[i].minutes)
                                        const x = padX + (n > 1 ? i * stepX : w / 2)
                                        const ratio = Math.max(0, Math.min(1, v / max))
                                        const y = h - padBottom - plotH * ratio
                                        pts.push({ x: x, y: y })
                                    }

                                    // Сглаживание через кривые Безье.
                                    const first = pts[0]
                                    const last = pts[n - 1]
                                    const pathPts = lineChart.buildSmooth(pts)

                                    // Заполнение: путь + закрытие до низа.
                                    ctx.beginPath()
                                    ctx.moveTo(first.x, first.y)
                                    for (const t of pathPts) {
                                        ctx.bezierCurveTo(t.c1x, t.c1y, t.c2x, t.c2y, t.x, t.y)
                                    }
                                    ctx.lineTo(last.x, h - padBottom)
                                    ctx.lineTo(first.x, h - padBottom)
                                    ctx.closePath()

                                    const fillGrad = ctx.createLinearGradient(0, padTop, 0, h)
                                    fillGrad.addColorStop(0, Qt.rgba(
                                        AppTheme.accent.r, AppTheme.accent.g,
                                        AppTheme.accent.b, 0.35))
                                    fillGrad.addColorStop(1, Qt.rgba(
                                        AppTheme.accent.r, AppTheme.accent.g,
                                        AppTheme.accent.b, 0.05))
                                    ctx.fillStyle = fillGrad
                                    ctx.fill()

                                    // Сама линия поверх.
                                    ctx.beginPath()
                                    ctx.moveTo(first.x, first.y)
                                    for (const t of pathPts) {
                                        ctx.bezierCurveTo(t.c1x, t.c1y, t.c2x, t.c2y, t.x, t.y)
                                    }
                                    ctx.strokeStyle = Qt.rgba(
                                        AppTheme.accent.r, AppTheme.accent.g,
                                        AppTheme.accent.b, 1)
                                    ctx.lineWidth = 2
                                    ctx.lineJoin = "round"
                                    ctx.lineCap = "round"
                                    ctx.stroke()
                                }

                                // Строит плавные сегменты Безье между точками:
                                // возвращает массив {x,y,c1x,c1y,c2x,c2y}.
                                function buildSmooth(pts) {
                                    const out = []
                                    const n = pts.length
                                    if (n === 0) return out
                                    out.push({
                                        x: pts[0].x, y: pts[0].y,
                                        c1x: pts[0].x, c1y: pts[0].y,
                                        c2x: pts[0].x, c2y: pts[0].y
                                    })
                                    for (let i = 1; i < n; i++) {
                                        const p0 = pts[i - 1]
                                        const p1 = pts[i]
                                        const c1x = p0.x + (p1.x - p0.x) * 0.5
                                        const c2x = p0.x + (p1.x - p0.x) * 0.5
                                        out.push({
                                            x: p1.x, y: p1.y,
                                            c1x: c1x, c1y: p0.y,
                                            c2x: c2x, c2y: p1.y
                                        })
                                    }
                                    return out
                                }
                            }
                        }

                        // Низ: подписи дат.
                        Row {
                            width: parent.width - 36
                            x: 36 // сдвиг под шкалу минут
                            height: 12

                            spacing: 2

                            Repeater {
                                id: dateRepeater
                                model: chartCard.chartData

                                delegate: Item {
                                    required property int index
                                    required property var modelData

                                    readonly property string label:
                                        String(modelData.label || "")

                                    width: dateWidth()
                                    height: parent.height

                                    readonly property int total: dateRepeater.count

                                    function dateWidth() {
                                        const n = dateRepeater.count
                                        if (n <= 0) return 0
                                        return (parent.width - 2 * (n - 1)) / n
                                    }

                                    Label {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        anchors.verticalCenter: parent.verticalCenter

                                        text: index % 5 === 0 ? label : ""
                                        color: AppTheme.textMuted
                                        font.pixelSize: 9
                                    }
                                }
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет данных за этот период"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: !chartCard.hasData
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 16

                Column {
                    width: parent.width / 2 - parent.spacing / 2
                    spacing: 12

                    Label {
                        width: parent.width

                        text: "Топ треков"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Rectangle {
                        width: parent.width

                    height: root.stats && root.stats.topTracks(20).length > 0
                        ? Math.min(root.stats.topTracks(20).length * 56 + 20, 10 * 56 + 20)
                        : 100

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    ListView {
                        id: topTracksView

                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 6
                        clip: true

                        interactive: topTracksView.count > 8
                        boundsBehavior: Flickable.StopAtBounds

                        model: {
                            root.stats
                            AppTheme.dark
                            return root.stats ? root.stats.topTracks(20) : []
                        }

                        delegate: Item {
                            required property int index
                            required property string id
                            required property string title
                            required property string artist
                            required property string coverUri
                            required property int plays

                            width: topTracksView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: rowArea.containsMouse
                                    ? AppTheme.panelActive
                                    : "transparent"

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 9
                                anchors.rightMargin: 9

                                spacing: 11

                                Label {
                                    Layout.preferredWidth: 20
                                    Layout.alignment: Qt.AlignVCenter

                                    text: index + 1
                                    color: index < 3 ? AppTheme.accent : AppTheme.textMuted
                                    font.pixelSize: 12
                                    font.bold: index < 3
                                                                        horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Rectangle {
                                    Layout.preferredWidth: 36
                                    Layout.preferredHeight: 36
                                    Layout.alignment: Qt.AlignVCenter

                                    radius: 18
                                    color: AppTheme.artworkPlaceholder
                                    clip: true

                                    Image {
                                        id: rowCover
                                        anchors.fill: parent
                                        source: coverUri.length > 0
                                            ? "image://yandex/circle/" + coverUri
                                            : ""
                                        sourceSize: Qt.size(72, 72)
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                        cache: true
                                        smooth: true
                                        visible: status === Image.Ready
                                    }

                                    Icon {
                                        anchors.centerIn: parent
                                        glyph: "note"
                                        color: AppTheme.textMuted
                                        size: 16
                                        strokeWidth: 1.6
                                        visible: rowCover.status !== Image.Ready
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    spacing: 3

                                    Label {
                                        Layout.fillWidth: true
                                        text: title
                                        color: AppTheme.textPrimary
                                        font.pixelSize: 13
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        text: artist
                                        color: AppTheme.textSecondary
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }
                                }

                                Label {
                                    Layout.alignment: Qt.AlignVCenter

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                                                        verticalAlignment: Text.AlignVCenter
                                }
                            }

                            MouseArea {
                                id: rowArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет данных — послушайте несколько треков"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.stats === null ||
                            root.stats.topTracks(20).length === 0
                    }
                }
            }

            Column {
                    width: parent.width / 2 - parent.spacing / 2
                    spacing: 12

                    Label {
                        width: parent.width

                        text: "Топ исполнителей"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Rectangle {
                    width: parent.width

                    height: root.stats && root.stats.topArtists(20).length > 0
                        ? Math.min(root.stats.topArtists(20).length * 56 + 20, 10 * 56 + 20)
                        : 100

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    ListView {
                        id: topArtistsView

                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 6
                        clip: true

                        interactive: topArtistsView.count > 8
                        boundsBehavior: Flickable.StopAtBounds

                        model: {
                            root.stats
                            AppTheme.dark
                            return root.stats ? root.stats.topArtists(20) : []
                        }

                        delegate: Item {
                            required property int index
                            required property string id
                            required property string name
                            required property string coverUri
                            required property int plays

                            width: topArtistsView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: artistRowArea.containsMouse
                                    ? AppTheme.panelActive
                                    : "transparent"

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 9
                                anchors.rightMargin: 9

                                spacing: 11

                                Label {
                                    Layout.preferredWidth: 20
                                    Layout.alignment: Qt.AlignVCenter

                                    text: index + 1
                                    color: index < 3 ? AppTheme.accent : AppTheme.textMuted
                                    font.pixelSize: 12
                                    font.bold: index < 3
                                                                        horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Rectangle {
                                    Layout.preferredWidth: 36
                                    Layout.preferredHeight: 36
                                    Layout.alignment: Qt.AlignVCenter

                                    radius: 18
                                    color: AppTheme.artworkPlaceholder
                                    clip: true

                                    Image {
                                        id: artistAvatar
                                        anchors.fill: parent
                                        source: coverUri.length > 0
                                            ? "image://yandex/circle/" + coverUri
                                            : ""
                                        sourceSize: Qt.size(72, 72)
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                        cache: true
                                        smooth: true
                                        visible: status === Image.Ready
                                    }

                                    Icon {
                                        anchors.centerIn: parent
                                        glyph: "artist"
                                        color: AppTheme.textMuted
                                        size: 16
                                        strokeWidth: 1.6
                                        visible: artistAvatar.status !== Image.Ready
                                    }
                                }

                                Label {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    text: name
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 13
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                }

                                Label {
                                    Layout.alignment: Qt.AlignVCenter

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                                                        verticalAlignment: Text.AlignVCenter
                                }
                            }

                            MouseArea {
                                id: artistRowArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет данных — послушайте несколько треков"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.stats === null ||
                            root.stats.topArtists(20).length === 0
                    }
                }
            }
            }

            Item {
                width: 1
                height: 20
            }
        }
    }
}