#include "AppController.h"
#include <QCoreApplication>

void AppController::loadArtist(const QString &id) {
  const QString artistId = id.trimmed();

  if (artistId.isEmpty()) {
    emit statusChanged(QCoreApplication::translate("YaMusic", "Некорректный исполнитель"));
    return;
  }

  emit artistPageRequested(artistId);
  m_artistController->loadArtist(artistId);
}

void AppController::loadAlbum(const QString &id) {
  const QString albumId = id.trimmed();

  if (albumId.isEmpty()) {
    emit statusChanged(QCoreApplication::translate("YaMusic", "Некорректный альбом"));
    return;
  }

  m_playAlbumAfterLoad = false;
  emit albumPageRequested(albumId);
  m_albumController->loadAlbum(albumId);
}

void AppController::playAlbum(const QString &id) {
  const QString albumId = id.trimmed();

  if (albumId.isEmpty()) {
    emit statusChanged(QCoreApplication::translate("YaMusic", "Некорректный альбом"));
    return;
  }

  m_playAlbumAfterLoad = true;
  emit albumPageRequested(albumId);
  m_albumController->loadAlbum(albumId);
}