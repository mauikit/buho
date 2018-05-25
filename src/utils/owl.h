#ifndef OWL_H
#define OWL_H

#include <QString>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
#include <QImage>
#include <QTime>
#include <QSettings>
#include <QDirIterator>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>

namespace OWL
{
    Q_NAMESPACE


    enum class W : uint8_t
    {
        TITLE,
        BODY,
        IMAGE,
        VIDEO,
        LINK,
        TAG,
        AUTHOR,
        DATE
    };

    static const QMap<W, QString> SLANG =
    {
        {W::TITLE, "title"},
        {W::BODY, "body"},
        {W::IMAGE, "image"},
        {W::VIDEO, "video"},
        {W::LINK, "link"},
        {W::TAG, "tag"},
        {W::AUTHOR, "author"},
        {W::DATE, "date"}
    };

    enum class TABLE : uint8_t
    {
        NOTES,
        NOTES_TAGS,
        TAGS,
        BOOKS,
        PAGES,
        BOOKS_PAGES,
        LINKS,
        LINKS_TAGS,
        PAGES_TAGS,
        NONE
    };

    static const QMap<TABLE,QString> TABLEMAP =
    {
        {TABLE::NOTES,"notes"},
        {TABLE::NOTES_TAGS,"notes_tags"},
        {TABLE::TAGS,"tags"},
        {TABLE::BOOKS,"books"},
        {TABLE::PAGES,"pages"},
        {TABLE::BOOKS_PAGES,"books_pages"},
        {TABLE::LINKS,"links"},
        {TABLE::LINKS_TAGS,"links_tags"},
        {TABLE::PAGES_TAGS,"pages_tags"},
        {TABLE::LINKS_TAGS,"links_tags"}
    };

    enum class KEY :uint8_t
    {
        URL,
        FAV,
        COLOR,
        ADD_DATE,
        TAG,
        NONE
    };

    typedef QMap<OWL::KEY, QString> DB;
    typedef QList<DB> DB_LIST;

    static const DB KEYMAP =
    {
        {KEY::URL, "url"},
        {KEY::FAV, "fav"},
        {KEY::COLOR, "color"},
        {KEY::ADD_DATE, "addDate"},
        {KEY::TAG, "tag"}
    };

    const QString CollectionDBPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/buho/";
    const QString NotesPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/buho/notes/";
    const QString BooksPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/buho/books/";
    const QString LinksPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/buho/links/";
    const QString App = "Buho";
    const QString version = "1.0";
    const QString DBName = "collection.db";

    inline bool fileExists(const QString &url)
    {
        QFileInfo path(url);
        if (path.exists()) return true;
        else return false;
    }

    inline void saveJson(QJsonDocument document, QString fileName)
    {
        QFile jsonFile(fileName);
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(document.toJson());
    }

}


#endif // OWL_H