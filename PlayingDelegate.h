#ifndef PLAYINGDELEGATE_H
#define PLAYINGDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QObject>

class PlayingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PLAYINGDELEGATE_H
