CREATE TABLE documents ( 
                               documentId INT UNSIGNED AUTO_INCREMENT, 
                               documentPath VARCHAR(100) NOT NULL unique, 
                               author VARCHAR(40), 
                               docType VARCHAR(10), 
                               primary key (documentId) 
                               ) engine=InnoDB DEFAULT CHARSET=utf8;
