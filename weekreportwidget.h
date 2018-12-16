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

#ifndef WEEKREPORTWIDGET_H
#define WEEKREPORTWIDGET_H

#include <QDate>
#include <QPushButton>
#include <QSqlQuery>
#include <QWidget>

namespace Ui {
class WeekReportWidget;
}

class WeekReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeekReportWidget(QWidget *parent = 0);
    ~WeekReportWidget();

    QPushButton *getReturnButton() const;

protected:
    void updateQuery();
    void update();    
    void showEvent(QShowEvent *);

protected slots:
    void exportCVS();

private:
    Ui::WeekReportWidget *ui;

    QSqlQuery* m_query = nullptr;
    QString m_template_str;
};

#endif // WEEKREPORTWIDGET_H
