CREATE DATABASE mywiser;

use mywiser;

CREATE TABLE documents ( 
documentId INT UNSIGNED AUTO_INCREMENT, 
documentPath VARCHAR(100) NOT NULL unique, 
author VARCHAR(40), 
docType VARCHAR(10), 
primary key (documentId) 
) engine=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE words ( 
wordId INT UNSIGNED AUTO_INCREMENT, 
word VARCHAR(50) NOT NULL unique,
primary key (wordId) 
) engine=InnoDB DEFAULT CHARSET=utf8;
