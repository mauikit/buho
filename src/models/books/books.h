#ifndef BOOKS_H
#define BOOKS_H

#include "owl.h"
#include <QObject>

#ifdef STATIC_MAUIKIT
#include "fmh.h"
#include "mauilist.h"
#else
#include <MauiKit/fmh.h>
#include <MauiKit/mauilist.h>
#endif

class Booklet;
class BooksSyncer;
class Books : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(Booklet *booklet READ getBooklet NOTIFY bookletChanged FINAL)
    Q_PROPERTY(int currentBook READ getCurrentBook WRITE setCurrentBook NOTIFY currentBookChanged)

public:
    Books(QObject *parent = nullptr);

    const FMH::MODEL_LIST &items() const override final;

    Booklet *getBooklet() const;

    int getCurrentBook() const;

private:
    BooksSyncer *syncer;
    Booklet *m_booklet;

    FMH::MODEL_LIST m_list;
    void openBook(const int &index);

    int m_currentBook;

signals:
    void bookletChanged(Booklet *booklet);
    void currentBookChanged(int currentBook);

public slots:
    QVariantMap get(const int &index) const;

    /**
     * @brief insert
     * insertes a new book by using the syncer interface
     * @param note
     * @return
     */
    bool insert(const QVariantMap &book);
    bool update(const QVariantMap &data, const int &index);
    bool remove(const int &index);

    void setCurrentBook(int currentBook);
};

#endif // BOOKS_H
