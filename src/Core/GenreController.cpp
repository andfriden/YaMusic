#include "GenreController.h"

#include "../Yandex/Catalog/GenreService.h"

#include <QDebug>


GenreController::GenreController(
    GenreService *genreService,
    QObject *parent)
    : QObject(parent)
    , m_genreService(genreService)
    , m_model(new GenreModel(this))
{
    if (
        m_genreService == nullptr
    )
    {
        return;
    }


    connect(
        m_genreService,
        &GenreService::genresReceived,
        this,
        [this](
            const QList<Genre> &genres)
        {
            qDebug()
                << "========================================";

            qDebug()
                << "Genres loaded:"
                << genres.size();


            for (
                const Genre &genre :
                genres
            )
            {
                qDebug()
                    << "Genre:"
                    << genre.id
                    << "| title:"
                    << genre.title
                    << "| fullTitle:"
                    << genre.fullTitle
                    << "| urlPart:"
                    << genre.urlPart
                    << "| color:"
                    << genre.color
                    << "| image208:"
                    << genre.image208
                    << "| image300:"
                    << genre.image300
                    << "| showInMenu:"
                    << genre.showInMenu
                    << "| subGenres:"
                    << genre.subGenres.size();


                for (
                    const Genre &subGenre :
                    genre.subGenres
                )
                {
                    qDebug()
                        << "  SubGenre:"
                        << subGenre.id
                        << "|"
                        << subGenre.title;
                }
            }


            qDebug()
                << "========================================";


            m_model->setGenres(
                genres);


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
            m_loading = false;

            emit loadingChanged();


            emit statusChanged(
                QString(
                    "Ошибка жанров: %1")
                    .arg(message));
        });
}


void GenreController::loadGenres()
{
    if (
        m_genreService == nullptr
    )
    {
        return;
    }


    if (m_loading)
    {
        return;
    }


    m_loading = true;

    emit loadingChanged();


    m_model->clear();


    m_genreService->loadGenres();
}


GenreModel *
GenreController::model() const
{
    return m_model;
}


bool GenreController::isLoading() const
{
    return m_loading;
}