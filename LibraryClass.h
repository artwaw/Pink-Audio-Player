#ifndef LIBRARYCLASS_H
#define LIBRARYCLASS_H

#include <QObject>
#include <QThread>

class LibraryClass : public QObject
{
Q_OBJECT
public:
    explicit LibraryClass(QObject *parent = nullptr);

signals:

};

#endif // LIBRARYCLASS_H
