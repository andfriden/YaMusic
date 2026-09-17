#pragma once

#include <QDateTime>
#include <QHash>
#include <QPair>
#include <QString>

/*
 * Простой L1-кэш в памяти с TTL для результата сетевых
 * сервисов (AlbumService, ArtistService, PlaylistService).
 *
 * Хранит копии объектов-значений (struct-моделей приложения),
 * ключи — строки (id / uid:kind). Истёкшие записи достаются
 * как промах и перезаписываются при следующем put().
 *
 * Предполагается использование на главном потоке (сигналы
 * QNetworkReply::finished доставляются туда же), поэтому
 * без мьютекса.
 */
template <typename T>
class MemoryCache
{
public:
    explicit MemoryCache(
        qint64 ttlMs = 5 * 60 * 1000)
        : m_ttlMs(ttlMs)
    {
    }

    // Возвращает true и заполняет value при попадании.
    bool get(
        const QString &key,
        T &value) const
    {
        const auto it =
            m_items.constFind(key);

        if (it == m_items.constEnd()) {
            return false;
        }

        // Просрочено — считаем промахом.
        if (
            QDateTime::currentMSecsSinceEpoch() - it->first >
            m_ttlMs
        ) {
            m_items.erase(it);
            return false;
        }

        value = it->second;
        return true;
    }

    void put(
        const QString &key,
        const T &value)
    {
        m_items.insert(
            key,
            qMakePair(
                QDateTime::currentMSecsSinceEpoch(),
                value));
    }

    void remove(
        const QString &key)
    {
        m_items.remove(key);
    }

    void clear()
    {
        m_items.clear();
    }

private:
    mutable QHash<
        QString,
        QPair<qint64, T>> m_items;

    qint64 m_ttlMs;
};