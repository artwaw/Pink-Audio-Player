#include "PlayingDelegate.h"

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
