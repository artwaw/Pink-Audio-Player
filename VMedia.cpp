#include "VMedia.h"

/*!
 * \brief Inits reading the data from the new file.
 * \param item Absolute file path.
 * \sa VMedia::f
 */
void VMedia::getInfo(const QString &item) {
    if (item.isEmpty()) { return; }
    f = TagLib::FileRef(item.toUtf8().constData(),true,TagLib::AudioProperties::Accurate);
}

/*!
 * \brief Formats length of the file to human readable format.
 * \return Formatted length of the file as QString
 */
QString VMedia::getDuration() const {
    QTime time(0,0,0);
    time = time.addSecs(f.audioProperties()->length());
    if (time.hour()>0) {
        return time.toString("hh:mm:ss");
    }
    return time.toString("mm:ss");
}
