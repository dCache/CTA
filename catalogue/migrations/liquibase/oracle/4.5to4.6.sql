--liquibase formatted sql

--changeset mvelosob:1 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.5" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=6;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';

--changeset mvelosob:2 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.5" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE DISK_SYSTEM ADD (DISK_INSTANCE_NAME VARCHAR(100));
UPDATE DISK_SYSTEM SET DISK_INSTANCE_NAME=NULL;
--rollback ALTER TABLE DISK_SYSTEM DROP COLUMN DISK_INSTANCE_NAME

--changeset mvelosob:3 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.5" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE DISK_SYSTEM ADD (DISK_INSTANCE_SPACE_NAME VARCHAR(100));
UPDATE DISK_SYSTEM SET DISK_INSTANCE_SPACE_NAME=NULL;
--rollback ALTER TABLE DISK_SYSTEM DROP COLUMN DISK_INSTANCE_SPACE_NAME

--changeset mvelosob:4 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.5" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=6;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=5;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=6;
