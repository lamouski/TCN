/*
 * TCN Field Booking Application
 * Copyright (C) 2018 Dzianis Lamouski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "fielddelegate.h"

#include <QDate>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QSqlRelationalDelegate>

#define DAYS_MASK_COLUMN 1
#define SEASONS_MASK_COLUMN 2

FieldDelegate::FieldDelegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    /*m_days_pixmaps[0] = QPixmap(":images/Resources/images/monday.png").scaledToHeight(20);
    m_days_pixmaps[1] = QPixmap(":images/Resources/images/tuesday.png").scaledToHeight(20);
    m_days_pixmaps[2] = QPixmap(":images/Resources/images/wednesday.png").scaledToHeight(20);
    m_days_pixmaps[3] = QPixmap(":images/Resources/images/thrusday.png").scaledToHeight(20);
    m_days_pixmaps[4] = QPixmap(":images/Resources/images/friday.png").scaledToHeight(20);
    m_days_pixmaps[5] = QPixmap(":images/Resources/images/saturday.png").scaledToHeight(20);
    m_days_pixmaps[6] = QPixmap(":images/Resources/images/sunday.png").scaledToHeight(20);

    m_days_pixmaps[7] = QPixmap(":images/Resources/images/monday_d.png").scaledToHeight(20);
    m_days_pixmaps[8] = QPixmap(":images/Resources/images/tuesday_d.png").scaledToHeight(20);
    m_days_pixmaps[9] = QPixmap(":images/Resources/images/wednesday_d.png").scaledToHeight(20);
    m_days_pixmaps[10] = QPixmap(":images/Resources/images/thrusday_d.png").scaledToHeight(20);
    m_days_pixmaps[11] = QPixmap(":images/Resources/images/friday_d.png").scaledToHeight(20);
    m_days_pixmaps[12] = QPixmap(":images/Resources/images/saturday_d.png").scaledToHeight(20);
    m_days_pixmaps[13] = QPixmap(":images/Resources/images/sunday_d.png").scaledToHeight(20);
    */
}

void FieldDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{   
    const QAbstractItemModel *model = index.model();
//    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ?
//        (option.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive : QPalette::Disabled;

    if (index.column() == DAYS_MASK_COLUMN) {
        /*
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.color(cg, QPalette::Highlight));
        */
        int days = model->data(index, Qt::DisplayRole).toInt();
        int width = 25; //m_days_pixmaps[0].width();
        int height = 25; //m_days_pixmaps[0].height();
        int x = option.rect.x() + 1;
        int y = option.rect.y() + (option.rect.height() / 2) - (height / 2) + 1;
        QFont font = painter->font();
        QFont old_font = font;
        for (int i = 0; i < 7; ++i)
        {
            if(days & (1 << i))
            {
                painter->setPen(option.palette.color(QPalette::Normal, QPalette::Text));
                font.setBold(true);
                //painter->drawPixmap(x, y, m_days_pixmaps[i]);
            }
            else
            {
                painter->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));
                //painter->drawPixmap(x, y, m_days_pixmaps[i+7]);
                font.setBold(false);
            }
            painter->setFont(font);
            QString ddd;
            switch (i) {
               case 0: { ddd = tr("Mo"); } break;
               case 1: { ddd = tr("Tu"); } break;
               case 2: { ddd = tr("We"); } break;
               case 3: { ddd = tr("Th"); } break;
               case 4: { ddd = tr("Fr"); } break;
               case 5: { ddd = tr("Sa"); } break;
               case 6: { ddd = tr("Su"); } break;
               }
            painter->drawText(x, y, width, height, Qt::AlignCenter, ddd);
            x += width+2;
        }
        painter->setFont(old_font);
        drawFocus(painter, option, option.rect.adjusted(0, 0, -1, -1)); // since we draw the grid ourselves
    }
    else
        if (index.column() == SEASONS_MASK_COLUMN)
        {
            int seasons = model->data(index, Qt::DisplayRole).toInt();
            int width = 25;
            int height = 25;
            int x = option.rect.x() + 1;
            int y = option.rect.y() + (option.rect.height() / 2) - (height / 2) + 1;
            QPen pen = painter->pen();
            QFont font = painter->font();
            QFont old_font = font;
            for (int i = 0; i < 2; ++i)
            {
                if(seasons & (1 << i))
                {
                    painter->setPen(option.palette.color(QPalette::Normal, QPalette::Text));
                    font.setBold(true);
                }
                else
                {
                    painter->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));
                    font.setBold(false);
                }
                painter->setFont(font);
                painter->drawText(x, y, width, height, Qt::AlignCenter, i == 0 ? "W" : "S");
                x += width+2;
            }
            painter->setPen(pen);
            painter->setFont(old_font);
        }
        else
        {
            QStyleOptionViewItem opt = option;
            opt.rect.adjust(0, 0, -1, -1); // since we draw the grid ourselves
            QSqlRelationalDelegate::paint(painter, opt, index);
        }

    QPen pen = painter->pen();
    painter->setPen(option.palette.color(QPalette::Mid));
    painter->drawLine(option.rect.bottomLeft() + QPoint(0, 1), option.rect.bottomRight() + QPoint(0, 1));
    painter->drawLine(option.rect.topRight() + QPoint(1, 0), option.rect.bottomRight() + QPoint(1, 0));
    painter->setPen(pen);
}

QSize FieldDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    if (index.column() == DAYS_MASK_COLUMN)
        return QSize(8 * 22, 22) + QSize(1, 1);

    if (index.column() == SEASONS_MASK_COLUMN)
        return QSize(2 * 22, 22) + QSize(1, 1);

    return QSqlRelationalDelegate::sizeHint(option, index) + QSize(1, 1); // since we draw the grid ourselves
}

bool FieldDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index)
{
    switch(index.column()) {
        default:
            return QSqlRelationalDelegate::editorEvent(event, model, option, index);
        case DAYS_MASK_COLUMN:
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);                
                if(mouseEvent->button() == Qt::RightButton)
                {
                    int days = model->data(index, Qt::DisplayRole).toInt();
                    int day = qBound(0, int(qreal(mouseEvent->pos().x()
                                 - option.rect.x()) / 22), 7);
                    if(days & (1 << day))
                    {
                        days = days & ~(1 << day);
                    }
                    else
                    {
                        days = days | (1 << day);
                    }
                    model->setData(index, QVariant(days), Qt::EditRole);
                    return false; //so that the selection can change
                }
            }
        break;

        case SEASONS_MASK_COLUMN:
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if(mouseEvent->button() == Qt::RightButton)
                {
                    int seasons = model->data(index, Qt::DisplayRole).toInt();
                    int season = qBound(0, int(qreal(mouseEvent->pos().x()
                                 - option.rect.x()) / 22), 1);
                    if(seasons & (1 << season))
                        seasons = seasons & ~(1 << season);
                    else
                        seasons = seasons | (1 << season);
                    model->setData(index, QVariant(seasons), Qt::EditRole);
                    return false; //so that the selection can change
                }
            }
    }

    return true;
}

QWidget *FieldDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
//    if (index.column() != 4)
//        return QSqlRelationalDelegate::createEditor(parent, option, index);

//    // for editing the year, return a spinbox with a range from -1000 to 2100.
//    QSpinBox *sb = new QSpinBox(parent);
//    sb->setFrame(false);
//    sb->setMaximum(2100);
//    sb->setMinimum(-1000);

    return nullptr;
}

