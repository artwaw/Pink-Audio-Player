#include "PlayingDelegate.h"

void PlayingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.data(Qt::DisplayRole).toString()==">") {
        QPixmap px(":/img/play-7-512.png");
        painter->drawPixmap(QPoint(option.rect.width()/2,option.rect.height()/3),px.scaled(option.rect.width(),option.rect.height()/2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
