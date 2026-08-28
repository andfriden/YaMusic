# YaMusic

Native-клиент Яндекс Музыки для desktop, написанный на **C++17**, **Qt 6**, **Qt Quick / QML** и **CMake**.

Проект ориентирован на нативный интерфейс музыкального приложения с поиском, персональной музыкой, медиатекой, плейлистами, альбомами, исполнителями и полноценным воспроизведением.

> Основной язык документации — русский.

## Возможности

### Аккаунт

- Авторизация через токен Яндекс Музыки
- Получение информации об аккаунте
- Получение UID пользователя
- Проверка соединения с API

### Поиск

- Поиск музыки
- Результаты поиска
- Artwork треков
- Название и исполнитель
- Воспроизведение найденного трека
- Переход к исполнителю
- Переход к альбому
- Состояния загрузки, ошибки и пустого результата

### Моя волна

- Загрузка My Wave
- Догрузка следующих треков
- Модель треков
- Воспроизведение
- Работа с очередью

### Персональный контент

- Персональные рекомендации
- Недавно прослушанные треки
- Персональные плейлисты

### Медиатека

- Страница медиатеки
- Пользовательские плейлисты
- Artwork плейлистов
- Открытие страницы плейлиста
- Список треков плейлиста
- Воспроизведение треков
- Переход к исполнителю из трека
- Переход к альбому из трека

### Альбомы

- Страница альбома
- Artwork
- Список треков
- Информация об исполнителе
- Воспроизведение альбома

### Исполнители

- Страница исполнителя
- Artwork
- Популярные треки
- Популярные альбомы
- Похожие исполнители
- Переход между исполнителями

### Воспроизведение

- Play
- Pause
- Stop
- Next
- Previous
- Seek
- Repeat
- Shuffle
- Очередь воспроизведения
- Текущий трек
- Состояние воспроизведения
- Автоматический переход к следующему треку
- Автоматическое заполнение очереди

# Архитектура

Проект использует слоистую архитектуру и избегает размещения всей бизнес-логики в `AppController`.

```text
QML
 │
 ▼
AppController
 │
 ├── SearchController
 ├── PersonalController
 ├── LibraryController
 ├── AlbumController
 ├── ArtistController
 └── PlaybackController
 │
 ▼
Yandex Services
 │
 ├── AccountService
 ├── SearchService
 ├── PlaylistService
 ├── LikesService
 ├── AlbumService
 ├── ArtistService
 ├── TrackService
 └── Personal Services
 │
 ▼
Yandex API
```

Воспроизведение отделено от API-каталога и персональных сервисов:

```text
TrackService
     │
     ▼
PlaybackController
     │
     ▼
QueueService
     │
     ▼
PlayerService
     │
     ▼
QMediaPlayer
```

## Ответственность компонентов

### AppController

Фасад приложения и точка координации между QML и специализированными контроллерами.

### LibraryController

Отвечает за:

- медиатеку;
- пользовательские плейлисты;
- лайкнутые треки;
- состояние текущего плейлиста;
- состояние текущего исполнителя в библиотечном контексте.

### PersonalController

Отвечает за персональный контент:

- My Wave;
- рекомендации;
- Recent Listening;
- персональные плейлисты.

### SearchController

Отвечает за поиск и взаимодействие QML с результатами поиска.

### AlbumController

Отвечает за состояние и содержимое страницы альбома.

### ArtistController

Отвечает за состояние и содержимое страницы исполнителя.

### PlaybackController

Оркестрирует:

- текущий трек;
- очередь;
- переходы между треками;
- repeat;
- shuffle;
- воспроизведение.

### PlayerService

Непосредственное управление `QMediaPlayer`.

### QueueService

Управление очередью воспроизведения.

# Структура проекта

```text
YaMusic/
│
├── src/
│   ├── Core/
│   │   ├── AppController.*
│   │   ├── AppController+Catalog.cpp
│   │   ├── AppController+Library.cpp
│   │   ├── AppController+Personal.cpp
│   │   ├── AppController+Playback.cpp
│   │   ├── AppController+Search.cpp
│   │   ├── AppController+Selection.cpp
│   │   ├── AlbumController.*
│   │   ├── ArtistController.*
│   │   ├── LibraryController.*
│   │   ├── PersonalController.*
│   │   ├── PlaybackController.*
│   │   └── SearchController.*
│   │
│   ├── Models/
│   ├── Player/
│   ├── Playback/
│   ├── Queue/
│   └── Yandex/
│       ├── Account/
│       ├── Auth/
│       ├── Catalog/
│       └── Personal/
│
├── qml/
│   ├── Components/
│   ├── Context/
│   ├── Home/
│   ├── Layout/
│   ├── MyWave/
│   ├── Pages/
│   ├── Search/
│   └── Theme/
│
├── CMakeLists.txt
├── main.cpp
└── README.md
```

# Roadmap

## v0.1 — Technical POC ✅

**Цель:** получить рабочий фундамент приложения и первое реальное воспроизведение музыки.

### Project Foundation

- [x] C++17
- [x] Qt 6
- [x] Qt Quick / QML
- [x] CMake
- [x] Базовая структура проекта
- [x] AppController

### Yandex API Foundation

- [x] AccountService
- [x] Проверка соединения с API
- [x] Получение информации об аккаунте
- [ ] Привести API-слой к стабильному виду

### Player / Playback

- [x] PlayerService
- [x] QMediaPlayer
- [x] play()
- [x] playUrl()
- [x] pause()
- [x] resume()
- [x] stop()
- [x] togglePlayback()
- [x] playingChanged
- [x] playbackStarted
- [x] playbackPaused
- [x] playbackStopped
- [x] currentUrlChanged
- [x] Обработка ошибок
- [x] Получение реального stream URL
- [x] Связь TrackService → Playback
- [x] Реальное воспроизведение найденного трека

## v0.2 — Search ✅

**Цель:** полноценный поиск музыки.

### Search

- [x] SearchService
- [x] SearchModel
- [x] Результаты поиска
- [x] Выбор результата
- [x] Модель Track
- [x] Загрузка полной информации о треке
- [x] Получение stream URL
- [x] Подключение к PlayerService

### UI

- [x] Search field
- [x] Список результатов
- [x] Artwork
- [x] Artist / Title
- [x] Play по нажатию
- [x] Loading
- [x] Error
- [x] Empty state

## v0.3 — My Wave / Personal ✅

**Цель:** подключить персональную музыку пользователя.

### Personal

- [x] YandexPersonal
- [x] My Wave
- [x] Персональные рекомендации
- [x] Recent Listening
- [x] Personal Playlists
- [ ] Feed
- [ ] Likes UI

### Wave

- [x] Получение треков Wave
- [x] Модель треков
- [x] Воспроизведение Wave
- [x] Переключение треков
- [ ] Автоматическое получение следующего трека

## v0.4 — Smart Queue & Playback ✅

**Цель:** нормальная музыкальная очередь и надёжное управление воспроизведением.

### Queue

- [x] QueueService
- [x] Current Track
- [x] Up Next
- [x] Добавление треков
- [x] Очистка очереди

### Playback

- [x] Auto-next
- [x] Next
- [x] Previous
- [x] Repeat
- [x] Shuffle
- [x] Автоматическое заполнение очереди

## v0.5 — Library & Playlists ✅

**Цель:** полноценная персональная медиатека.

### Library

- [x] LibraryPage
- [x] Пользовательские плейлисты
- [x] LibraryPlaylistsModel
- [x] Artwork плейлистов
- [x] Открытие PlaylistPage
- [x] Список треков
- [x] Воспроизведение плейлиста
- [x] Artist navigation
- [x] Album navigation

### Architecture

- [x] LibraryController
- [x] Library service layer
- [x] AppController → LibraryController
- [x] AppController+Library.cpp

## v0.6 — Likes 🟡

**Цель:** полноценная работа с понравившимися треками.

### Backend

- [x] LikesService
- [x] LikedTracksModel
- [x] Получение ID лайкнутых треков
- [x] Получение полной информации о Track
- [x] LibraryController integration
- [x] AppController integration

### UI

- [ ] Полный список лайкнутых треков
- [ ] Корректная прокрутка
- [ ] Artist navigation
- [ ] Album navigation
- [ ] Удобное воспроизведение списка лайков

## v0.7 — Playback Polish / Now Playing 🔜

**Цель:** улучшить пользовательский опыт воспроизведения.

### Player UI

- [ ] Переработать Mini Player
- [ ] Улучшить Now Playing
- [ ] Добавить управление громкостью
- [ ] Улучшить playback controls
- [ ] Улучшить отображение очереди
- [ ] Улучшить состояния воспроизведения

### Now Playing

- [ ] Улучшенное отображение текущего трека
- [ ] Улучшенное artwork
- [ ] Улучшенные метаданные

# Будущее

## Catalog

- [ ] Genres
- [ ] Charts
- [ ] New Releases

## Personal

- [ ] Feed
- [ ] Улучшенные рекомендации
- [ ] Дальнейшее развитие Likes

## Radio

- [ ] Rotor
- [ ] Радиораздел

## Lyrics

- [ ] Lyrics
- [ ] Отображение текста песни

## Infrastructure

- [ ] Полноценное получение токена для новых пользователей
- [ ] Установка и хранение токена
- [ ] Logout
- [ ] Смена аккаунта
- [ ] Proxy configuration
- [ ] Применение proxy к API-запросам
- [ ] Тестирование авторизации через proxy
- [ ] Тестирование API через proxy
- [ ] Тестирование artwork через proxy

# Принципы разработки

### Не смешивать бизнес-логику

```text
QML
 ↓
AppController
 ↓
Specialized Controllers
 ↓
Yandex Services
 ↓
Yandex API
```

`AppController` не должен превращаться в единый класс со всей бизнес-логикой приложения.

### Разделять ответственность

- `AppController` — фасад и координация
- `LibraryController` — медиатека
- `PersonalController` — персональный контент
- `SearchController` — поиск
- `AlbumController` — альбом
- `ArtistController` — исполнитель
- `PlaybackController` — оркестрация воспроизведения
- `PlayerService` — непосредственное воспроизведение
- `QueueService` — очередь
- Yandex Services — работа с API

# Сборка

## Конфигурация

```bash
cmake -S . -B cmake-build-debug \
    -DCMAKE_BUILD_TYPE=Debug
```

## Сборка

```bash
cmake --build cmake-build-debug \
    --target appYaMusic \
    -j 10
```

## Запуск на macOS

```bash
./cmake-build-debug/appYaMusic.app/Contents/MacOS/appYaMusic
```

# Текущий статус

**Текущая версия разработки: v0.6 — Likes 🟡**

Основной фундамент приложения уже готов.

Работают:

- поиск;
- My Wave;
- персональные рекомендации;
- Recent Listening;
- альбомы;
- исполнители;
- плейлисты;
- медиатека;
- воспроизведение;
- очередь;
- автоматический переход между треками;
- автоматическое заполнение очереди;
- навигация между сущностями.

Текущий незавершённый участок — **UI лайкнутых треков**.

После завершения `v0.6` основной фокус перейдёт на **Playback Polish / Now Playing**.
