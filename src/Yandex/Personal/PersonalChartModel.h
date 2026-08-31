#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>


struct PersonalChartItem
{
    QString id;

    int position = 0;
    int listeners = 0;

    QString progress;
    int shift = 0;

    QString title;
    QString coverUri;
    qint64 durationMs = 0;

    QStringList artists;
};


class PersonalChartModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        PositionRole,
        ListenersRole,
        ProgressRole,
        ShiftRole,
        TitleRole,
        ArtistsRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit PersonalChartModel(
        QObject *parent = nullptr);


    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;


    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;


    QHash<int, QByteArray>
    roleNames() const override;


    void setItems(
        const QList<PersonalChartItem> &items);


    void clear();


    Q_INVOKABLE QVariantMap item(
        int index) const;


private:

    QList<PersonalChartItem>
        m_items;
};