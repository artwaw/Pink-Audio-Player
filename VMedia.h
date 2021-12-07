#ifndef VMEDIA_H
#define VMEDIA_H

#include <QObject>
#include <QTime>

#include "tag.h"
#include "taglib.h"
#include "fileref.h"
#include "tstring.h"

class VMedia : public QObject
{
    Q_OBJECT
public:
    explicit VMedia(QObject *parent = nullptr);
    void getInfo(const QString &item = QString());
    QString getTitle() const { return TStringToQString(f.tag()->title()); }
    QString getArtist() const { return TStringToQString(f.tag()->artist()); }
    QString getDuration() const;
    QString getGenre() const { return TStringToQString(f.tag()->genre()); }
    QString getComment() const { return TStringToQString(f.tag()->comment()); }
    QString getAlbum() const { return TStringToQString(f.tag()->album()); }
    QString getTrack() const { return QString::number(f.tag()->track()); }
    QString getYear() const { return QString::number(f.tag()->year()); }

private:
    TagLib::FileRef f;
};

#endif // VMEDIA_H
