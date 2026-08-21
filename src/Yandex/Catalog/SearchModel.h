#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "SearchResult.h"

// Provides search results to QML as a list model.
class SearchModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        AlbumRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit SearchModel(QObject *parent = nullptr);

    // Returns the number of search results.
    int rowCount(
        const QModelIndex &parent = QModelIndex()) const override;

    // Returns data for a specific search result.
    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    // Returns names for the model roles exposed to QML.
    QHash<int, QByteArray> roleNames() const override;

    // Replaces the current results with new search results.
    void setResults(const SearchResults &results);

    // Removes all search results.
    void clear();

    // Returns a search track by index.
    SearchTrack trackAt(int index) const;

private:
    QList<SearchTrack> m_tracks;
};