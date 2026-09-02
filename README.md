# YaMusic

[Русский](#русский) | [English](#english)

---

<a id="русский"></a>

## Русский

Нативный desktop-клиент Яндекс Музыки, разработанный на **C++17**, **Qt 6**, **Qt Quick / QML** и **CMake**.

YaMusic предоставляет интерфейс для поиска музыки, персональных рекомендаций, управления медиатекой и воспроизведения. Проект использует неофициальные интерфейсы Yandex Music API и находится в активной разработке.

> Основной язык документации — русский.

## Возможности

- Авторизация по токену Яндекс Музыки
- Поиск музыки
- «Моя волна»
- Персональные рекомендации
- Персональные плейлисты
- Недавно прослушанные треки
- Пользовательские плейлисты
- Понравившиеся треки
- Чарты
- Страницы плейлистов, альбомов и исполнителей
- Единая очередь воспроизведения
- Автоматический переход к следующему треку
- Repeat и Shuffle
- Управление громкостью
- Mini Player
- Expanded Now Playing
- Dynamic Player Accent

## Сборка и запуск

### Требования

- Qt 6
- Qt Quick
- Qt Multimedia
- CMake
- Компилятор с поддержкой C++17

### Windows

#### Конфигурация
```powershell
cmake -S . -B build `
    -DCMAKE_BUILD_TYPE=Debug
```

#### Сборка
```powershell
cmake --build build --config Debug
```

#### Запуск
```powershell
.\build\Debug\appYaMusic.exe
```

### Linux

#### Конфигурация
```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Сборка
```bash
cmake --build build \
    --target appYaMusic \
    -j$(nproc)
```

#### Запуск
```bash
./build/appYaMusic
```

### macOS

#### Конфигурация
```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Сборка
```bash
cmake --build build \
    --target appYaMusic \
    -j$(sysctl -n hw.ncpu)
```

#### Запуск
```bash
./build/appYaMusic.app/Contents/MacOS/appYaMusic
```

## Дорожная карта

### v0.1 — Technical POC ✅

- [x] Базовый проект на C++17, Qt 6 и CMake
- [x] Интеграция с Yandex Music API
- [x] Получение информации об аккаунте
- [x] Базовое воспроизведение
- [x] Получение stream URL

### v0.2 — Search ✅

- [x] Поиск музыки
- [x] Отображение результатов
- [x] Обложки и метаданные
- [x] Воспроизведение найденного трека
- [x] Состояния загрузки, ошибки и пустого результата

### v0.3 — My Wave / Personal ✅

- [x] «Моя волна»
- [x] Автоматическая догрузка треков
- [x] Персональные рекомендации
- [x] Персональные плейлисты
- [x] Недавно прослушанные треки

### v0.4 — Smart Queue &amp; Playback ✅

- [x] Единая очередь воспроизведения
- [x] Добавление, удаление и перемещение треков
- [x] Автоматический переход к следующему треку
- [x] Repeat
- [x] Shuffle
- [x] Поддержка разных источников воспроизведения

### v0.5 — Library &amp; Playlists ✅

- [x] Страница медиатеки
- [x] Пользовательские плейлисты
- [x] Страница плейлиста
- [x] Воспроизведение плейлистов
- [x] Навигация к альбомам и исполнителям

### v0.6 — Likes ✅

- [x] Понравившиеся треки
- [x] Воспроизведение списка
- [x] Навигация к альбомам и исполнителям

### v0.7 — Playback Polish / Now Playing ✅

- [x] Переработанный Mini Player
- [x] Expanded Now Playing
- [x] Dynamic Player Accent
- [x] Управление громкостью
- [x] Улучшенное отображение очереди
- [x] Улучшенные обложки и метаданные

### v0.8 — Catalog &amp; Library Expansion 🚧

#### Catalog

- [x] Чарты
- [ ] Единая страница каталога
- [ ] Жанры
- [ ] Новые релизы
- [ ] Унифицированные состояния загрузки, ошибки и отсутствия данных

#### Library

- [x] Пользовательские плейлисты
- [x] Понравившиеся треки
- [x] Недавно прослушанные треки
- [ ] Сохранённые альбомы
- [ ] Любимые исполнители
- [ ] Обновление данных

### Дальнейшие планы

- [ ] Feed
- [ ] Расширенные персональные рекомендации
- [ ] Радиораздел
- [ ] Тексты песен
- [ ] Интерфейс настройки токена
- [ ] Выход из аккаунта и смена пользователя
- [ ] Настройка proxy

## Примечание

YaMusic является независимым неофициальным проектом и не связан с Яндексом.

Для доступа к персональному контенту необходим действующий токен Яндекс Музыки. Не публикуйте токен и не добавляйте его в исходный код или историю Git.

&lt;p align="right"&gt;&lt;a href="#yamusic"&gt;Наверх ↑&lt;/a&gt;&lt;/p&gt;

---

&lt;a id="english"&gt;&lt;/a&gt;

## English

A native Yandex Music desktop client built with **C++17**, **Qt 6**, **Qt Quick / QML**, and **CMake**.

YaMusic provides an interface for music search, personalized recommendations, library management, and playback. The project uses unofficial Yandex Music API interfaces and is under active development.

## Features

- Authentication with a Yandex Music token
- Music search
- My Wave
- Personalized recommendations
- Personalized playlists
- Recently played tracks
- User playlists
- Liked tracks
- Charts
- Playlist, album, and artist pages
- Unified playback queue
- Automatic playback of the next track
- Repeat and Shuffle
- Volume control
- Mini Player
- Expanded Now Playing
- Dynamic Player Accent

## Build and run

### Requirements

- Qt 6
- Qt Quick
- Qt Multimedia
- CMake
- A compiler with C++17 support

### Windows

#### Configure
```powershell
cmake -S . -B build `
    -DCMAKE_BUILD_TYPE=Debug
```

#### Build
```powershell
cmake --build build --config Debug
```

#### Run
```powershell
.\build\Debug\appYaMusic.exe
```

### Linux

#### Configure
```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Build
```bash
cmake --build build \
    --target appYaMusic \
    -j$(nproc)
```

#### Run
```bash
./build/appYaMusic
```

### macOS

#### Configure
```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Build
```bash
cmake --build build \
    --target appYaMusic \
    -j$(sysctl -n hw.ncpu)
```

#### Run
```bash
./build/appYaMusic.app/Contents/MacOS/appYaMusic
```

## Roadmap

### v0.1 — Technical POC ✅

- [x] Base project with C++17, Qt 6, and CMake
- [x] Yandex Music API integration
- [x] Account information
- [x] Basic playback
- [x] Stream URL retrieval

### v0.2 — Search ✅

- [x] Music search
- [x] Search results
- [x] Artwork and metadata
- [x] Playback of a selected track
- [x] Loading, error, and empty states

### v0.3 — My Wave / Personal ✅

- [x] My Wave
- [x] Automatic loading of additional tracks
- [x] Personalized recommendations
- [x] Personalized playlists
- [x] Recently played tracks

### v0.4 — Smart Queue &amp; Playback ✅

- [x] Unified playback queue
- [x] Add, remove, and reorder tracks
- [x] Automatic playback of the next track
- [x] Repeat
- [x] Shuffle
- [x] Multiple playback sources

### v0.5 — Library &amp; Playlists ✅

- [x] Library page
- [x] User playlists
- [x] Playlist page
- [x] Playlist playback
- [x] Album and artist navigation

### v0.6 — Likes ✅

- [x] Liked tracks
- [x] Liked tracks playback
- [x] Album and artist navigation

### v0.7 — Playback Polish / Now Playing ✅

- [x] Redesigned Mini Player
- [x] Expanded Now Playing
- [x] Dynamic Player Accent
- [x] Volume control
- [x] Improved queue presentation
- [x] Improved artwork and metadata

### v0.8 — Catalog &amp; Library Expansion 🚧

#### Catalog

- [x] Charts
- [ ] Unified catalog page
- [ ] Genres
- [ ] New releases
- [ ] Unified loading, error, and empty states

#### Library

- [x] User playlists
- [x] Liked tracks
- [x] Recently played tracks
- [ ] Saved albums
- [ ] Favorite artists
- [ ] Data refresh

### Future plans

- [ ] Feed
- [ ] Advanced personalized recommendations
- [ ] Radio
- [ ] Lyrics
- [ ] Token configuration interface
- [ ] Sign out and account switching
- [ ] Proxy configuration

## Disclaimer

YaMusic is an independent, unofficial project and is not affiliated with Yandex.

A valid Yandex Music token is required to access personalized content. Do not publish your token or commit it to source code or Git history.

&lt;p align="right"&gt;&lt;a href="#yamusic"&gt;Back to top ↑&lt;/a&gt;&lt;/p&gt;