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
