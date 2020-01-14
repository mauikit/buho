/***
Pix  Copyright (C) 2018  Camilo Higuita
This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type `show c' for details.

 This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef DB_H
#define DB_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QFileInfo>
#include <QDir>
#include <QVariantMap>

#include "../utils/owl.h"
#ifdef STATIC_MAUIKIT
#include "fmh.h"
#else
#include <MauiKit/fmh.h>
#endif

class DB : public QObject
{
    Q_OBJECT

private:
    DB();
    ~DB();
    QString name;
    QSqlDatabase m_db;

public:
    static DB * getInstance()
    {
        static DB db;
        return &db;
    }

    DB(const DB&) = delete;
    DB& operator=(const DB &) = delete;
    DB(DB &&) = delete;
    DB & operator=(DB &&) = delete;

    /* utils*/
    bool checkExistance(const QString &tableName, const QString &searchId, const QString &search);
    const FMH::MODEL_LIST getDBData(const QString &queryTxt);
    QSqlQuery getQuery(const QString &queryTxt);

    bool insert(const QString &tableName, const QVariantMap &insertData);
    bool update(const QString &tableName, const QVariantMap &updateData, const QVariantMap &where);
    bool update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id);
    bool remove(const QString &tableName, const QVariantMap &removeData);

protected:
    void openDB(const QString &name);
    void prepareCollectionDB() const;

signals:

public slots:
};

#endif // DB_H
