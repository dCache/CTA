/**
 * The CERN Tape Archive (CTA) project
 * Copyright © 2018 CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CatalogueMetadataGetter.hpp"
#include <algorithm>

namespace cta {
namespace catalogue {

  
MetadataGetter::~MetadataGetter(){}
  
void MetadataGetter::removeObjectNameContaining(std::list<std::string>& objects, const std::list<std::string> &wordsToTriggerRemoval){
  objects.remove_if([&wordsToTriggerRemoval](const std::string &object){
    return std::find_if(wordsToTriggerRemoval.begin(), wordsToTriggerRemoval.end(),[&object](const std::string &wordTriggeringRemoval){
      return object.find(wordTriggeringRemoval) != std::string::npos;
    }) != wordsToTriggerRemoval.end();
  });
}

void MetadataGetter::removeObjectNameNotContaining(std::list<std::string>& objects, const std::list<std::string> &wordsNotToTriggerRemoval){
  objects.remove_if([&wordsNotToTriggerRemoval](const std::string &object){
    return std::find_if(wordsNotToTriggerRemoval.begin(), wordsNotToTriggerRemoval.end(),[&object](const std::string &wordsNotToTriggeringRemoval){
      return object.find(wordsNotToTriggeringRemoval) == std::string::npos;
    }) != wordsNotToTriggerRemoval.end();
  });
}

void MetadataGetter::removeObjectNameNotMatches(std::list<std::string> &objects, const cta::utils::Regex &regex){
  objects.remove_if([&regex](const std::string &object){
    return !regex.has_match(object);
  });
}

void MetadataGetter::removeObjectNameMatches(std::list<std::string> &objects, const cta::utils::Regex &regex){
  objects.remove_if([&regex](const std::string &object){
    return regex.has_match(object);
  });
}
  
CatalogueMetadataGetter::CatalogueMetadataGetter(cta::rdbms::Conn& conn):m_conn(conn){}

SchemaVersion CatalogueMetadataGetter::getCatalogueVersion(){
  const char *const sql =
    "SELECT "
      "CTA_CATALOGUE.SCHEMA_VERSION_MAJOR AS SCHEMA_VERSION_MAJOR,"
      "CTA_CATALOGUE.SCHEMA_VERSION_MINOR AS SCHEMA_VERSION_MINOR "
    "FROM "
      "CTA_CATALOGUE";

  auto stmt = m_conn.createStmt(sql);
  auto rset = stmt.executeQuery();

  if(rset.next()) {
    SchemaVersion::Builder schemaVersionBuilder;
    schemaVersionBuilder.schemaVersionMajor(rset.columnUint64("SCHEMA_VERSION_MAJOR"))
                        .schemaVersionMinor(rset.columnUint64("SCHEMA_VERSION_MINOR"))
                        //By default, the status is set as COMPLETE (to be backward-compatible with version 1.0 of the schema)
                        .status(SchemaVersion::Status::COMPLETE);

    //The cta-catalogue-schema-verify tool has to be backward-compatible with version 1.0
    //of the schema that does not have the NEXT_SCHEMA_VERSION_MAJOR, NEXT_SCHEMA_VERSION_MINOR and the STATUS column
    const char *const sql2 =
    "SELECT "
        "CTA_CATALOGUE.NEXT_SCHEMA_VERSION_MAJOR AS NEXT_SCHEMA_VERSION_MAJOR,"
        "CTA_CATALOGUE.NEXT_SCHEMA_VERSION_MINOR AS NEXT_SCHEMA_VERSION_MINOR,"
        "CTA_CATALOGUE.STATUS AS STATUS "
      "FROM "
        "CTA_CATALOGUE";
    
    auto stmt2 = m_conn.createStmt(sql2);
    try{
      auto rset2 = stmt2.executeQuery();
      if(rset2.next()){
        auto schemaVersionMajorNext = rset2.columnOptionalUint64("NEXT_SCHEMA_VERSION_MAJOR");
        auto schemaVersionMinorNext = rset2.columnOptionalUint64("NEXT_SCHEMA_VERSION_MINOR");
        auto schemaStatus = rset2.columnString("STATUS");
        if(schemaVersionMajorNext && schemaVersionMinorNext){
          schemaVersionBuilder.nextSchemaVersionMajor(schemaVersionMajorNext.value())
                              .nextSchemaVersionMinor(schemaVersionMinorNext.value())
                              .status(schemaStatus);
        }
      }
    } catch (const cta::exception::Exception &ex){
    }
    return schemaVersionBuilder.build();
  } else {
    throw exception::Exception("CTA_CATALOGUE does not contain any row");
  }
}

std::list<std::string> CatalogueMetadataGetter::getTableNames(){
  std::list<std::string> tableNames = m_conn.getTableNames();
  removeObjectNameContaining(tableNames,{"DATABASECHANGELOG","DATABASECHANGELOGLOCK"});
  return tableNames;
}

std::list<std::string> CatalogueMetadataGetter::getIndexNames(){
  std::list<std::string> indexNames = m_conn.getIndexNames();
  //We just want indexes created by the user, their name are finishing by _IDX or by _I
  cta::utils::Regex regexIndexes("(.*_IDX$)|(.*_I$)");
  removeObjectNameNotMatches(indexNames,regexIndexes);
  return indexNames;
}

std::map<std::string,std::string> CatalogueMetadataGetter::getColumns(const std::string& tableName){
  return m_conn.getColumns(tableName);
}

std::list<std::string> CatalogueMetadataGetter::getConstraintNames(const std::string &tableName){
  std::list<std::string> constraintNames = m_conn.getConstraintNames(tableName);
  //This constraint is added by ALTER TABLE, we can't check its existence for now
  removeObjectNameContaining(constraintNames,{"CATALOGUE_STATUS_CONTENT_CK"});
  return constraintNames;
}

std::list<std::string> CatalogueMetadataGetter::getParallelTableNames(){
  return m_conn.getParallelTableNames();
}

CatalogueMetadataGetter::~CatalogueMetadataGetter() {}

SQLiteCatalogueMetadataGetter::SQLiteCatalogueMetadataGetter(cta::rdbms::Conn & conn):CatalogueMetadataGetter(conn){}
SQLiteCatalogueMetadataGetter::~SQLiteCatalogueMetadataGetter(){}

std::list<std::string> SQLiteCatalogueMetadataGetter::getIndexNames() {
  std::list<std::string> indexNames = CatalogueMetadataGetter::getIndexNames();
  //We do not want the sqlite_autoindex created automatically by SQLite
  removeObjectNameContaining(indexNames,{"sqlite_autoindex"});
  return indexNames;
}

std::list<std::string> SQLiteCatalogueMetadataGetter::getTableNames(){
  std::list<std::string> tableNames = CatalogueMetadataGetter::getTableNames();
  //We do not want the sqlite_sequence tables created automatically by SQLite
  removeObjectNameContaining(tableNames,{"sqlite_sequence"});
  return tableNames;
}

cta::rdbms::Login::DbType SQLiteCatalogueMetadataGetter::getDbType(){
  return cta::rdbms::Login::DbType::DBTYPE_SQLITE;
}

OracleCatalogueMetadataGetter::OracleCatalogueMetadataGetter(cta::rdbms::Conn & conn):CatalogueMetadataGetter(conn){}
OracleCatalogueMetadataGetter::~OracleCatalogueMetadataGetter(){}
cta::rdbms::Login::DbType OracleCatalogueMetadataGetter::getDbType(){
  return cta::rdbms::Login::DbType::DBTYPE_ORACLE;
}

MySQLCatalogueMetadataGetter::MySQLCatalogueMetadataGetter(cta::rdbms::Conn& conn):CatalogueMetadataGetter(conn) {}
MySQLCatalogueMetadataGetter::~MySQLCatalogueMetadataGetter(){}
cta::rdbms::Login::DbType MySQLCatalogueMetadataGetter::getDbType(){
  return cta::rdbms::Login::DbType::DBTYPE_MYSQL;
}

PostgresCatalogueMetadataGetter::PostgresCatalogueMetadataGetter(cta::rdbms::Conn& conn):CatalogueMetadataGetter(conn) {}
PostgresCatalogueMetadataGetter::~PostgresCatalogueMetadataGetter(){}
cta::rdbms::Login::DbType PostgresCatalogueMetadataGetter::getDbType(){
  return cta::rdbms::Login::DbType::DBTYPE_POSTGRESQL;
}

CatalogueMetadataGetter * CatalogueMetadataGetterFactory::create(const rdbms::Login::DbType dbType, cta::rdbms::Conn & conn) {
  typedef rdbms::Login::DbType DbType;
  switch(dbType){
    case DbType::DBTYPE_IN_MEMORY:
    case DbType::DBTYPE_SQLITE:
      return new SQLiteCatalogueMetadataGetter(conn);
    case DbType::DBTYPE_ORACLE:
      return new OracleCatalogueMetadataGetter(conn);
    case DbType::DBTYPE_MYSQL:
      return new MySQLCatalogueMetadataGetter(conn);
    case DbType::DBTYPE_POSTGRESQL:
      return new PostgresCatalogueMetadataGetter(conn);
    default:
      throw cta::exception::Exception("In CatalogueMetadataGetterFactory::create(), can't get CatalogueMetadataGetter for dbType "+rdbms::Login::dbTypeToString(dbType));
  }
}

/**
 * SCHEMA METADATA GETTER methods
 */
SchemaMetadataGetter::SchemaMetadataGetter(std::unique_ptr<SQLiteCatalogueMetadataGetter> sqliteCatalogueMetadataGetter, const cta::rdbms::Login::DbType dbType):m_dbType(dbType) {
  m_sqliteCatalogueMetadataGetter = std::move(sqliteCatalogueMetadataGetter);
}

std::list<std::string> SchemaMetadataGetter::getIndexNames() {
  return m_sqliteCatalogueMetadataGetter->getIndexNames();
}

std::list<std::string> SchemaMetadataGetter::getTableNames() {
  return m_sqliteCatalogueMetadataGetter->getTableNames();
}

std::map<std::string,std::string> SchemaMetadataGetter::getColumns(const std::string& tableName) {
  return m_sqliteCatalogueMetadataGetter->getColumns(tableName);
}

std::list<std::string> SchemaMetadataGetter::getConstraintNames(const std::string& tableName) {
  std::list<std::string> constraintNames = m_sqliteCatalogueMetadataGetter->getConstraintNames(tableName);
  if(m_dbType == cta::rdbms::Login::DbType::DBTYPE_POSTGRESQL){
    //If the database to compare is POSTGRESQL, we cannot compare NOT NULL CONSTRAINT names
    //indeed, POSTGRESQL can not give the NOT NULL constraint names
    removeObjectNameMatches(constraintNames,cta::utils::Regex("(^NN_)|(_NN$)"));
  }
  return constraintNames;
}

}}