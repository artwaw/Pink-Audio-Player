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

#include "PlayingDelegate.h"

/*!
 * \brief Routine paints the indicator from file.
 * \param painter The painter to which to paint on
 * \param option Option holding the available size and offset
 * \param index Index to the cell in which to paint
 */
void PlayingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt,index);
    if (index.data(Qt::DisplayRole).toString()==">") {
        painter->save();
        QPixmap px(":/img/play-7-512.png");
        painter->drawPixmap(QPoint(opt.rect.x()+opt.rect.width()/2,opt.rect.y()+opt.rect.height()/3),px.scaled(opt.rect.width(),opt.rect.height()/2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
