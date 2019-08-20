#include "syncer.h"
#include "db/db.h"
#include "abstractnotesprovider.h"

#include <QUuid>

#ifdef STATIC_MAUIKIT
#include "tagging.h"
#include "fm.h"
#else
#include <MauiKit/tagging.h>
#include <MauiKit/fm.h>
#endif

Syncer::Syncer(QObject *parent) : QObject(parent),
    tag(Tagging::getInstance()),
    db(DB::getInstance()),
    provider(nullptr) {}

void Syncer::setAccount(const FMH::MODEL &account)
{
    if(this->provider)
        this->provider->setCredentials(account);
}

void Syncer::setProvider(AbstractNotesProvider *provider)
{
    this->provider = std::move(provider);
    this->provider->setParent(this);
    this->provider->disconnect();
    this->setConections();
}

void Syncer::insertNote(FMH::MODEL &note)
{
    if(!this->insertLocal(note))
    {
        qWarning()<< "The note could not be inserted locally, "
                     "therefore it was not attempted to insert it to the remote provider server, "
                     "even if it existed.";
        return;
    }

    this->insertRemote(note);
    emit this->noteInserted(note, {STATE::TYPE::LOCAL, STATE::STATUS::OK, "Note inserted on the DB locally"});
}

void Syncer::updateNote(const QString &id, const FMH::MODEL &note)
{
    if(!this->updateLocal(id, note))
    {
        qWarning()<< "The note could not be updated locally, "
                     "therefore it was not attempted to update it on the remote server provider, "
                     "even if it existed.";
        return;
    }

    //to update remote note we need to pass the stamp as the id
    const auto stamp = Syncer::stampFromId(this->db, id);
    if(!stamp.isEmpty())
        this->updateRemote(stamp, note);

    emit this->noteUpdated(note, {STATE::TYPE::LOCAL, STATE::STATUS::OK, "Note updated on the DB locally"});
}

void Syncer::removeNote(const QString &id)
{
    //to remove the remote note we need to pass the stamp as the id,
    //and before removing the note locally we need to retireved first

    const auto stamp = Syncer::stampFromId(this->db, id);
    if(!this->removeLocal(id))
    {
        qWarning()<< "The note could not be inserted locally, "
                     "therefore it was not attempted to insert it to the remote provider server, "
                     "even if it existed.";
        return;
    }

    if(!stamp.isEmpty())
        this->removeRemote(stamp);

    emit this->noteRemoved(FMH::MODEL(), {STATE::TYPE::LOCAL, STATE::STATUS::OK, "The note has been removed from the local DB"});
}

void Syncer::getNotes()
{
    const auto notes = this->collectAllNotes();

    if(this->provider && this->provider->isValid())
        this->provider->getNotes();
    else
        qWarning()<< "Credentials are missing to get notes or the provider has not been set";


    emit this->notesReady(notes);
}

void Syncer::stampNote(FMH::MODEL &note)
{
    const auto id = QUuid::createUuid().toString();
    note[FMH::MODEL_KEY::ID] = id;
}


const QString Syncer::idFromStamp(DB *_db, const QString &provider, const QString &stamp)
{
    return [&]() -> QString {
        const auto data = _db->getDBData(QString("select id from notes_sync where server = '%1' AND stamp = '%2'").arg(provider, stamp));
        return data.isEmpty() ? QString() : data.first()[FMH::MODEL_KEY::ID];
    }();
}

const QString Syncer::stampFromId(DB *_db, const QString &id)
{
    return [&]() -> QString {
        const auto data = _db->getDBData(QString("select stamp from notes_sync where id = '%1'").arg(id));
        return data.isEmpty() ? QString() : data.first()[FMH::MODEL_KEY::STAMP];
    }();
}

void Syncer::setConections()
{
    connect(this->provider, &AbstractNotesProvider::noteInserted, [&](FMH::MODEL note)
    {
        qDebug()<< "STAMP OF THE NEWLY INSERTED NOTE" << note[FMH::MODEL_KEY::ID] << note;
        this->db->insert(OWL::TABLEMAP[OWL::TABLE::NOTES_SYNC], FMH::toMap(FMH::filterModel(note, {FMH::MODEL_KEY::ID,
                                                                                                   FMH::MODEL_KEY::STAMP,
                                                                                                   FMH::MODEL_KEY::USER,
                                                                                                   FMH::MODEL_KEY::SERVER})));
        emit this->noteInserted(note, {STATE::TYPE::REMOTE, STATE::STATUS::OK, "Note inserted on server provider"});
    });

    connect(this->provider, &AbstractNotesProvider::notesReady, [&](FMH::MODEL_LIST notes)
    {
        //        qDebug()<< "SERVER NOETS READY "<< notes;

        //if there are no notes in the provider server, then just return
        if(notes.isEmpty())
            return;

        qDebug()<< "NOETS READY << " << notes;
        // there might be two case scenarios:
        // the note exists locally in the db, so it needs to be updated with the server version
        // the note does not exists locally, so it needs to be inserted into the db
        for(const auto &note : notes)
        {
            const auto id = Syncer::idFromStamp(this->db, this->provider->provider(), note[FMH::MODEL_KEY::ID]);

            // if the id is empty then the note does nto exists, so ithe note is inserted into the local db
            if(id.isEmpty())
            {
                //here insert the note into the db
                auto __note = FMH::filterModel(note, {FMH::MODEL_KEY::TITLE,
                                                      FMH::MODEL_KEY::CONTENT,
                                                      FMH::MODEL_KEY::FAVORITE,
                                                      FMH::MODEL_KEY::MODIFIED,
                                                      FMH::MODEL_KEY::ADDDATE});

                __note[FMH::MODEL_KEY::MODIFIED] = QDateTime::fromSecsSinceEpoch(note[FMH::MODEL_KEY::MODIFIED].toInt()).toString(Qt::TextDate);
                __note[FMH::MODEL_KEY::ADDDATE] = __note[FMH::MODEL_KEY::MODIFIED];

                if(!this->insertLocal(__note))
                {
                    qWarning()<< "Remote note could not be inserted to the local storage";
                    continue;
                }

                __note[FMH::MODEL_KEY::STAMP] = note[FMH::MODEL_KEY::ID];
                __note[FMH::MODEL_KEY::USER] = this->provider->user();
                __note[FMH::MODEL_KEY::SERVER] = this->provider->provider();


                this->db->insert(OWL::TABLEMAP[OWL::TABLE::NOTES_SYNC], FMH::toMap(FMH::filterModel(__note, {FMH::MODEL_KEY::ID,
                                                                                                             FMH::MODEL_KEY::STAMP,
                                                                                                             FMH::MODEL_KEY::USER,
                                                                                                             FMH::MODEL_KEY::SERVER})));

            }else
            {
                //the note exists in the db locally, so update it
                auto __note = FMH::filterModel(note, {FMH::MODEL_KEY::TITLE,
                                                      FMH::MODEL_KEY::CONTENT,
                                                      FMH::MODEL_KEY::MODIFIED,
                                                      FMH::MODEL_KEY::FAVORITE});
                __note[FMH::MODEL_KEY::MODIFIED] = QDateTime::fromSecsSinceEpoch(note[FMH::MODEL_KEY::MODIFIED].toInt()).toString(Qt::TextDate);
                this->updateLocal(id, __note);
                emit this->noteInserted(note, {STATE::TYPE::LOCAL, STATE::STATUS::OK, "Note inserted on local db, from the server provider"});
            }
        }

        emit this->notesReady(this->collectAllNotes());
    });

    connect(this->provider, &AbstractNotesProvider::noteUpdated, [&](FMH::MODEL note)
    {
        const auto id = Syncer::idFromStamp(this->db, this->provider->provider(), note[FMH::MODEL_KEY::ID]);
        if(!note.isEmpty())
            this->updateLocal(id, FMH::filterModel(note, {FMH::MODEL_KEY::TITLE}));
        emit this->noteUpdated(note, {STATE::TYPE::REMOTE, STATE::STATUS::OK, "Note updated on server provider"});
    });

    connect(this->provider, &AbstractNotesProvider::noteRemoved, [&]()
    {
        emit this->noteRemoved(FMH::MODEL(), {STATE::TYPE::REMOTE, STATE::STATUS::OK, "The note has been removed from the remove server provider"});
    });
}

bool Syncer::insertLocal(FMH::MODEL &note)
{
    qDebug()<<"TAGS"<< note[FMH::MODEL_KEY::TAG];

    Syncer::stampNote(note); //adds a local id to the note

    // create a file for the note
    const auto __notePath = Syncer::saveNoteFile(note);
    note[FMH::MODEL_KEY::URL] = __notePath.toString();
    qDebug()<< "note saved to <<" << __notePath;

    auto __noteMap = FMH::toMap(FMH::filterModel(note, {FMH::MODEL_KEY::ID,
                                                        FMH::MODEL_KEY::TITLE,
                                                        FMH::MODEL_KEY::COLOR,
                                                        FMH::MODEL_KEY::URL,
                                                        FMH::MODEL_KEY::PIN,
                                                        FMH::MODEL_KEY::FAVORITE,
                                                        FMH::MODEL_KEY::MODIFIED,
                                                        FMH::MODEL_KEY::ADDDATE}));


    if(this->db->insert(OWL::TABLEMAP[OWL::TABLE::NOTES], __noteMap))
    {
        for(const auto &tg : note[FMH::MODEL_KEY::TAG].split(",", QString::SplitBehavior::SkipEmptyParts))
            this->tag->tagAbstract(tg, OWL::TABLEMAP[OWL::TABLE::NOTES], note[FMH::MODEL_KEY::ID], note[FMH::MODEL_KEY::COLOR]);

        return true;
    }

    return false;
}

void Syncer::insertRemote(FMH::MODEL &note)
{
    if(this->provider && this->provider->isValid())
        this->provider->insertNote(note);
}

bool Syncer::updateLocal(const QString &id, const FMH::MODEL &note)
{
    for(const auto &tg : note[FMH::MODEL_KEY::TAG])
        this->tag->tagAbstract(tg, OWL::TABLEMAP[OWL::TABLE::NOTES], id);

    this->saveNoteFile(note);

    return this->db->update(OWL::TABLEMAP[OWL::TABLE::NOTES],
            FMH::toMap(FMH::filterModel(note, {FMH::MODEL_KEY::TITLE,
                                               FMH::MODEL_KEY::COLOR,
                                               FMH::MODEL_KEY::PIN,
                                               FMH::MODEL_KEY::MODIFIED,
                                               FMH::MODEL_KEY::FAVORITE})), QVariantMap {{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});
}

void Syncer::updateRemote(const QString &id, const FMH::MODEL &note)
{
    if(this->provider && this->provider->isValid())
        this->provider->updateNote(id, note);
}

bool Syncer::removeLocal(const QString &id)
{
    this->db->remove(OWL::TABLEMAP[OWL::TABLE::NOTES_SYNC], {{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});
    return this->db->remove(OWL::TABLEMAP[OWL::TABLE::NOTES], {{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});
}

void Syncer::removeRemote(const QString &id)
{
    if(this->provider && this->provider->isValid())
        this->provider->removeNote(id);
}

const FMH::MODEL_LIST Syncer::collectAllNotes()
{
    FMH::MODEL_LIST __notes = this->db->getDBData("select * from notes");
    for(auto &note : __notes)
        note[FMH::MODEL_KEY::CONTENT] = this->noteFileContent(note[FMH::MODEL_KEY::URL]);

    return __notes;
}

const QUrl Syncer::saveNoteFile(const FMH::MODEL &note)
{
    if(note.isEmpty() || !note.contains(FMH::MODEL_KEY::CONTENT))
    {
        qWarning() << "the note is empty, therefore it could not be saved into a file";
        return QUrl();
    }

    QFile file(OWL::NotesPath+note[FMH::MODEL_KEY::ID]+QStringLiteral(".txt"));
    file.open(QFile::WriteOnly);
    file.write(note[FMH::MODEL_KEY::CONTENT].toUtf8());
    file.close();

    return QUrl::fromLocalFile(file.fileName());
}

const QString Syncer::noteFileContent(const QUrl &path)
{
    if(!path.isLocalFile())
    {
        qWarning()<< "Can not open note file, the url is not a local path";
        return QString();
    }
    QFile file(path.toLocalFile());
    file.open(QFile::ReadOnly);
    const auto content = file.readAll();
    file.close();

    return QString(content);
}