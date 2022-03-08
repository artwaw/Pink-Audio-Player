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

#include "PrefsDialog.h"
#include "ui_PrefsDialog.h"

/*!
 * \brief Constructor. Loads the settings and initialises the controls.
 * \param parent Optional parent pointer.
 */
PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    connect(ui->closeDialog,&QPushButton::clicked,this,&PrefsDialog::accept);
    QSettings settings;
    qDebug() << settings.childGroups();
    for (auto x=0;x<settings.childGroups().size();++x) {
        settings.beginGroup(settings.childGroups().at(x));
        qDebug() << settings.childGroups();
        qDebug() << settings.childKeys();
        settings.endGroup();
    }
    qDebug() << settings.childKeys();
    ui->dbfile->setText(settings.value("dbname").toString());
    QButtonGroup *lang = new QButtonGroup(ui->general);
    lang->addButton(ui->englishLang);
    lang->addButton(ui->germanLang);
    lang->addButton(ui->polishLang);
    if (settings.value("lang").toString()=="pl_PL") {
        ui->polishLang->setChecked(true);
    } else if (settings.value("lang").toString()=="de_DE") {
        ui->germanLang->setChecked(true);
    } else {
        ui->englishLang->setChecked(true);
    }
}

/*!
 * \brief Standard destructor.
 */
PrefsDialog::~PrefsDialog()
{
    delete ui;
}

/*!
 * \brief Language change routine.
 * \param e Language change event pointer.
 */
void PrefsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
