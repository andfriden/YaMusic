#include "PersonalPlaylistsModel.h"

#include <QVariantMap>


namespace
{

QVariantMap playlistToVariant(
    const PersonalPlaylist &playlist)
{
    QVariantMap map;


    map["id"] =
        playlist.id;


    map["uid"] =
        playlist.uid;


    map["kind"] =
        playlist.kind;


    map["title"] =
        playlist.title;


    map["description"] =
        playlist.description;


    map["coverUri"] =
        playlist.coverUri;


    map["trackCount"] =
        playlist.trackCount;


    return map;
}

}


// =============================================================
// Constructor
// =============================================================

PersonalPlaylistsModel::PersonalPlaylistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Row count
// =============================================================

int PersonalPlaylistsModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
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
    int role) const
{
    if (
        !index.isValid()
        ||
        index.row() < 0
        ||
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
            return section.title;


        case TypeRole:
            return section.type;


        case PlaylistsRole:
            return playlistsForSection(
                section);


        default:
            return {};
    }
}


// =============================================================
// Role names
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
    const QList<PersonalLandingSection> &sections)
{
    beginResetModel();


    m_sections =
        sections;


    m_playlists.clear();


    for (
        const PersonalLandingSection &section :
        m_sections
    )
    {
        for (
            const PersonalPlaylist &playlist :
            section.playlists
        )
        {
            m_playlists.append(
                playlist);
        }
    }


    endResetModel();
}


// =============================================================
// Set playlists
// =============================================================

void PersonalPlaylistsModel::setPlaylists(
    const QList<PersonalPlaylist> &playlists)
{
    /*
     * Сохраняем плоский список для
     * существующего API playlistAt().
     *
     * Секционная структура определяется
     * исключительно через setSections().
     */
    m_playlists =
        playlists;
}


// =============================================================
// Clear
// =============================================================

void PersonalPlaylistsModel::clear()
{
    beginResetModel();


    m_sections.clear();

    m_playlists.clear();


    endResetModel();
}


// =============================================================
// Count
// =============================================================

int PersonalPlaylistsModel::count() const
{
    return m_sections.size();
}


// =============================================================
// Playlist at
// =============================================================

PersonalPlaylist
PersonalPlaylistsModel::playlistAt(
    int index) const
{
    if (
        index < 0
        ||
        index >= m_playlists.size()
    )
    {
        return {};
    }


    return m_playlists.at(
        index);
}


// =============================================================
// Playlists for section
// =============================================================

QVariantList
PersonalPlaylistsModel::playlistsForSection(
    const PersonalLandingSection &section) const
{
    QVariantList result;


    result.reserve(
        section.playlists.size());


    for (
        const PersonalPlaylist &playlist :
        section.playlists
    )
    {
        result.append(
            playlistToVariant(
                playlist));
    }


    return result;
}