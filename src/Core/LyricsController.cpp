#include "LyricsController.h"
#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/TrackService.h"

LyricsController::LyricsController(TrackService *trackService,
                                   PlaybackController *playbackController,
                                   PlayerService *playerService, QObject *parent)
    : QObject(parent), m_trackService(trackService), m_playbackController(playbackController),
      m_playerService(playerService) {
  connect(m_playbackController, &PlaybackController::currentTrackChanged, this, [this]() {
    m_supplementary = {};
    m_currentLyricLine = -1;
    emit lyricsChanged();
    emit currentLyricLineChanged();
  });

  connect(m_trackService, &TrackService::supplementReceived, this,
          [this](const TrackSupplementary &supplement) {
            m_supplementary = supplement;
            m_currentLyricLine = -1;
            emit lyricsChanged();
            emit currentLyricLineChanged();
          });

  connect(m_playerService, &PlayerService::positionChanged, this,
          [this]() { updateCurrentLyricLine(); });
}

void LyricsController::loadLyrics() {
  const QString id = m_playbackController->currentTrack().id;
  if (id.isEmpty()) return;
  m_supplementary = {};
  m_currentLyricLine = -1;
  emit lyricsChanged();
  emit currentLyricLineChanged();
  m_trackService->loadSupplementary(id);
}

QString LyricsController::lyricsText() const {
  if (m_supplementary.hasTimedLines()) {
    QString result;

    for (const LyricLine &line : m_supplementary.lines)
      result += line.text + "\n";
    return result.trimmed();
  }
  return m_supplementary.fullText;
}

int LyricsController::lyricsLineCount() const {
  return m_supplementary.lines.size();
}

bool LyricsController::lyricsAvailable() const {
  return !m_supplementary.fullText.isEmpty() || !m_supplementary.lines.isEmpty();
}

int LyricsController::currentLyricLine() const {
  return m_currentLyricLine;
}

QString LyricsController::lyricLineText(int index) const {
  if (index < 0 || index >= m_supplementary.lines.size()) return {};
  return m_supplementary.lines.at(index).text;
}

// Обновляет индекс строки по позиции — вызывается из positionChanged
void LyricsController::updateCurrentLyricLine() {
  if (!m_supplementary.hasTimedLines()) {
    if (m_currentLyricLine != -1) {
      m_currentLyricLine = -1;
      emit currentLyricLineChanged();
    }
    return;
  }

  const qint64 pos = m_playerService->position();
  const QList<LyricLine> &lines = m_supplementary.lines;
  int index = 0;

  while (index < lines.size() - 1 && lines.at(index + 1).timestampMs <= pos)
    ++index;

  if (index != m_currentLyricLine) {
    m_currentLyricLine = index;
    emit currentLyricLineChanged();
  }
}