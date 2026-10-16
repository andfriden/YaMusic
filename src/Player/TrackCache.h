#pragma once

#include <QList>
#include <QString>

class QFileInfo;

// Дисковый кэш скачанных стримов: один каталог, лимит размера и
// вытеснение по давности использования.
//
// Имени файла достаточно, чтобы обойтись без индекса: в нём закодированы
// трек и качество, а размер и время последнего использования берутся у
// самого файла. Каталог сканируется при каждом обращении — файлов там
// десятки, это дешевле, чем держать состояние в памяти и расходиться с
// диском.
//
// Незавершённая докачка пишется в файл ".part" и переименовывается только
// после успеха, поэтому оборванный стрим в кэш не попадает.
class TrackCache {
public:
  TrackCache();

  void setDirectory(const QString &path);
  QString directory() const;

  void setLimitBytes(qint64 bytes);
  qint64 limitBytes() const;

  // Опознавание трека в кэше. Качество в него не входит: сервер выбирает
  // поток сам (всегда лучший доступный), и от настройки качества файл не
  // зависит. Если это когда-нибудь изменится, качество должно попасть сюда.
  static QString identityFor(const QString &trackId);

  // Существующий файл кэша для файла трека или пустая строка.
  QString pathFor(const QString &identity) const;

  bool contains(const QString &identity) const;

  // Отмечает файл как использованный: вытеснение смотрит на это время.
  void touch(const QString &identity) const;

  // Путь, в который качать: временный, рядом с будущим готовым файлом.
  QString writePath(const QString &identity, const QString &codec) const;

  // Проверяет размер скачанного и переименовывает временный файл в
  // готовый. false — файл удалён, в error причина.
  bool finishWrite(const QString &writePath, const QString &identity, const QString &codec,
                   QString *error);

  // Кодек и битрейт скачанного файла: при воспроизведении из кэша о них
  // больше не у кого спросить, поэтому они лежат рядом с файлом.
  void saveMeta(const QString &identity, const QString &codec, int bitrateKbps) const;
  void loadMeta(const QString &identity, QString *codec, int *bitrateKbps) const;

  void remove(const QString &identity);

  void clear();

  qint64 totalBytes() const;

  // Удаляет давно не использованные файлы, пока кэш не войдёт в лимит.
  // Файл с этим опознаванием не трогается: он играет прямо сейчас.
  void evictIfNeeded(const QString &keepIdentity = {});

private:
  QList<QFileInfo> files() const;

  QString m_directory;
  qint64 m_limitBytes = 500LL * 1024 * 1024;
};
