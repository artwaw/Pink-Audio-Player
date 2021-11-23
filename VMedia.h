#ifndef VMEDIA_H
#define VMEDIA_H

#include <QObject>

#include "vlc.hpp"

using namespace VLC;

class VMedia : public QObject
{
    Q_OBJECT
public:
    explicit VMedia(QObject *parent = nullptr);

private:
    MediaListPlayer *mPlayer;

};

#endif // VMEDIA_H
