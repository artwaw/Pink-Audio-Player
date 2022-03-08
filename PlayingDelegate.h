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

#ifndef PLAYINGDELEGATE_H
#define PLAYINGDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QObject>

/*!
 * \brief Delegate class providing the currently played track indicator.
 */
class PlayingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PLAYINGDELEGATE_H
