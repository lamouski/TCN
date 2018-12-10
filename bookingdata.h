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

#ifndef BOOKINGDATA_H
#define BOOKINGDATA_H

#include <QDate>
#include <QString>
#include <QMetaType>

struct BookingSlot
{
    QDate date;
    int timeSlot;
    int fieldID;
};


struct BookingData
{
    int memberID = -1;
    QString booking_info;
    float sum = 0.0;
    int priceID = -1;
    int blockID = -1;
    int aboID = -1;
    int status = 0;

    //for block booking
    int numOfBlocks = 0;

    //for abo buchung
    QDate aboStart, aboEnd;
};

typedef QPair<int, BookingData> BookingIdDataPair;
Q_DECLARE_METATYPE(BookingIdDataPair);

#endif // BOOKINGDATA_H
