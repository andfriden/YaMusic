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
* Свои плейлисты, лайки, недавно прослушанные, альбомы, исполнители
* Чарты России и мира, жанры, новые релизы
* Страницы альбомов, плейлистов, исполнителей и жанров
* Единая очередь: repeat, shuffle, автопереход на похожий контент
* Подсветка текущего трека
* Mini Player, Expanded Now Playing, перемотка, громкость, Dynamic Player Accent
* Текст песен (🎤) с синхронизированной подсветкой строк
* Тёмная тема Catppuccin (Latte / Macchiato), попап настроек
* Адаптивный интерфейс: верхняя навигация, боковая и контекстная панели

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
| Linux x86_64 | AppImage | `YaMusic-<version>-linux-x86_64.AppImage` |
| Linux x86_64 | архив | `YaMusic-<version>-linux-x86_64.tar.gz` |

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
* **Linux x86_64** — `YaMusic-<version>-linux-x86_64.AppImage` и `YaMusic-<version>-linux-x86_64.tar.gz`

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

Контроллеры: `AppController`, `AlbumController`, `ArtistController`, `ChartController`, `GenreController`, `LibraryController`, `PersonalController`, `PlaybackController`, `SearchController`.

Воспроизведение построено на Qt Multimedia: `PlaybackController` связывает состояние с приложением, `PlayerService` управляет `QMediaPlayer`, `QueueService` — очередью.

---

## Структура проекта

```text
YaMusic/
├── src/        # Core, Models, Playback, Player, Queue, Yandex
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

---

## Disclaimer

YaMusic — независимый неофициальный клиент Яндекс Музыки, не связан с компанией Яндекс. Для работы используется API Яндекс Музыки.

---

## Лицензия

Проект распространяется на условиях лицензии, указанной в репозитории.

---

[⬆ Наверх](#yamusic)