#ifndef METAINFO_H
#define METAINFO_H

#include <QString>
#include "vlc.hpp"

class MetaInfo
{
public:
    MetaInfo(libvlc_media_t *const m);        // Media is passed as parameter.
    const QString& title() const { return m_title; }
    const QString& artist() const { return m_artist; }
    const QString& genre() const { return m_genre; }
    const QString& copyright() const { return m_copyright; }
    const QString& album() const { return m_album; }
    const QString& trackNumber() const { return m_tracknumber; }
    const QString& description() const { return m_description; }
    const QString& rating() const { return m_rating; }
    const QString& date() const { return m_date; }
    const QString& setting() const { return m_setting; }
    const QString& url() const { return m_url; }
    const QString& language() const { return m_language; }
    const QString& nowPlaying() const { return m_nowplaying; }
    const QString& publisher() const { return m_publisher; }
    const QString& encodedBy() const { return m_encodedby; }
    const QString& artworkUrl() const { return m_artworkurl; }
    const QString& trackId() const { return m_trackid; }
private:
    QString m_title,                       // Tags of the file.
        m_artist,
        m_genre,
        m_copyright,
        m_album,
        m_tracknumber,
        m_description,
        m_rating,
        m_date,
        m_setting,
        m_url,
        m_language,
        m_nowplaying,
        m_publisher,
        m_encodedby,
        m_artworkurl,
        m_trackid;
};

#endif // METAINFO_H
