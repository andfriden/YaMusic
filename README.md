<div align="center">

# 🎧 YaMusic

**Независимый desktop-клиент Яндекс Музыки**

Построен на C++17 · Qt 6 · QML · macOS, Linux и Windows

[![License](https://img.shields.io/github/license/andfriden/YaMusic)](https://github.com/andfriden/YaMusic/blob/main/LICENSE)
[![Release](https://img.shields.io/github/v/tag/andfriden/YaMusic)](https://github.com/andfriden/YaMusic/releases)

</div>

---

![YaMusic](screenshots/player.png)

---

## Возможности

* OAuth-авторизация с сохранением между запусками
* Поиск, «Моя волна», персональные рекомендации и плейлисты
* Свои плейлисты: создание, переименование, удаление, добавление и удаление треков
* Лайки треков, альбомов и исполнителей, недавно прослушанные
* Чарты России и мира, жанры, новые релизы
* Раздел «Станции»: полный список станций ротора с группировкой по типам
* Раздел «Подкасты»: витрина подкастов из лендинга
* Страницы альбомов, плейлистов, исполнителей и жанров
* Радио исполнителя: все треки артиста, затем цепочка похожих без повторов
* Единая очередь: repeat, shuffle, автопереход на похожий контент и продолжение альбома
* Подсветка текущего трека
* Mini Player, Expanded Now Playing, перемотка, громкость, Dynamic Player Accent
* Текст песен (🎤) с синхронизированной подсветкой строк, фолбэк на публичную базу LRCLIB
* Выбор качества аудио (низкое / среднее / высокое / lossless) и показ кодека с битрейтом
* Треки без цензуры — подмена на версии без цензуры по открытой базе FckCensorData
* Блок «Колесо» на главной: подборки по вашим трекам и альбомам
* Дорожки без прав на прослушивание помечаются в списке и уводят во внешний сервис вместо ошибки
* Скачивание трека в «Загрузки» с метаданными и вложенной обложкой (↓)
* Отчёты о падениях: перехват сигналов и `terminate`, файл отчёта на диске и отправка в трекер из окна
* Тёмная тема Catppuccin (Latte / Macchiato), попап настроек
* Адаптивный интерфейс: верхняя навигация, боковая и контекстная панели

Полная история изменений по версиям — в [CHANGELOG.md](CHANGELOG.md).

---

## История прослушивания

YaMusic подключается к Яндекс Музыке по протоколу **Ynison** (долгоживущее WebSocket-соединение):
регистрирует устройство-плеер, транслирует «сейчас играет» и состояние очереди, отправляет
heartbeat прогресса и события завершения треков. Раздел «Недавно прослушаны» в приложении
читается через `/music-history`.

Ограничение со стороны сервера: запись треков в вашу историю на стороне Яндекс Музыки
(чтобы прослушанное появлялось в истории и в официальном приложении) выполняется только для
сессий, инициированных официальными клиентами. Ни `/play-audio`, ни `/plays`, ни `/queues`,
ни Ynison-статусы от неофициального клиента сервер в историю не засчитывает — события
принимаются (200/ok), но не записываются. Чтобы прослушанное в YaMusic попало в общую
историю/рекомендации Яндекса, используйте официальное приложение.

Использованная документация: [MarshalX/yandex-music-api](https://github.com/MarshalX/yandex-music-api),
[acherkashin/yandex-music-open-api](https://github.com/acherkashin/yandex-music-open-api),
[trudenboy/ma-provider-yandex-ynison](https://github.com/trudenboy/ma-provider-yandex-ynison).

---

## Установка

Готовые пакеты содержат Qt runtime — отдельная установка Qt не требуется.
Пакеты собираются автоматически в GitHub Actions при пуше тега `v*` (см. `.github/workflows/build.yml`).

| ОС | Архитектура | Пакет |
|---|---|---|
| Windows x64 | portable | `YaMusic-<version>-windows-x64-portable.zip` |
| macOS | Intel | `YaMusic-<version>-macos-x64.dmg` |
| macOS | Apple Silicon | `YaMusic-<version>-macos-arm64.dmg` |
| Linux x86_64 | AppImage | `YaMusic-<version>-x86_64.AppImage` |
| Linux x86_64 | архив | `YaMusic-<version>-x86_64.tar.gz` |

macOS-пакеты собираются отдельно для Intel (x64) и Apple Silicon (arm64);
Windows — portable-версия; Linux — AppImage и архив.

---

## Авторизация

1. Получите OAuth-ссылку в приложении и откройте её в браузере.
2. Авторизуйтесь в аккаунте Яндекса и разрешите доступ.
3. Скопируйте ссылку из адресной строки браузера и вставьте обратно в YaMusic.

Ссылка содержит данные доступа к аккаунту — не публикуйте её (GitHub, код, скриншоты, логи).

---

## Сборка из исходников

**Зависимости:** C++17, Qt 6 (Base, Quick, Multimedia), CMake, Ninja. `CMakeLists.txt` — источник истины.

**Ubuntu:**

```bash
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-declarative-dev qt6-multimedia-dev
```

**Arch Linux:**

```bash
sudo pacman -S --needed base-devel cmake ninja qt6-base qt6-declarative qt6-multimedia
```

**macOS:** Xcode / Apple Clang, Qt 6, CMake, Ninja.
**Windows:** Visual Studio 2022 (C++ workload), Qt 6, CMake, Ninja.

Общие шаги для всех ОС:

```bash
git clone https://github.com/andfriden/YaMusic.git
cd YaMusic
cmake -S . -B build
cmake --build build --parallel
```

Запуск: `./build/appYaMusic` (Linux), `./build/appYaMusic.app/Contents/MacOS/appYaMusic` (macOS), `.\build\Debug\appYaMusic.exe` (Windows, сборка с `--config Debug`).

Установка в отдельный каталог: `cmake --install build --prefix dist`.

### Релизные пакеты

Пакеты собираются автоматически в GitHub Actions при пуше тега `v*` (см. `.github/workflows/build.yml`):

* **Windows x64** — `YaMusic-<version>-windows-x64-portable.zip`
* **macOS** — отдельно для Intel и Apple Silicon: `YaMusic-<version>-macos-x64.dmg`, `YaMusic-<version>-macos-arm64.dmg`
* **Linux x86_64** — `YaMusic-<version>-x86_64.AppImage` и `YaMusic-<version>-x86_64.tar.gz`

AppImage собирается скриптом `scripts/build-appimage.sh` (скачивает linuxdeploy-инструменты в `.tools` и не требует FUSE):

```bash
./scripts/build-appimage.sh --full
```

---

## Архитектура

```text
QML → Controllers → Services / Models → (Yandex Music API, Playback)
```

C++ отвечает за бизнес-логику, API, авторизацию, модели, состояние приложения и воспроизведение; QML — за интерфейс, страницы и навигацию.

Контроллеры: `AppController`, `AccentController`, `AlbumController`, `ArtistController`, `AudioQualityController`, `ChartController`, `CrashReporter`, `GenreController`, `LibraryController`, `LyricsController`, `PersonalController`, `PlaybackController`, `SearchController`, `StationsController`, `ThemeController`.

Воспроизведение построено на Qt Multimedia: `PlaybackController` связывает состояние с приложением, `PlayerService` управляет `QMediaPlayer`, `QueueService` — очередью.

---

## Структура проекта

```text
YaMusic/
├── src/        # Core, Crash, Models, Playback, Player, Queue, Yandex
├── qml/        # Components, Home, Layout, MyWave, Pages, Search, Theme
├── data/       # genre_playlists.csv
├── CMakeLists.txt
└── main.cpp
```

---

## Roadmap

| Версия | Содержание |
|---|---|
| v0.1 | Технический POC: API, авторизация, базовое воспроизведение |
| v0.2 | Поиск, результаты, страницы альбомов и исполнителей |
| v0.3 | «Моя волна», рекомендации, недавно прослушанные |
| v0.4 | Умная очередь, repeat, shuffle |
| v0.5 | Плейлисты, библиотека |
| v0.6 | Лайки |
| v0.7 | Mini Player, Expanded Now Playing, перемотка, громкость |
| v0.8 | Чарты, жанры, новые релизы, переключение тем ✅ |
| v0.9 | Текст песен (Lyrics) ✅ |
| v0.10 | Системные медиа-кнопки: MPRIS (Linux), SMTC (Windows), MPRemoteCommandCenter (macOS) ✅ |
| v0.11 | Full Search, похожие треки, автопереход на похожий контент, история через /music-history, Ynison ✅ |
| v0.12 | Похожие треки: `/tracks/{id}/similar` и вкладка «Похожие» в Expanded Now Playing ✅ |
| v0.13 | Раздел «Станции» и радио исполнителя, управление плейлистами, лайки альбомов и исполнителей, раздел «Подкасты», скачивание треков с тегами и обложкой ✅ |
| v0.14 | Качество звука (низкое…lossless) и показ кодека с битрейтом, тексты с фолбэком на LRCLIB, треки без цензуры, «Колесо» на главной, гард недоступных дорожек ✅ |
| v0.14.1 | Дизлайки треков, правила очереди «Моей волны», автообновление с установкой на macOS ✅ |
| v0.14.2 | Бейджи контента «AI» (база Slopless) и «Без цензуры» (FckCensorData) у треков ✅ |

Выпущены версии до **v0.14.2** включительно. Раздел «Аудиокниги» живёт в ветке
`feat/audiobooks`. Подробный план и легенда приоритетов — в [ROADMAP.md](ROADMAP.md).

---

## Disclaimer

YaMusic — независимый неофициальный клиент Яндекс Музыки, не связан с компанией Яндекс. Для работы используется API Яндекс Музыки.

---

## Лицензия

Проект распространяется на условиях лицензии, указанной в репозитории.

---

[⬆ Наверх](#yamusic)