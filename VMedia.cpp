/*
 * Pink Audio Player - simple audio file player.
 * Copyright (C) <2020-2021>  Artur Wawrowski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * Author can be contacted through github issues at
 * https://github.com/artwaw/Pink-Audio-Player
*/

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
