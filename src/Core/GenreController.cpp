#include "GenreController.h"

#include "../Yandex/Catalog/GenreService.h"
#include "../Yandex/Personal/PlaylistService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QTextStream>
#include <QVariantMap>


namespace
{

QVariantMap playlistToMap(
    const Playlist &playlist)
{
    QVariantMap result;

    result.insert(
        "uid",
        playlist.uid);

    result.insert(
        "kind",
        playlist.kind);

    result.insert(
        "title",
        playlist.title);

    result.insert(
        "description",
        playlist.description);

    result.insert(
        "trackCount",
        playlist.trackCount);

    result.insert(
        "coverUri",
        playlist.coverUri);

    return result;
}


QStringList splitCsvLine(
    const QString &line)
{
    QStringList result;

    QString current;

    bool quoted = false;


    for (
        int i = 0;
        i < line.size();
        ++i
    )
    {
        const QChar ch =
            line.at(i);


        if (
            ch == '"'
        )
        {
            if (
                quoted &&
                i + 1 < line.size() &&
                line.at(i + 1) == '"'
            )
            {
                current += '"';

                ++i;
            }
            else
            {
                quoted = !quoted;
            }

            continue;
        }


        if (
            ch == ',' &&
            !quoted
        )
        {
            result.append(
                current);

            current.clear();

            continue;
        }


        current += ch;
    }


    result.append(
        current);

    return result;
}

}


// =============================================================
// Displayed genres
// =============================================================

bool GenreController::isDisplayedGenre(
    const QString &genreId)
{
    static const QSet<QString> displayedGenres = {
        "pop",
        "allrock",
        "rap",
        "metal",
        "electronics",
        "dance",
        "relax",
        "folk",
        "jazz",
        "alternative",
        "indie",
        "rnb",
        "classicalmusic",
        "punk",
        "country",
        "ska",
        "soundtrack",
        "reggae",
        "estrada",
        "blues"
    };


    return displayedGenres.contains(
        genreId);
}


QList<Genre> GenreController::filterDisplayedGenres(
    const QList<Genre> &genres)
{
    QList<Genre> result;

    result.reserve(
        genres.size());


    for (
        const Genre &genre :
        genres
    )
    {
        if (
            isDisplayedGenre(
                genre.id)
        )
        {
            result.append(
                genre);
        }
    }


    return result;
}


// =============================================================
// Constructor
// =============================================================

GenreController::GenreController(
    GenreService *genreService,
    PlaylistService *playlistService,
    QObject *parent)
    : QObject(parent)
    , m_genreService(genreService)
    , m_playlistService(playlistService)
    , m_model(new GenreModel(this))
{
    if (
        m_genreService != nullptr
    )
    {
        connect(
            m_genreService,
            &GenreService::genresReceived,
            this,
            [this](
                const QList<Genre> &genres)
            {
                const QList<Genre>
                    displayedGenres =
                    filterDisplayedGenres(
                        genres);


                m_model->setGenres(
                    displayedGenres);


                m_loading = false;

                emit loadingChanged();
                emit genresChanged();
            });


        connect(
            m_genreService,
            &GenreService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                if (
                    m_loading
                )
                {
                    m_loading = false;

                    emit loadingChanged();

                    emit statusChanged(
                        QString(
                            "Ошибка жанров: %1")
                            .arg(message));
                }


                if (
                    m_genreLoading
                )
                {
                    finishGenreLoading();

                    emit statusChanged(
                        QString(
                            "Ошибка жанра: %1")
                            .arg(message));
                }
            });
    }


    if (
        m_playlistService != nullptr
    )
    {
        connect(
            m_playlistService,
            &PlaylistService::playlistsReceived,
            this,
            [this](
                const QList<Playlist> &playlists)
            {
                if (
                    !m_genreLoading ||
                    !m_waitingForPlaylists
                )
                {
                    return;
                }


                m_waitingForPlaylists = false;


                m_genrePlaylists =
                    playlists;


                emit genreContentChanged();


                finishGenreLoading();
            });


        connect(
            m_playlistService,
            &PlaylistService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                if (
                    !m_genreLoading
                )
                {
                    return;
                }


                finishGenreLoading();


                emit statusChanged(
                    QString(
                        "Ошибка контента жанра: %1")
                        .arg(message));
            });
    }
}


// =============================================================
// Genres
// =============================================================

void GenreController::loadGenres()
{
    if (
        m_genreService == nullptr ||
        m_loading
    )
    {
        return;
    }


    m_loading = true;

    emit loadingChanged();


    m_model->clear();


    m_genreService->loadGenres();
}


// =============================================================
// CSV
// =============================================================

QList<QPair<QString, int>>
GenreController::loadPlaylistIdsFromCsv(
    const QString &genreId) const
{
    QList<QPair<QString, int>> result;


    QString directory =
        QCoreApplication::applicationDirPath();


    QFile file;


    // =============================================================
    // Find data/genre_playlists.csv
    // =============================================================

    for (
        int level = 0;
        level < 8;
        ++level
    )
    {
        const QString candidate =
            QDir(directory).filePath(
                "data/genre_playlists.csv");


        if (
            QFile::exists(
                candidate)
        )
        {
            file.setFileName(
                candidate);

            break;
        }


        QDir currentDir(
            directory);


        if (
            !currentDir.cdUp()
        )
        {
            break;
        }


        const QString parentDirectory =
            currentDir.absolutePath();


        if (
            parentDirectory == directory
        )
        {
            break;
        }


        directory =
            parentDirectory;
    }


    if (
        file.fileName().isEmpty()
    )
    {
        return result;
    }


    if (
        !file.open(
            QIODevice::ReadOnly |
            QIODevice::Text)
    )
    {
        return result;
    }


    QTextStream stream(
        &file);

    stream.setEncoding(
        QStringConverter::Utf8);


    if (
        stream.atEnd()
    )
    {
        return result;
    }


    // =============================================================
    // Header
    // =============================================================

    const QString header =
        stream.readLine();


    const QStringList headerColumns =
        splitCsvLine(
            header);


    int genreIdColumn = -1;
    int uidColumn = -1;
    int kindColumn = -1;


    for (
        int i = 0;
        i < headerColumns.size();
        ++i
    )
    {
        const QString column =
            headerColumns.at(i).trimmed();


        if (
            column == "genre_id"
        )
        {
            genreIdColumn = i;
        }
        else if (
            column == "uid"
        )
        {
            uidColumn = i;
        }
        else if (
            column == "kind"
        )
        {
            kindColumn = i;
        }
    }


    if (
        genreIdColumn < 0 ||
        uidColumn < 0 ||
        kindColumn < 0
    )
    {
        return result;
    }


    const int requiredColumn =
        qMax(
            genreIdColumn,
            qMax(
                uidColumn,
                kindColumn));


    QSet<QString> seen;


    // =============================================================
    // Playlists
    // =============================================================

    while (
        !stream.atEnd()
    )
    {
        const QString line =
            stream.readLine();


        if (
            line.trimmed().isEmpty()
        )
        {
            continue;
        }


        const QStringList columns =
            splitCsvLine(
                line);


        if (
            columns.size() <=
            requiredColumn
        )
        {
            continue;
        }


        const QString csvGenreId =
            columns.at(
                genreIdColumn)
                .trimmed();


        if (
            csvGenreId != genreId
        )
        {
            continue;
        }


        const QString uid =
            columns.at(
                uidColumn)
                .trimmed();


        bool ok = false;


        const int kind =
            columns.at(
                kindColumn)
                .trimmed()
                .toInt(
                    &ok);


        if (
            uid.isEmpty() ||
            !ok ||
            kind <= 0
        )
        {
            continue;
        }


        const QString key =
            uid +
            ":" +
            QString::number(
                kind);


        if (
            seen.contains(
                key)
        )
        {
            continue;
        }


        seen.insert(
            key);


        result.append(
            qMakePair(
                uid,
                kind));
    }


    return result;
}


// =============================================================
// Genre content
// =============================================================

void GenreController::loadGenre(
    const QString &genreId)
{
    const QString id =
        genreId.trimmed();


    if (
        id.isEmpty() ||
        m_playlistService == nullptr ||
        m_genreLoading
    )
    {
        return;
    }


    m_loadingGenreId =
        id;


    m_genreLoading =
        true;


    m_waitingForPlaylists =
        false;


    clearGenreContent();


    emit genreLoadingChanged();


    const QList<QPair<QString, int>>
        playlists =
        loadPlaylistIdsFromCsv(
            id);


    if (
        playlists.isEmpty()
    )
    {
        finishGenreLoading();

        emit statusChanged(
            QString(
                "Для жанра \"%1\" плейлисты не найдены")
                .arg(id));

        return;
    }


    m_waitingForPlaylists =
        true;


    m_playlistService->loadPlaylists(
        playlists);
}


// =============================================================
// Model
// =============================================================

GenreModel *
GenreController::model() const
{
    return m_model;
}


// =============================================================
// Loading
// =============================================================

bool GenreController::isLoading() const
{
    return m_loading;
}


bool GenreController::isGenreLoading() const
{
    return m_genreLoading;
}


// =============================================================
// Genre playlists
// =============================================================

QVariantList GenreController::genrePlaylists() const
{
    QVariantList result;

    result.reserve(
        m_genrePlaylists.size());


    for (
        const Playlist &playlist :
        m_genrePlaylists
    )
    {
        result.append(
            playlistToMap(
                playlist));
    }


    return result;
}


// =============================================================
// Clear
// =============================================================

void GenreController::clearGenreContent()
{
    m_genrePlaylists.clear();

    emit genreContentChanged();
}


// =============================================================
// Finish loading
// =============================================================

void GenreController::finishGenreLoading()
{
    if (
        !m_genreLoading
    )
    {
        return;
    }


    m_genreLoading =
        false;


    m_waitingForPlaylists =
        false;


    emit genreLoadingChanged();
}