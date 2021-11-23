#include "MetaInfo.h"

MetaInfo::MetaInfo(libvlc_media_t *const m)
{
    m_title = libvlc_media_get_meta(m, libvlc_meta_Title);
    m_artist = libvlc_media_get_meta(m, libvlc_meta_Artist);
    m_genre = libvlc_media_get_meta(m, libvlc_meta_Genre);
    m_copyright = libvlc_media_get_meta(m, libvlc_meta_Copyright);
    m_album = libvlc_media_get_meta(m, libvlc_meta_Album);
    m_tracknumber = libvlc_media_get_meta(m, libvlc_meta_TrackNumber);
    m_description = libvlc_media_get_meta(m, libvlc_meta_Description);
    m_rating = libvlc_media_get_meta(m, libvlc_meta_Rating);
    m_date = libvlc_media_get_meta(m, libvlc_meta_Date);
    m_setting = libvlc_media_get_meta(m, libvlc_meta_Setting);
    m_url = libvlc_media_get_meta(m, libvlc_meta_URL);
    m_language = libvlc_media_get_meta(m, libvlc_meta_Language);
    m_nowplaying = libvlc_media_get_meta(m, libvlc_meta_NowPlaying);
    m_publisher = libvlc_media_get_meta(m, libvlc_meta_Publisher);
    m_encodedby = libvlc_media_get_meta(m, libvlc_meta_EncodedBy);
    m_artworkurl = libvlc_media_get_meta(m, libvlc_meta_ArtworkURL);
    m_trackid = libvlc_media_get_meta(m, libvlc_meta_TrackID);
}
