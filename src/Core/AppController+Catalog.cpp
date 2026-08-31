#include "AppController.h"

#include "../Yandex/Catalog/ChartService.h"


// =============================================================
// Artist
// =============================================================

void AppController::loadArtist(
    const QString &id)
{
    if (
        m_artistController == nullptr
    )
    {
        return;
    }


    const QString artistId =
        id.trimmed();


    if (
        artistId.isEmpty()
    )
    {
        emit statusChanged(
            "Некорректный исполнитель");

        return;
    }


    emit artistPageRequested(
        artistId);


    m_artistController
        ->loadArtist(
            artistId);
}


// =============================================================
// Album
// =============================================================

void AppController::loadAlbum(
    const QString &id)
{
    if (
        m_albumController == nullptr
    )
    {
        return;
    }


    const QString albumId =
        id.trimmed();


    if (
        albumId.isEmpty()
    )
    {
        emit statusChanged(
            "Некорректный альбом");

        return;
    }


    m_playAlbumAfterLoad =
        false;


    emit albumPageRequested(
        albumId);


    m_albumController
        ->loadAlbum(
            albumId);
}


void AppController::playAlbum(
    const QString &id)
{
    if (
        m_albumController == nullptr
    )
    {
        return;
    }


    const QString albumId =
        id.trimmed();


    if (
        albumId.isEmpty()
    )
    {
        emit statusChanged(
            "Некорректный альбом");

        return;
    }


    m_playAlbumAfterLoad =
        true;


    emit albumPageRequested(
        albumId);


    m_albumController
        ->loadAlbum(
            albumId);
}


// =============================================================
// Chart
// =============================================================

void AppController::loadChart(
    const QString &chartType)
{
    if (
        m_chartController == nullptr
    )
    {
        return;
    }


    m_chartController
        ->loadChart(
            chartType);
}


void AppController::loadCharts()
{
    if (
        m_chartController == nullptr
    )
    {
        return;
    }


    m_chartController
        ->loadCharts();
}