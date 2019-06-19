/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
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

#include "catalogue/CatalogueSchema.hpp"
#include "common/utils/Regex.hpp"
#include "common/utils/utils.hpp"
#include "common/exception/Exception.hpp"

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CatalogueSchema::CatalogueSchema(const std::string &sqlSchema): sql(sqlSchema) {
}
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CatalogueSchema::CatalogueSchema(const std::string &sqlSchema,
  const std::string &sqlSchemaTrigger): sql(sqlSchema), sql_trigger(sqlSchemaTrigger) {
}

//------------------------------------------------------------------------------
// getSchemaColumns
//------------------------------------------------------------------------------
std::map<std::string, std::string> CatalogueSchema::getSchemaColumns(const std::string &table) const {
  std::map<std::string, std::string> schemaColumnNames;
  std::string::size_type searchPos = 0;
  std::string::size_type findResult = std::string::npos;
  const std::string columnTypes = 
    "NUMERIC|"
    "INTEGER|"
    "CHAR|"
    "VARCHAR|"
    "VARCHAR2";
  
  try {
    while(std::string::npos != (findResult = sql.find(';', searchPos))) {
      // Calculate the length of the current statement without the trailing ';'
      const std::string::size_type stmtLen = findResult - searchPos;
      const std::string sqlStmt = utils::trimString(sql.substr(searchPos, stmtLen));
      searchPos = findResult + 1;

      if(0 < sqlStmt.size()) { // Ignore empty statements
        const std::string createTableSQL = "CREATE TABLE " + table + "\\(([a-zA-Z0-9_, \\)\\(]+)\\)";
        cta::utils::Regex tableSqlRegex(createTableSQL.c_str());
        auto tableSql = tableSqlRegex.exec(sqlStmt);
        if (2 == tableSql.size()) {
          tableSql[1] += ","; // hack for parsing 
          // we use the same logic here as for trailing ';'
          std::string::size_type searchPosComma = 0;
          std::string::size_type findResultComma = std::string::npos;
          while(std::string::npos != (findResultComma = tableSql[1].find(',', searchPosComma))) {
            const std::string::size_type stmtLenComma = findResultComma - searchPosComma;
            const std::string sqlStmtComma = utils::trimString(tableSql[1].substr(searchPosComma, stmtLenComma));
            searchPosComma = findResultComma + 1;
            if(0 < sqlStmtComma.size()) { // Ignore empty statements
              const std::string columnSQL = "([a-zA-Z_]+) +(" + columnTypes + ")";
              cta::utils::Regex columnSqlRegex(columnSQL.c_str());
              auto columnSql = columnSqlRegex.exec(sqlStmtComma);
              if (3 == columnSql.size()) {
                schemaColumnNames.insert(std::make_pair(columnSql[1], columnSql[2]));
              }
            }
          }
        }
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
  return schemaColumnNames;
}

//------------------------------------------------------------------------------
// getSchemaTableNames
//------------------------------------------------------------------------------
std::list<std::string> CatalogueSchema::getSchemaTableNames() const {  
  std::list<std::string> schemaTables;
  std::string::size_type searchPos = 0;
  std::string::size_type findResult = std::string::npos;
  try {
    while(std::string::npos != (findResult = sql.find(';', searchPos))) {
      // Calculate the length of the current statement without the trailing ';'
      const std::string::size_type stmtLen = findResult - searchPos;
      const std::string sqlStmt = utils::trimString(sql.substr(searchPos, stmtLen));
      searchPos = findResult + 1;

      if(0 < sqlStmt.size()) { // Ignore empty statements
        cta::utils::Regex tableNamesRegex("CREATE TABLE ([a-zA-Z_]+)");
        auto tableName = tableNamesRegex.exec(sqlStmt);
        if (2 == tableName.size()) {
          schemaTables.push_back(tableName[1].c_str());
        }
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
  return schemaTables;
}

//------------------------------------------------------------------------------
// getSchemaIndexNames
//------------------------------------------------------------------------------
std::list<std::string> CatalogueSchema::getSchemaIndexNames() const {  
  std::list<std::string> schemaIndices;
  std::string::size_type searchPos = 0;
  std::string::size_type findResult = std::string::npos;
  try {
    while(std::string::npos != (findResult = sql.find(';', searchPos))) {
      // Calculate the length of the current statement without the trailing ';'
      const std::string::size_type stmtLen = findResult - searchPos;
      const std::string sqlStmt = utils::trimString(sql.substr(searchPos, stmtLen));
      searchPos = findResult + 1;

      if(0 < sqlStmt.size()) { // Ignore empty statements
        cta::utils::Regex tableIndicesRegex("CREATE INDEX ([a-zA-Z_]+)");
        auto indexName = tableIndicesRegex.exec(sqlStmt);
        if (2 == indexName.size()) {
          schemaIndices.push_back(indexName[1].c_str());
        }
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
  return schemaIndices;
}

//------------------------------------------------------------------------------
// getSchemaSequenceNames
//------------------------------------------------------------------------------
std::list<std::string> CatalogueSchema::getSchemaSequenceNames() const {  
  std::list<std::string> schemaSequences;
  std::string::size_type searchPos = 0;
  std::string::size_type findResult = std::string::npos;
  try {
    while(std::string::npos != (findResult = sql.find(';', searchPos))) {
      // Calculate the length of the current statement without the trailing ';'
      const std::string::size_type stmtLen = findResult - searchPos;
      const std::string sqlStmt = utils::trimString(sql.substr(searchPos, stmtLen));
      searchPos = findResult + 1;

      if(0 < sqlStmt.size()) { // Ignore empty statements
        cta::utils::Regex tableSequencesRegex("CREATE SEQUENCE ([a-zA-Z_]+)");
        auto sequenceName = tableSequencesRegex.exec(sqlStmt);
        if (2 == sequenceName.size()) {
          schemaSequences.push_back(sequenceName[1].c_str());
        }
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
  return schemaSequences;
}

//------------------------------------------------------------------------------
// getSchemaTriggerNames
//------------------------------------------------------------------------------
std::list<std::string> CatalogueSchema::getSchemaTriggerNames() const {  
  std::list<std::string> schemaTriggers;
  std::string::size_type searchPos = 0;
  std::string::size_type findResult = std::string::npos;
  try {
    while(std::string::npos != (findResult = sql_trigger.find(';', searchPos))) {
      // Calculate the length of the current statement without the trailing ';'
      const std::string::size_type stmtLen = findResult - searchPos;
      const std::string sqlStmt = utils::trimString(sql_trigger.substr(searchPos, stmtLen));
      searchPos = findResult + 1;

      if(0 < sqlStmt.size()) { // Ignore empty statements
        cta::utils::Regex tableTriggersRegex("CREATE TRIGGER `([a-zA-Z_]+)`");
        auto triggerName = tableTriggersRegex.exec(sqlStmt);
        if (2 == triggerName.size()) {
          schemaTriggers.push_back(triggerName[1].c_str());
        }
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
  return schemaTriggers;
}

//------------------------------------------------------------------------------
// getSchemaVersion
//------------------------------------------------------------------------------
std::map<std::string, uint64_t> CatalogueSchema::getSchemaVersion() const {
  try {
    std::map<std::string, uint64_t> schemaVersion;
    cta::utils::Regex schemaVersionRegex(
      "INSERT INTO CTA_CATALOGUE\\("
      "  SCHEMA_VERSION_MAJOR,"
      "  SCHEMA_VERSION_MINOR\\)"
      "VALUES\\("
      "  ([[:digit:]]+),"
      "  ([[:digit:]]+)\\);"
    );
    auto version = schemaVersionRegex.exec(sql);
    if (3 == version.size()) {
      schemaVersion.insert(std::make_pair("SCHEMA_VERSION_MAJOR", cta::utils::toUint64(version[1].c_str())));
      schemaVersion.insert(std::make_pair("SCHEMA_VERSION_MINOR", cta::utils::toUint64(version[2].c_str())));
    } else {
      exception::Exception ex;
      ex.getMessage() << "Could not find SCHEMA_VERSION";
      throw ex; 
    }
    return schemaVersion;
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace catalogue
} // namespace cta
