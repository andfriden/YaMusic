<div align="center">

# 🎧 YaMusic

**Независимый desktop-клиент Яндекс Музыки**

Построен на C++20 · Qt 6.9 · QML · macOS, Linux и Windows

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
* Раздел «Статистика»: сводка прослушиваний, топы треков и исполнителей, график «Минуты по дням» за последние 30 дней
* «Новые релизы», жанры
* Раздел «Станции»: полный список станций ротора с группировкой по типам
* Раздел «Подкасты»: витрина подкастов из лендинга
* Страницы альбомов, плейлистов, исполнителей и жанров
* Радио исполнителя: все треки артиста, затем цепочка похожих без повторов
* Единая очередь: repeat, shuffle, автопереход на похожий контент и продолжение альбома
* Подсветка текущего трека
* Отдельные окна: мини-плеер поверх других окон и развёрнутый плеер — с перемоткой, громкостью и Dynamic Player Accent; настройки — раздел в боковом меню
* Единый набор SVG-иконок плеера вместо символьных обозначений (play/pause, next/prev, лайк, текст песни)
* Текст песен с синхронизированной подсветкой строк, фолбэк на публичную базу LRCLIB
* Выбор качества аудио (низкое / среднее / высокое / lossless) и показ кодека с битрейтом
* Треки без цензуры — подмена на версии без цензуры по открытой базе FckCensorData
* Блок «Колесо» на главной: подборки по вашим трекам и альбомам
* Дорожки без прав на прослушивание помечаются в списке и уводят во внешний сервис вместо ошибки
* Скачивание трека в «Загрузки» с метаданными и вложенной обложкой
* Контекстное меню трека по правой кнопке мыши (в списках и плеере): «Моя волна по треку», лайк и дизлайк, добавление в плейлист, скачивание, переход к исполнителю и альбому, копирование «Исполнитель — Название»
* Отчёты о падениях: перехват сигналов и `terminate`, файл отчёта на диске и отправка в трекер из окна
* Тёмная тема Catppuccin (Latte / Macchiato), раздел настроек в боковом меню
* Адаптивный интерфейс: верхняя навигация, боковая и контекстная панели

Полная история изменений по версиям — в [CHANGELOG.md](CHANGELOG.md).

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

**Зависимости:** C++20, Qt 6.9 (Quick, Gui, Network, Multimedia; на Linux ещё DBus), TagLib, CMake 3.21+, Ninja. `CMakeLists.txt` — источник истины.

**Ubuntu:**

```bash
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-declarative-dev qt6-multimedia-dev libtag1-dev
```

**Arch Linux:**

```bash
sudo pacman -S --needed base-devel cmake ninja qt6-base qt6-declarative qt6-multimedia taglib
```

**macOS:** Xcode / Apple Clang, Qt 6.9, CMake, Ninja, TagLib (Homebrew).
**Windows:** Visual Studio 2022 (C++ workload), Qt 6.9, CMake, Ninja, TagLib (vcpkg).

Нужен именно Qt 6.9 (`find_package(Qt6 6.9 REQUIRED)`): в репозиториях Ubuntu 22.04 и 24.04 его ещё нет — ставьте Qt через online installer или берите дистрибутив со свежим Qt. TagLib обязателен, без него конфигурация не пройдёт.

Общие шаги для всех ОС:

```bash
git clone https://github.com/andfriden/YaMusic.git
cd YaMusic
cmake -S . -B build
cmake --build build --parallel
```

Запуск: `./build/yamusic` (Linux), `./build/YaMusic.app/Contents/MacOS/YaMusic` (macOS), `.\build\Debug\YaMusic.exe` (Windows). Многоконфигурационные генераторы (Xcode, Visual Studio) добавляют подкаталог сборки — `Debug` или `Release`.

Установка в отдельный каталог: `cmake --install build --prefix dist`.

### Релизные пакеты

Пакеты собираются автоматически в GitHub Actions при пуше тега `v*` (см. `.github/workflows/build.yml`):

* **Windows x64** — `YaMusic-<version>-windows-x64-portable.zip`
* **macOS** — отдельно для Intel и Apple Silicon: `YaMusic-<version>-macos-x64.dmg`, `YaMusic-<version>-macos-arm64.dmg`
* **Linux x86_64** — `YaMusic-<version>-x86_64.AppImage` и `YaMusic-<version>-linux-x86_64.tar.gz`

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

Контроллеры: `AppController`, `AccentController`, `AlbumController`, `ArtistController`, `AudioQualityController`, `AuthController`, `CrashReporter`, `GenreController`, `LibraryController`, `LyricsController`, `PersonalController`, `PlaybackController`, `PlaybackSettingsController`, `PlayerAccentService`, `PreloadController`, `SearchController`, `StationsController`, `StatsController`, `ThemeController`, `UpdateController`.

Воспроизведение построено на Qt Multimedia: `PlaybackController` связывает состояние с приложением, `PlayerService` управляет `QMediaPlayer`, `QueueService` — очередью.

---

## Структура проекта

```text
YaMusic/
├── src/        # Core, Crash, Download, MediaControls, Models, Playback, Player, Queue, Update, Yandex, Ynison
├── qml/        # Components, Context, Home, Layout, MyWave, Pages, Theme
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
| v0.7 | Expanded Now Playing, перемотка, громкость (мини-плеер появился только в v0.14.3) |
| v0.8 | Чарты, жанры, новые релизы, переключение тем ✅ |
| v0.9 | Текст песен (Lyrics) ✅ |
| v0.10 | Системные медиа-кнопки: MPRIS (Linux), SMTC (Windows), MPRemoteCommandCenter (macOS) ✅ |
| v0.11 | Full Search, похожие треки, автопереход на похожий контент, история через /music-history, Ynison ✅ |
| v0.12 | Похожие треки: `/tracks/{id}/similar` и вкладка «Похожие» в Expanded Now Playing ✅ |
| v0.13 | Раздел «Станции» и радио исполнителя, управление плейлистами, лайки альбомов и исполнителей, раздел «Подкасты», скачивание треков с тегами и обложкой ✅ |
| v0.14 | Качество звука (низкое…lossless) и показ кодека с битрейтом, тексты с фолбэком на LRCLIB, треки без цензуры, «Колесо» на главной, гард недоступных дорожек ✅ |
| v0.14.1 | Дизлайки треков, правила очереди «Моей волны», автообновление с установкой на macOS ✅ |
| v0.14.2 | Бейджи контента «AI» (база Slopless) и «Без цензуры» (FckCensorData) у треков ✅ |
| v0.14.3 | Мини-плеер в отдельном окне, раздел «Статистика», отдельные окна плеера и настроек, SVG-иконки плеера, AppImage на старом LTS ✅ |
| v0.15.0 | Новый интерфейс (боковой сайдбар, обновлённая главная с кнопками разделов), «Моя волна по треку», контекстное меню трека (правая кнопка мыши) в списках и плеере, ссылки на альбом и артиста, имя профиля в приветствии ✅ |
| v0.15.1 | Умный поиск (живой ввод, подсказки, история, исправление опечаток), поиск в сайдбаре и на главной, скрытие AI-треков ✅ |

Выпущены версии до **v0.15.1** включительно. Раздел «Аудиокниги» живёт в ветке
`feat/audiobooks`. Подробный план и легенда приоритетов — в [ROADMAP.md](ROADMAP.md).

---

## Disclaimer

YaMusic — независимый неофициальный клиент Яндекс Музыки, не связан с компанией Яндекс. Для работы используется API Яндекс Музыки.

---

## Лицензия

Проект распространяется на условиях лицензии, указанной в репозитории.

---

[⬆ Наверх](#yamusic)