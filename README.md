# YaMusic

Unofficial desktop client for Yandex Music built with C++17, Qt 6 and QML.

## Возможности

### Музыка и поиск

- Авторизация через OAuth-токен Яндекс Музыки
- Поиск музыки
- «Моя волна»
- Персональные рекомендации
- Персональные плейлисты
- Недавно прослушанные треки
- Пользовательские плейлисты
- Понравившиеся треки
- Чарты
- Жанры и поджанры
- Страницы жанров
- Страницы плейлистов
- Страницы альбомов
- Страницы исполнителей

### Воспроизведение

- Единая очередь воспроизведения
- Добавление и удаление треков
- Изменение порядка треков
- Автоматический переход к следующему треку
- Repeat
- Shuffle
- Управление громкостью
- Mini Player
- Expanded Now Playing
- Прогресс воспроизведения и перемотка
- Dynamic Player Accent
- Несколько источников воспроизведения

---

## Установка

Для обычного использования рекомендуется использовать готовые пакеты YaMusic.

Готовые пакеты будут содержать необходимые Qt runtime-библиотеки и QML-модули, поэтому **отдельная установка Qt для запуска приложения не потребуется**.

### Windows

Будет доступен установщик:

```text
YaMusic-<version>-windows-x64.exe
```

### macOS

Будет доступен пакет:

```text
YaMusic-<version>-macos.dmg
```

После установки приложение запускается как обычное macOS-приложение.

### Linux

Будет доступен AppImage:

```text
YaMusic-<version>-linux-x86_64.AppImage
```

Запуск:

```bash
chmod +x YaMusic-*.AppImage
./YaMusic-*.AppImage
```

---

## Авторизация

### Важно

**Для работы YaMusic необходим OAuth-токен Яндекс Музыки.**

Без токена приложение не сможет получить доступ к API Яндекс Музыки.

Токен должен быть получен для собственного аккаунта Яндекса.

### Получение токена

Для получения токена используется OAuth Яндекса.

1. Создайте OAuth-приложение в Яндексе.
2. Получите `Client ID` приложения.
3. Откройте OAuth-ссылку:

```text
https://oauth.yandex.ru/authorize?response_type=token&client_id=<CLIENT_ID>
```

где `<CLIENT_ID>` — идентификатор вашего приложения.

4. Авторизуйтесь в аккаунте Яндекса.
5. Разрешите доступ приложению.
6. После авторизации браузер перенаправит вас на URL, содержащий:

```text
#access_token=<TOKEN>&expires_in=<SECONDS>
```

7. Скопируйте значение `access_token`.

Например:

```text
#access_token=xxxxxxxxxxxxxxxx&expires_in=31536000
```

В YaMusic необходимо использовать:

```text
xxxxxxxxxxxxxxxx
```

Официальная документация Яндекса:

https://yandex.ru/dev/id/doc/ru/tokens/debug-token

### Безопасность

**Никому не передавайте свой OAuth-токен.**

Токен предоставляет доступ к данным вашего аккаунта.

Не публикуйте его:

- в GitHub;
- в исходном коде;
- в скриншотах;
- в логах;
- в открытых конфигурационных файлах.

Не добавляйте токен в Git.

---

## Сборка из исходников

Если вы хотите собрать YaMusic самостоятельно, необходимы:

- C++17 compiler
- Qt 6
- Qt Quick
- Qt Multimedia
- CMake
- Ninja или другой поддерживаемый CMake generator

### Linux

Для Arch Linux:

```bash
sudo pacman -S \
    base-devel \
    cmake \
    ninja \
    qt6-base \
    qt6-declarative \
    qt6-multimedia
```

Клонирование проекта:

```bash
git clone https://github.com/andfriden/YaMusic.git
cd YaMusic
```

Сборка:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

Запуск:

```bash
./build/appYaMusic
```

### macOS

Необходимы:

- Xcode / Apple Clang
- Qt 6
- CMake
- Ninja

Клонирование:

```bash
git clone https://github.com/andfriden/YaMusic.git
cd YaMusic
```

Сборка:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

Запуск:

```bash
./build/appYaMusic.app/Contents/MacOS/appYaMusic
```

### Windows

Необходимы:

- Visual Studio 2022 с C++ workload
- Qt 6
- CMake
- Ninja или Visual Studio generator

Клонирование:

```powershell
git clone https://github.com/andfriden/YaMusic.git
cd YaMusic
```

Сборка:

```powershell
cmake -S . -B build `
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build --config Debug
```

Запуск:

```powershell
.\build\Debug\appYaMusic.exe
```

---

## Архитектура

YaMusic построен с разделением логики приложения и пользовательского интерфейса.

```text
QML
 │
 ▼
Controllers
 │
 ▼
Services / Models
 │
 ├── Yandex Music API
 │
 └── Playback
```

### C++

C++ отвечает за:

- бизнес-логику;
- работу с API;
- авторизацию;
- модели данных;
- состояние приложения;
- воспроизведение;
- очередь воспроизведения.

### QML

QML отвечает за:

- пользовательский интерфейс;
- страницы;
- навигацию;
- визуальное состояние;
- взаимодействие пользователя с приложением.

### Controllers

Основные контроллеры:

- `AppController`
- `AlbumController`
- `ArtistController`
- `ChartController`
- `GenreController`
- `LibraryController`
- `PersonalController`
- `SearchController`
- `PlaybackController`

### Services

Services инкапсулируют работу с API Яндекс Музыки и отдельными подсистемами приложения.

### Playback

Воспроизведение построено на Qt Multimedia.

Основные компоненты:

- `PlaybackController`
- `PlayerService`
- `QueueService`
- `QueueModel`

`PlayerService` отвечает непосредственно за управление `QMediaPlayer`.

`QueueService` управляет очередью воспроизведения.

`PlaybackController` связывает состояние воспроизведения с остальной частью приложения.

---

## Структура проекта

```text
YaMusic/
├── src/
│   ├── Core/
│   ├── Models/
│   ├── Playback/
│   ├── Player/
│   ├── Queue/
│   └── Yandex/
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
└── README.md
```

---

## Roadmap

### v0.1 — Technical POC

- [x] Базовое Qt-приложение
- [x] Подключение к Yandex Music API
- [x] Авторизация
- [x] Базовое воспроизведение

### v0.2 — Search

- [x] Поиск
- [x] Результаты поиска
- [x] Работа с треками
- [x] Страницы альбомов
- [x] Страницы исполнителей

### v0.3 — My Wave / Personal

- [x] «Моя волна»
- [x] Персональные рекомендации
- [x] Недавно прослушанные
- [x] Персональные плейлисты

### v0.4 — Smart Queue & Playback

- [x] Единая очередь
- [x] Добавление и удаление треков
- [x] Изменение порядка
- [x] Автоматический переход к следующему треку
- [x] Repeat
- [x] Shuffle
- [x] Несколько источников воспроизведения

### v0.5 — Library & Playlists

- [x] Пользовательские плейлисты
- [x] Страницы плейлистов
- [x] Библиотека
- [x] Недавно прослушанные

### v0.6 — Likes

- [x] Понравившиеся треки
- [x] Работа с лайками

### v0.7 — Playback Polish / Now Playing

- [x] Mini Player
- [x] Expanded Now Playing
- [x] Управление прогрессом
- [x] Перемотка
- [x] Управление громкостью
- [x] Dynamic Player Accent

### v0.8 — Catalog & Library Expansion 🚧

#### Catalog

- [x] Чарты
- [x] Жанры
- [x] Поджанры
- [x] Страницы жанров
- [ ] Единая страница каталога
- [ ] Новые релизы
- [ ] Унифицированные состояния загрузки, ошибки и отсутствия данных

#### Library

- [x] Пользовательские плейлисты
- [x] Понравившиеся треки
- [x] Недавно прослушанные треки
- [ ] Сохранённые альбомы
- [ ] Любимые исполнители
- [ ] Обновление данных

---

## Дальнейшие планы

- Feed
- Расширенные персональные рекомендации
- Радио
- Lyrics
- Интерфейс настройки токена
- Выход из аккаунта
- Переключение аккаунтов
- Настройка proxy

---

## Disclaimer

YaMusic — независимый неофициальный клиент Яндекс Музыки.

Проект не является официальным продуктом Яндекса и не связан с компанией Яндекс.

Для работы приложения используется API Яндекс Музыки и OAuth-токен пользователя.

---

## Лицензия

Проект распространяется на условиях лицензии, указанной в репозитории.

---

[⬆ Наверх](#yamusic)
