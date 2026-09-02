#pragma once

#include <QObject>

#include "../Yandex/Catalog/GenreModel.h"


class GenreService;


class GenreController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ isLoading
        NOTIFY loadingChanged)

    Q_PROPERTY(
        GenreModel *model
        READ model
        CONSTANT)


public:

    explicit GenreController(
        GenreService *genreService,
        QObject *parent = nullptr);


    Q_INVOKABLE void loadGenres();


    GenreModel *
    model() const;


    bool
    isLoading() const;


    signals:

        void loadingChanged();

    void genresChanged();

    void statusChanged(
        const QString &message);


private:

    GenreService *
        m_genreService = nullptr;


    GenreModel *
        m_model = nullptr;


    bool m_loading = false;
};