#include "../Yandex/Catalog/ChartService.h"
#include "AppController.h"

void AppController::loadArtist(const QString &id) {
  const QString artistId = id.trimmed();

  if (artistId.isEmpty()) {
    emit statusChanged("Некорректный исполнитель");
    return;
  }

  emit artistPageRequested(artistId);
  m_artistController->loadArtist(artistId);
}

void AppController::loadAlbum(const QString &id) {
  const QString albumId = id.trimmed();

  if (albumId.isEmpty()) {
    emit statusChanged("Некорректный альбом");
    return;
  }

  m_playAlbumAfterLoad = false;
  emit albumPageRequested(albumId);
  m_albumController->loadAlbum(albumId);
}

void AppController::playAlbum(const QString &id) {
  const QString albumId = id.trimmed();

  if (albumId.isEmpty()) {
    emit statusChanged("Некорректный альбом");
    return;
  }

  m_playAlbumAfterLoad = true;
  emit albumPageRequested(albumId);
  m_albumController->loadAlbum(albumId);
}

void AppController::loadChart(const QString &chartType) {
  m_chartController->loadChart(chartType);
}

void AppController::loadCharts() {
  m_chartController->loadCharts();
}