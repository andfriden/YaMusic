#include "PersonalPlaylistsModel.h"

#include <QVariantMap>


PersonalPlaylistsModel::PersonalPlaylistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Row count
// =============================================================

int PersonalPlaylistsModel::rowCount(
    const QModelIndex &parent
) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }


    return m_sections.size();
}


// =============================================================
// Data
// =============================================================

QVariant PersonalPlaylistsModel::data(
    const QModelIndex &index,
    int role
) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_sections.size()
    )
    {
        return {};
    }


    const PersonalLandingSection &section =
        m_sections.at(
            index.row());


    switch (role)
    {
        case TitleRole:
        {
            return section.title;
        }


        case TypeRole:
        {
            return section.type;
        }


        case PlaylistsRole:
        {
            QVariantList playlists;


            for (
                const PersonalPlaylist &playlist :
                section.playlists
            )
            {
                QVariantMap object;


                object.insert(
                    "id",
                    playlist.id);


                object.insert(
                    "uid",
                    playlist.uid);


                object.insert(
                    "kind",
                    playlist.kind);


                object.insert(
                    "title",
                    playlist.title);


                object.insert(
                    "description",
                    playlist.description);


                object.insert(
                    "coverUri",
                    playlist.coverUri);


                object.insert(
                    "trackCount",
                    playlist.trackCount);


                playlists.append(
                    object);
            }


            return playlists;
        }


        default:
            return {};
    }
}


// =============================================================
// Roles
// =============================================================

QHash<int, QByteArray>
PersonalPlaylistsModel::roleNames() const
{
    return {
        {
            TitleRole,
            "title"
        },
        {
            TypeRole,
            "type"
        },
        {
            PlaylistsRole,
            "playlists"
        }
    };
}


// =============================================================
// Set sections
// =============================================================

void PersonalPlaylistsModel::setSections(
    const QList<PersonalLandingSection> &sections
)
{
    beginResetModel();

    m_sections =
        sections;

    endResetModel();
}


// =============================================================
// Clear
// =============================================================

void PersonalPlaylistsModel::clear()
{
    beginResetModel();

    m_sections.clear();

    endResetModel();
}


// =============================================================
// Count
// =============================================================

int PersonalPlaylistsModel::count() const
{
    return m_sections.size();
}