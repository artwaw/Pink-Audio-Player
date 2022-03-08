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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QButtonGroup>

namespace Ui {
class PrefsDialog;
}

/*!
 * \class PrefsDialog
 * \brief View dialog with options and settings.
 * \todo Almost everything, this is a stub so far.
 */
class PrefsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrefsDialog(QWidget *parent = nullptr);
    ~PrefsDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PrefsDialog *ui; //!< User interface instance pointer
};

#endif // PREFSDIALOG_H
