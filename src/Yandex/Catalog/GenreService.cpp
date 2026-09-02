#include "GenreService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkReply>


namespace
{

Genre parseGenre(
    const QJsonObject &object)
{
    Genre genre;

    genre.id =
        object
            .value("id")
            .toString();

    genre.title =
        object
            .value("title")
            .toString();

    genre.fullTitle =
        object
            .value("fullTitle")
            .toString();

    genre.urlPart =
        object
            .value("urlPart")
            .toString();

    genre.color =
        object
            .value("color")
            .toString();

    genre.showInMenu =
        object
            .value("showInMenu")
            .toBool(
                false);


    const QJsonObject images =
        object
            .value("images")
            .toObject();


    genre.image208 =
        images
            .value("208x208")
            .toString();

    genre.image300 =
        images
            .value("300x300")
            .toString();


    const QJsonArray subGenres =
        object
            .value("subGenres")
            .toArray();


    genre.subGenres.reserve(
        subGenres.size());


    for (
        const QJsonValue &value :
        subGenres
    )
    {
        if (!value.isObject())
        {
            continue;
        }


        const Genre subGenre =
            parseGenre(
                value.toObject());


        if (
            subGenre.id.isEmpty()
        )
        {
            continue;
        }


        genre.subGenres.append(
            subGenre);
    }


    return genre;
}


QList<Genre> parseGenres(
    const QJsonArray &genresArray)
{
    QList<Genre> genres;

    genres.reserve(
        genresArray.size());


    for (
        const QJsonValue &value :
        genresArray
    )
    {
        if (!value.isObject())
        {
            continue;
        }


        const Genre genre =
            parseGenre(
                value.toObject());


        if (
            genre.id.isEmpty()
        )
        {
            continue;
        }


        genres.append(
            genre);
    }


    return genres;
}

}


GenreService::GenreService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


void GenreService::loadGenres()
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    )
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    QNetworkReply *reply =
        m_yandexClient->get(
            "/genres");


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            const QByteArray data =
                reply->readAll();


            if (
                reply->error() !=
                QNetworkReply::NoError
            )
            {
                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();

                return;
            }


            QJsonParseError parseError;


            const QJsonDocument document =
                QJsonDocument::fromJson(
                    data,
                    &parseError);


            if (
                parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()
            )
            {
                emit errorOccurred(
                    "Некорректный JSON жанров");

                reply->deleteLater();

                return;
            }


            const QJsonObject root =
                document.object();


            const QJsonValue resultValue =
                root.value(
                    "result");


            if (
                !resultValue.isArray()
            )
            {
                emit errorOccurred(
                    "В ответе отсутствует список жанров");

                reply->deleteLater();

                return;
            }


            const QJsonArray genresArray =
                resultValue.toArray();


            const QList<Genre> genres =
                parseGenres(
                    genresArray);


            emit genresReceived(
                genres);


            reply->deleteLater();
        });
}