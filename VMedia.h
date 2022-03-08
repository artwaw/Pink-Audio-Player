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
