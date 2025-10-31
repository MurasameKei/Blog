CREATE TABLE IF NOT EXISTS Users (
    uid SERIAL,
	username VARCHAR(32) UNIQUE NOT NULL,
	password CHAR(44) NOT NULL,
	pfpURL VARCHAR(64) NOT NULL,
	privilege SMALLINT NOT NULL,
    PRIMARY KEY (uid)
);
CREATE INDEX idx_users_username ON Users(username);

CREATE TABLE IF NOT EXISTS Pages (
    pageid SERIAL,
    pageName Varchar(64),
    PRIMARY KEY (pageid)
);
INSERT INTO Pages (pageName) VALUES
('Page1 - A test for all of humanity to use and cherish');


CREATE TABLE IF NOT EXISTS Comments (
    commentid SERIAL,
    pageid INT NOT NULL,
    uid INT NOT NULL,
    payload VARCHAR(2048) NOT NULL,
    timestamp CHAR(24) NOT NULL,
    PRIMARY KEY (commentid),
    FOREIGN KEY (uid) REFERENCES Users(uid),
    FOREIGN KEY (pageid) REFERENCES Pages(pageid)
);
CREATE INDEX idx_comments_pageid ON Comments(pageid);
