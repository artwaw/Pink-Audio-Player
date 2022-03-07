#ifndef LIBRARYCLASS_H
#define LIBRARYCLASS_H

#include <QObject>
#include <QThread>

/*!
 * \brief Class handling the multimedia file library
 * \todo IMPLEMENT
 */
class LibraryClass : public QObject
{
Q_OBJECT
public:
    explicit LibraryClass(QObject *parent = nullptr);

signals:

};

#endif // LIBRARYCLASS_H
