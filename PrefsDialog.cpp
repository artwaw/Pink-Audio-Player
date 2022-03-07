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
