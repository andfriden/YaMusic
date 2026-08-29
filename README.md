# YaMusic

Native-клиент Яндекс Музыки для desktop, написанный на **C++17**, **Qt 6**, **Qt Quick / QML** и **CMake**.

Проект ориентирован на нативный интерфейс музыкального приложения с поиском, персональной музыкой, медиатекой, плейлистами, альбомами, исполнителями и полноценным воспроизведением.

> Основной язык документации — русский.

## Возможности

### Аккаунт

* Авторизация через токен Яндекс Музыки
* Получение информации об аккаунте
* Получение UID пользователя
* Проверка соединения с API

### Поиск

* Поиск музыки
* Результаты поиска
* Artwork треков
* Название и исполнитель
* Воспроизведение найденного трека
* Переход к исполнителю
* Переход к альбому
* Loading / Error / Empty states

### Моя волна

* Загрузка My Wave
* Догрузка следующих партий
* Модель треков
* Воспроизведение
* Автоматический переход к следующему треку
* Работа с очередью
* My Wave feedback

### Персональный контент

* Персональные рекомендации
* Недавно прослушанные треки
* Персональные плейлисты

### Медиатека

* Страница медиатеки
* Пользовательские плейлисты
* Artwork плейлистов
* Открытие страницы плейлиста
* Список треков
* Воспроизведение плейлиста
* Переход к исполнителю
* Переход к альбому

### Лайки

* Список понравившихся треков
* Прокрутка
* Artist navigation
* Album navigation
* Воспроизведение списка лайков

### Альбомы

* Страница альбома
* Artwork
* Список треков
* Информация об исполнителе
* Воспроизведение альбома
* Другие альбомы исполнителя

### Исполнители

* Страница исполнителя
* Artwork
* Популярные треки
* Популярные альбомы
* Похожие исполнители
* Переход между исполнителями

### Воспроизведение

* Play
* Pause
* Stop
* Next
* Previous
* Seek
* Repeat
* Shuffle
* Громкость
* Очередь воспроизведения
* Текущий трек
* Состояния воспроизведения
* Auto-next
* Автоматическое заполнение очереди
* Разные источники воспроизведения

### Now Playing

* Mini Player
* Dynamic Player Accent
* Artwork
* Прогресс воспроизведения
* Управление громкостью
* Expanded Now Playing
* Предыдущий трек
* Следующие треки
* Источник воспроизведения
* Queue

---

# Архитектура

Проект использует слоистую архитектуру.

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

Воспроизведение отделено от API-каталога:

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

---

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

---

# Roadmap

## v0.1 — Technical POC ✅

**Цель:** рабочий фундамент приложения и первое реальное воспроизведение музыки.

### Project Foundation

* [x] C++17
* [x] Qt 6
* [x] Qt Quick / QML
* [x] CMake
* [x] Базовая структура проекта
* [x] AppController

### Yandex API Foundation

* [x] AccountService
* [x] Проверка соединения с API
* [x] Получение информации об аккаунте
* [ ] Привести API-слой к стабильному виду

### Player / Playback

* [x] PlayerService
* [x] QMediaPlayer
* [x] play()
* [x] playUrl()
* [x] pause()
* [x] resume()
* [x] stop()
* [x] togglePlayback()
* [x] playingChanged
* [x] playbackStarted
* [x] playbackPaused
* [x] playbackStopped
* [x] currentUrlChanged
* [x] Обработка ошибок
* [x] Получение реального stream URL
* [x] Связь TrackService → Playback
* [x] Реальное воспроизведение найденного трека

---

## v0.2 — Search ✅

**Цель:** полноценный поиск музыки.

### Search

* [x] SearchService
* [x] SearchModel
* [x] Результаты поиска
* [x] Выбор результата
* [x] Модель Track
* [x] Загрузка полной информации о треке
* [x] Получение stream URL
* [x] Подключение к PlayerService

### UI

* [x] Search field
* [x] Список результатов
* [x] Artwork
* [x] Artist / Title
* [x] Play по нажатию
* [x] Loading
* [x] Error
* [x] Empty state

---

## v0.3 — My Wave / Personal ✅

**Цель:** подключить персональную музыку пользователя.

### Personal

* [x] YandexPersonal
* [x] My Wave
* [x] Персональные рекомендации
* [x] Recent Listening
* [x] Personal Playlists

### Wave

* [x] Получение треков Wave
* [x] Модель треков
* [x] Воспроизведение Wave
* [x] Переключение треков
* [x] Автоматическая подгрузка следующих партий
* [x] Feedback

---

## v0.4 — Smart Queue & Playback ✅

**Цель:** нормальная музыкальная очередь и надёжное управление воспроизведением.

### Queue

* [x] QueueService
* [x] Current Track
* [x] Up Next
* [x] Previous Track
* [x] Добавление треков
* [x] Удаление треков
* [x] Перемещение треков
* [x] Очистка очереди

### Playback

* [x] Auto-next
* [x] Next
* [x] Previous
* [x] Repeat
* [x] Shuffle
* [x] Автоматическое заполнение очереди
* [x] Разные источники воспроизведения

---

## v0.5 — Library & Playlists ✅

**Цель:** полноценная персональная медиатека.

### Library

* [x] LibraryPage
* [x] Пользовательские плейлисты
* [x] LibraryPlaylistsModel
* [x] Artwork плейлистов
* [x] Открытие PlaylistPage
* [x] Список треков
* [x] Воспроизведение плейлиста
* [x] Artist navigation
* [x] Album navigation

### Architecture

* [x] LibraryController
* [x] Library service layer
* [x] AppController → LibraryController
* [x] AppController+Library.cpp

---

## v0.6 — Likes ✅

**Цель:** полноценная работа с понравившимися треками.

### Backend

* [x] LikesService
* [x] LikedTracksModel
* [x] Получение ID лайкнутых треков
* [x] Получение полной информации о Track
* [x] LibraryController integration
* [x] AppController integration

### UI

* [x] Полный список лайкнутых треков
* [x] Корректное отображение списка
* [x] Прокрутка списка
* [x] Artist navigation
* [x] Album navigation
* [x] Воспроизведение списка лайков

---

## v0.7 — Playback Polish / Now Playing ✅

**Цель:** улучшить пользовательский опыт воспроизведения.

### Player UI

* [x] Переработан Mini Player
* [x] Dynamic Player Accent
* [x] Управление громкостью
* [x] Playback controls
* [x] Состояния воспроизведения
* [x] Улучшенное отображение очереди

### Now Playing

* [x] Текущий трек
* [x] Исполнитель
* [x] Альбом
* [x] Artwork
* [x] Прогресс воспроизведения
* [x] Expanded Now Playing / Queue
* [x] Кнопка открытия Expanded Now Playing
* [x] Отображение источника воспроизведения
* [x] Отображение текущего трека и очереди
* [x] Предыдущий трек
* [x] Следующие треки
* [x] Улучшенное artwork
* [x] Улучшенные метаданные

---

# Будущее

## Catalog

* [ ] Genres
* [ ] Charts
* [ ] New Releases

## Personal

* [ ] Feed
* [ ] Улучшенные рекомендации
* [ ] Дальнейшее развитие Likes

## Radio

* [ ] Rotor
* [ ] Радиораздел

## Lyrics

* [ ] Lyrics
* [ ] Отображение текста песни

## Infrastructure

* [ ] Полноценное получение токена для новых пользователей
* [ ] UI для установки токена
* [ ] Безопасное хранение токена
* [ ] Logout
* [ ] Смена аккаунта
* [ ] Proxy configuration
* [ ] Применение proxy к API-запросам
* [ ] Тестирование авторизации через proxy
* [ ] Тестирование API через proxy
* [ ] Тестирование artwork через proxy

---

# Сборка и запуск

YaMusic использует C++17, Qt 6 и CMake.

## Требования

* Qt 6
* CMake
* C++17 compiler
* Qt Quick
* Qt Multimedia

---

## Windows

### Конфигурация

```powershell
cmake -S . -B build `
    -DCMAKE_BUILD_TYPE=Debug
```

### Сборка

```powershell
cmake --build build --config Debug
```

### Запуск

```powershell
.\build\Debug\appYaMusic.exe
```

---

## Linux

### Конфигурация

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

### Сборка

```bash
cmake --build build \
    --target appYaMusic \
    -j$(nproc)
```

### Запуск

```bash
./build/appYaMusic
```

---

## macOS

### Конфигурация

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

### Сборка

```bash
cmake --build build \
    --target appYaMusic \
    -j$(sysctl -n hw.ncpu)
```

### Запуск

```bash
./build/appYaMusic.app/Contents/MacOS/appYaMusic
```

---

# Токен Яндекс Музыки

Для доступа к персональному контенту и части API нужен токен Яндекс Музыки.

Токен не должен находиться в Git, README или исходном коде.

## Хранение

YaMusic использует `QSettings`.

Ключ:

```text
yandex/token
```

Хранилище предоставляет:

```cpp
YandexTokenStorage::load()
YandexTokenStorage::saveToken()
YandexTokenStorage::clearToken()
```

Токен сохраняется локально через `QSettings` и автоматически используется при последующем запуске приложения.

## Пока нет UI ввода токена

Сейчас отдельного интерфейса для первого ввода токена ещё нет.

Для разработки токен можно сохранить через:

```cpp
YandexTokenStorage::saveToken(
    "YOUR_YANDEX_MUSIC_TOKEN"
);
```

После сохранения приложение сможет загрузить его через:

```cpp
YandexTokenStorage::load();
```

После появления полноценного UI этот раздел будет заменён инструкцией для пользователя.

> Не коммитьте настоящий токен в репозиторий.

---

# Текущий статус

**Текущая версия: v0.7 — Playback Polish / Now Playing ✅**

Основной фундамент приложения и playback-инфраструктура реализованы.

Работают:

* аккаунт и подключение к Yandex API;
* поиск;
* My Wave;
* персональные рекомендации;
* Recent Listening;
* персональные плейлисты;
* медиатека;
* лайкнутые треки;
* плейлисты;
* альбомы;
* исполнители;
* воспроизведение;
* QueueService;
* Auto-next;
* Repeat;
* Shuffle;
* управление громкостью;
* Dynamic Player Accent;
* Mini Player;
* Expanded Now Playing;
* единая очередь для разных источников воспроизведения.

Следующий этап — развитие функциональности из раздела **«Будущее»**.
