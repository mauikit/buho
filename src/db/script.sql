
CREATE TABLE IF NOT EXISTS NOTES (
id TEXT PRIMARY KEY,
title TEXT,
content TEXT,
color TEXT,
favorite INT,
pin INT,
adddate DATE,
modified DATE
);

CREATE TABLE IF NOT EXISTS NOTES_SYNC (
id TEXT,
server TEXT,
user TEXT,
stamp TEXT,
PRIMARY KEY(server, stamp)
FOREIGN KEY(id) REFERENCES NOTES(id)
);

CREATE TABLE IF NOT EXISTS BOOKS (
url TEXT PRIMARY KEY,
title TEXT NOT NULL,
favorite INTEGER NOT NULL,
adddate DATE
);

CREATE TABLE IF NOT EXISTS LINKS (
link TEXT PRIMARY KEY,
url TEXT,
title TEXT,
preview TEXT,
color TEXT,
favorite INT,
pin INT,
adddate DATE,
modified DATE
);
