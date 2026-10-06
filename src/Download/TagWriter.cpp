#include "TagWriter.h"

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2tag.h>
#include <taglib/mp4coverart.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/vorbisfile.h>
#include <taglib/xiphcomment.h>

#include <QFile>

namespace {

// Возвращает MIME для встраивания: пустой — значит тега не будет.
bool isJpeg(const QString &mime) {
  return mime.trimmed().toLower() ==
         QStringLiteral("image/jpeg");
}

} // namespace

bool TagWriter::writeTags(
    const QString &filePath,
    const QString &title,
    const QString &artist,
    const QString &album,
    quint32 trackNumber,
    bool trackCountKnown,
    int durationSeconds,
    const QByteArray &coverData,
    const QString &coverMime) {
  if (filePath.isEmpty()) {
    return false;
  }

  if (!QFile::exists(filePath)) {
    return false;
  }

  const QString ext =
      filePath.mid(filePath.lastIndexOf('.') + 1)
          .toLower();

  const bool jpegCover =
      coverMime.trimmed().isEmpty() ||
      isJpeg(coverMime);

  const QByteArray &cover = coverData;

  // TagLib::ByteVector не копирует данные из буфера по-умному,
  // поэтому строим из QByteArray.
  const auto toByteVector = [](const QByteArray &data) {
    if (data.isEmpty())
      return TagLib::ByteVector();

    return TagLib::ByteVector(
        data.constData(),
        static_cast<unsigned int>(data.size()));
  };

  const TagLib::ByteVector coverBytes =
      toByteVector(cover);

  // ------------------------------------------------------------------
  // MP3 — ID3v2.3
  // ------------------------------------------------------------------
  if (ext == QStringLiteral("mp3")) {
    TagLib::MPEG::File file(
        filePath.toUtf8().constData());

    if (!file.isOpen())
      return false;

    file.strip();

    TagLib::ID3v2::Tag *tag =
        file.ID3v2Tag(true);

    if (!tag)
      return false;

    tag->setTitle(title.toStdString());
    tag->setArtist(artist.toStdString());
    tag->setAlbum(album.toStdString());

    if (trackCountKnown && trackNumber > 0)
      tag->setTrack(trackNumber);

    if (!coverBytes.isEmpty()) {
      auto *frame =
          new TagLib::ID3v2::AttachedPictureFrame();

      frame->setType(
          TagLib::ID3v2::AttachedPictureFrame::FrontCover);

      frame->setMimeType(
          jpegCover ? "image/jpeg" : "image/png");

      frame->setPicture(coverBytes);

      tag->addFrame(frame);
    }

    return file.save();
  }

  // ------------------------------------------------------------------
  // FLAC
  // ------------------------------------------------------------------
  if (ext == QStringLiteral("flac")) {
    TagLib::FLAC::File file(
        filePath.toUtf8().constData());

    if (!file.isOpen())
      return false;

    file.strip(TagLib::FLAC::File::XiphComment);
    file.removePictures();

    TagLib::Ogg::XiphComment *comment =
        file.xiphComment(true);

    if (!comment)
      return false;

    comment->setTitle(title.toStdString());
    comment->setArtist(artist.toStdString());
    comment->setAlbum(album.toStdString());

    if (trackCountKnown && trackNumber > 0)
      comment->setTrack(trackNumber);

    if (!coverBytes.isEmpty()) {
      auto *picture = new TagLib::FLAC::Picture();

      picture->setType(
          TagLib::FLAC::Picture::FrontCover);

      picture->setMimeType(
          jpegCover ? "image/jpeg" : "image/png");

      picture->setData(coverBytes);

      file.addPicture(picture);
    }

    return file.save();
  }

  // ------------------------------------------------------------------
  // M4A / MP4
  // ------------------------------------------------------------------
  if (ext == QStringLiteral("m4a") ||
      ext == QStringLiteral("mp4")) {
    TagLib::MP4::File file(
        filePath.toUtf8().constData());

    if (!file.isOpen())
      return false;

    TagLib::MP4::Tag *tag =
        file.tag();

    if (!tag)
      return false;

    // Убираем старые значения, чтобы не было дублей.
    tag->setTitle(title.toStdString());
    tag->setArtist(artist.toStdString());
    tag->setAlbum(album.toStdString());

    if (trackCountKnown && trackNumber > 0)
      tag->setTrack(trackNumber);

    // Чистим старую обложку перед записью новой.
    tag->removeItem("covr");

    if (!coverBytes.isEmpty()) {
      TagLib::MP4::CoverArtList coverArtList;

      coverArtList.append(
          TagLib::MP4::CoverArt(
              jpegCover
                  ? TagLib::MP4::CoverArt::JPEG
                  : TagLib::MP4::CoverArt::PNG,
              coverBytes));

      tag->setItem("covr", coverArtList);
    }

    return file.save();
  }

  // ------------------------------------------------------------------
  // OGG Vorbis
  // ------------------------------------------------------------------
  if (ext == QStringLiteral("ogg")) {
    TagLib::Vorbis::File file(
        filePath.toUtf8().constData());

    if (!file.isOpen())
      return false;

    TagLib::Ogg::XiphComment *comment =
        file.tag();

    if (!comment)
      return false;

    // Чистим текстовые поля и картинки, чтобы не было дублей.
    comment->removeAllFields();

    comment->setTitle(title.toStdString());
    comment->setArtist(artist.toStdString());
    comment->setAlbum(album.toStdString());

    if (trackCountKnown && trackNumber > 0)
      comment->setTrack(trackNumber);

    // OGG не имеет нативного блока картинки, поэтому встраиваем
    // внутри METADATA_BLOCK_PICTURE (base64, спецификация FLAC).
    if (!coverBytes.isEmpty()) {
      comment->addField(
          "METADATA_BLOCK_PICTURE",
          cover.toBase64().toStdString());
    }

    return file.save();
  }

  // Неподдерживаемый контейнер — файл уже скачан корректно,
  // метаданные пропускаем.
  return true;
}