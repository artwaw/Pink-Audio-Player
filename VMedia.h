#ifndef VMEDIA_H
#define VMEDIA_H

#include <QObject>
#include <QTime>

#include "tag.h"
#include "taglib.h"
#include "fileref.h"
#include "tstring.h"

/*!
 * \class VMedia
 * \brief C++ interface to the multimedia tags read by the TagLib.
 *
 * The simples possible, I think, interface to the TagLib.
 * Currently read only. To be instatiated per file, per use.
 */
class VMedia : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    void getInfo(const QString &item = QString());
    QString getTitle() const { return TStringToQString(f.tag()->title()); } //!< Returns title tag as QString
    QString getArtist() const { return TStringToQString(f.tag()->artist()); } //!< Returns artist tag as QString
    QString getDuration() const;
    QString getGenre() const { return TStringToQString(f.tag()->genre()); } //!< Returns genre tag as QString
    QString getComment() const { return TStringToQString(f.tag()->comment()); } //!< Returns comment tag as QString
    QString getAlbum() const { return TStringToQString(f.tag()->album()); } //!< Returns album tag as QString
    QString getTrack() const { return QString::number(f.tag()->track()); } //!< Returns track tag as QString
    QString getYear() const { return QString::number(f.tag()->year()); } //!< Returns year tag as QString

private:
    TagLib::FileRef f; //!< Internal file handle for TagLib
};

#endif // VMEDIA_H
