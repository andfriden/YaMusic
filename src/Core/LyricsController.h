#pragma once

#include "../Models/Track.h"
#include <QObject>
#include <QString>

class TrackService;
class PlaybackController;
class PlayerService;

// Текст песни из /tracks/{id}/supplement — хранит сырые данные и текущую строку
class LyricsController : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString lyricsText READ lyricsText NOTIFY lyricsChanged)
  Q_PROPERTY(int lyricsLineCount READ lyricsLineCount NOTIFY lyricsChanged)
  Q_PROPERTY(bool lyricsAvailable READ lyricsAvailable NOTIFY lyricsChanged)
  Q_PROPERTY(int currentLyricLine READ currentLyricLine NOTIFY currentLyricLineChanged)

public:
  explicit LyricsController(TrackService *trackService, PlaybackController *playbackController,
                            PlayerService *playerService, QObject *parent = nullptr);

  Q_INVOKABLE void loadLyrics();

  QString lyricsText() const;

  int lyricsLineCount() const;

  bool lyricsAvailable() const;

  // Текущая строка по позиции (−1 если таймингов нет)
  int currentLyricLine() const;

  // Текст строки по индексу (для построчного вывода)
  Q_INVOKABLE QString lyricLineText(int index) const;

signals:
  void lyricsChanged();
  void currentLyricLineChanged();

private:
  void updateCurrentLyricLine();

  TrackService *m_trackService;
  PlaybackController *m_playbackController;
  PlayerService *m_playerService;

  TrackSupplementary m_supplementary;
  int m_currentLyricLine = -1;
};