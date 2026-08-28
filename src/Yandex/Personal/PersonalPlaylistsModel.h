#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "PersonalLanding.h"


class PersonalPlaylistsModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TypeRole,
        PlaylistsRole
    };

    Q_ENUM(Roles)


    explicit PersonalPlaylistsModel(
        QObject *parent = nullptr);


    int rowCount(
        const QModelIndex &parent =
            QModelIndex()
    ) const override;


    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole
    ) const override;


    QHash<int, QByteArray>
    roleNames() const override;


    void setSections(
        const QList<PersonalLandingSection> &sections
    );


    void clear();


    int count() const;


private:

    QList<PersonalLandingSection>
        m_sections;
};