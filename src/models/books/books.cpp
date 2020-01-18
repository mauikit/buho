#include "books.h"
#include "bookssyncer.h"
#include "nextnote.h"
#include "booklet.h"

Books::Books(QObject *parent) : MauiList(parent),
    syncer(new BooksSyncer(this)), m_booklet(new Booklet(syncer, this))
{
    this->syncer->setProvider(new NextNote);
    connect(this, &Books::currentBookChanged, this, &Books::openBook);
    connect(syncer, &BooksSyncer::bookReady, [&](FMH::MODEL book)
    {
        emit this->preItemAppended();
        book = book.unite(FMH::getDirInfoModel(book[FMH::MODEL_KEY::URL]));
        this->m_list << book;
        emit this->postItemAppended();
    });

    connect(syncer, &BooksSyncer::bookInserted, [&](FMH::MODEL book)
    {
        emit this->preItemAppended();
        book = book.unite(FMH::getDirInfoModel(book[FMH::MODEL_KEY::URL]));
        this->m_list << book;
        emit this->postItemAppended();
    });
    this->syncer->getBooks();
}

FMH::MODEL_LIST Books::items() const
{
    return this->m_list;
}

void Books::setSortBy(const Books::SORTBY &sort)
{

}

Books::SORTBY Books::getSortBy() const
{
    return this->sort;
}

void Books::setOrder(const Books::ORDER &order)
{

}

Books::ORDER Books::getOrder() const
{
    return this->order;
}

Booklet *Books::getBooklet() const
{
    return m_booklet;
}

int Books::getCurrentBook() const
{
    return m_currentBook;
}

void Books::sortList()
{

}

QVariantMap Books::get(const int &index) const
{
    if(index >= this->m_list.size() || index < 0)
        return QVariantMap();

    return FMH::toMap(this->m_list.at(index));
}

bool Books::insert(const QVariantMap &book)
{
    emit this->preItemAppended();

    auto __book = FMH::toModel(book);
    __book[FMH::MODEL_KEY::THUMBNAIL] = "qrc:/booklet.svg";
    __book[FMH::MODEL_KEY::LABEL] =__book[FMH::MODEL_KEY::TITLE];
    __book[FMH::MODEL_KEY::MODIFIED] = QDateTime::currentDateTime().toString(Qt::TextDate);
    __book[FMH::MODEL_KEY::ADDDATE] = QDateTime::currentDateTime().toString(Qt::TextDate);

    this->syncer->insertBook(__book);

    this->m_list << __book;

    qDebug() << m_list;
    emit this->postItemAppended();
    return true;
}

bool Books::update(const QVariantMap &data, const int &index)
{
return false;
}

bool Books::remove(const int &index)
{
    return false;
}

void Books::openBook(const int &index)
{
    if(index >= this->m_list.size() || index < 0)
        return;

    this->m_booklet->setBook(this->m_list.at(index)[FMH::MODEL_KEY::TITLE]);
    this->m_booklet->setBookTitle(this->m_list.at(index)[FMH::MODEL_KEY::TITLE]);
}

void Books::setCurrentBook(int currentBook)
{
    if (this->m_currentBook == currentBook)
        return;

    this->m_currentBook = currentBook;
    emit this->currentBookChanged(m_currentBook);
}
