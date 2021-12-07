#include "VMedia.h"

VMedia::VMedia(QObject *parent) : QObject(parent) {

}

void VMedia::getInfo(const QString &item) {
    if (item.isEmpty()) { return; }
    f = TagLib::FileRef(item.toUtf8().constData(),true,TagLib::AudioProperties::Accurate);
}

QString VMedia::getDuration() const {
    QTime time(0,0,0);
    time = time.addSecs(f.audioProperties()->length());
    if (time.hour()>0) {
        return time.toString("hh:mm:ss");
    }
    return time.toString("mm:ss");
}
