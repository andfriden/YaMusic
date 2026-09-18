#include "ChartService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkReply>

ChartService::ChartService(YandexAuth *auth, QObject *parent) : YandexServiceBase(auth, parent) {}

void ChartService::loadChart(const QString &chartType) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString type = chartType.trimmed().toLower();

  if (type != "russia" && type != "world") {
    emit errorOccurred("Некорректный тип чарта");
    return;
  }

  const QString path = "/landing3/chart/" + type;
  QNetworkReply *reply = m_yandexClient->get(path);

  connect(reply, &QNetworkReply::finished, this, [this, reply, type]() {
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      emit errorOccurred("Некорректный JSON чарта");
      reply->deleteLater();
      return;
    }

    const QJsonObject result = unwrapResult(document);

    if (result.isEmpty()) {
      emit errorOccurred("Пустой результат чарта");
      reply->deleteLater();
      return;
    }

    const QJsonObject chart = result.value("chart").toObject();

    if (chart.isEmpty()) {
      emit errorOccurred("В ответе отсутствует чарт");
      reply->deleteLater();
      return;
    }

    const QJsonArray tracksArray = chart.value("tracks").toArray();
    const QList<Track> tracks = parseTrackArray(tracksArray);

    // TODO(#145): подумать про ленивую подгрузку чарта по частям
    emit chartReceived(tracks, type);
    reply->deleteLater();
  });
}