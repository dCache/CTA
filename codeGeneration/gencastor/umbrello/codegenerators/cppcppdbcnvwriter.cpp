// Include files
#include <qmap.h>
#include <qfile.h>
#include <qregexp.h>
#include <qptrlist.h>
#include <qtextstream.h>

// local
#include "cppcppdbcnvwriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CppCppDbCnvWriter
//
// 2005-08-10 : Giuseppe Lo Presti
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CppCppDbCnvWriter::CppCppDbCnvWriter(UMLDoc *parent, const char *name) :
  CppCppBaseCnvWriter(parent, name), m_firstClass(true) {
  setPrefix("Db");
}

//=============================================================================
// Standard destructor
//=============================================================================
CppCppDbCnvWriter::~CppCppDbCnvWriter() {
  if (!m_firstClass) {
    endSQLFile();
  }
}

//=============================================================================
// startSQLFile
//=============================================================================
void CppCppDbCnvWriter::startSQLFile() {
  // Preparing SQL files for creation/deletion of the database(s)
  QFile file;
  openFile(file, s_topNS + "/db/oracleSchema.sql",
           IO_WriteOnly | IO_Truncate);
  file.close();
  openFile(file, s_topNS + "/db/oracleGeneratedCore_create.sql",
           IO_WriteOnly | IO_Truncate);
  QTextStream stream(&file);

  stream << "/* Type2Obj metatable definition */" << endl
         << "CREATE TABLE Type2Obj "
         << "(type INTEGER CONSTRAINT PK_Type2Obj_Type PRIMARY KEY,"
         << " object VARCHAR2(100) CONSTRAINT NN_Type2Obj_Object NOT NULL,"
         << " svcHandler VARCHAR2(100),"
         << " CONSTRAINT UN_Type2Obj_typeObject UNIQUE (type, object));" << endl
         << endl
         << "/* ObjStatus metatable definition */" << endl
         << "CREATE TABLE ObjStatus "
         << "(object VARCHAR2(100) CONSTRAINT NN_ObjStatus_object NOT NULL,"
         << " field VARCHAR2(100) CONSTRAINT NN_ObjStatus_field NOT NULL,"
         << " statusCode INTEGER CONSTRAINT NN_ObjStatus_statusCode NOT NULL,"
         << " statusName VARCHAR2(100) CONSTRAINT NN_ObjStatus_statusName NOT NULL,"
         << " CONSTRAINT UN_ObjStatus_objectFieldCode UNIQUE (object, field, statusCode));"
         << endl << endl;

  file.close();
  openFile(file, s_topNS + "/db/oracleGeneratedTrailer_create.sql",
           IO_WriteOnly | IO_Truncate);
  file.close();

  /*  openFile(file, s_topNS + "/db/postgresSchema.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();
      openFile(file, s_topNS + "/db/postgresGeneratedCore_create.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();
      openFile(file, s_topNS + "/db/postgresGeneratedTrailer_create.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();

      openFile(file, s_topNS + "/db/" + s_topNS + "_postgres_drop.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();
      openFile(file, s_topNS + "/db/postgresGeneratedHeader_drop.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();
      openFile(file, s_topNS + "/db/postgresGeneratedCore_drop.sql",
      IO_WriteOnly | IO_Truncate);
      file.close();*/
}

//=============================================================================
// insertFileIntoStream
//=============================================================================
void CppCppDbCnvWriter::insertFileintoStream(QTextStream &stream,
                                             QString fileName) {
  QFile file;
  openFile(file, fileName, IO_ReadOnly);
  QTextStream source(&file);
  QString line = source.readLine();
  while (! line.isNull()) {
    stream << line << endl;
    line = source.readLine();
  }
  file.close();
}

//=============================================================================
// endSQLFile
//=============================================================================
void CppCppDbCnvWriter::endSQLFile() {
  // Finalizing SQL files for creation/deletion of the database(s)
  QFile file;
  openFile(file, s_topNS + "/db/oracleSchema.sql",
           IO_WriteOnly | IO_Append);
  QTextStream streamO(&file);
  insertFileintoStream(streamO, s_topNS + "/db/oracleGeneratedCore_create.sql");
  insertFileintoStream(streamO, s_topNS + "/db/oracleGeneratedTrailer_create.sql");
  file.close();
}

//=============================================================================
// Initialization
//=============================================================================
bool CppCppDbCnvWriter::init(UMLClassifier* c, QString fileName) {
  // call upper class init
  this->CppBaseWriter::init(c, fileName);
  // fixes the namespace
  m_originalPackage = m_classInfo->fullPackageName;
  m_classInfo->packageName = s_topNS + "::db::cnv";
  m_classInfo->fullPackageName = s_topNS + "::db::cnv::";
  // includes converter  header file and object header file
  m_includes.insert(QString("\"Db") + m_classInfo->className + "Cnv.hpp\"");
  m_includes.insert(QString("\"") +
                    computeFileName(m_class,".h") + ".hpp\"");
  // calls the postinit of this class
  postinit(c, fileName);
  return true;
}

//=============================================================================
// writeClass
//=============================================================================
void CppCppDbCnvWriter::writeClass(UMLClassifier */*c*/) {
  if (m_firstClass) {
    startSQLFile();
    m_firstClass = false;
  }
  // static factory declaration
  writeFactory();
  // static constants initialization
  writeConstants();
  // creation/deletion of the databases
  writeOraSqlStatements();
  // constructor and destructor
  writeConstructors();
  // objtype methods
  writeObjType();
  // FillRep methods
  writeFillRep();
  // FillObj methods
  writeFillObj();
  // createRep method
  writeCreateRep();
  // bulkCreateRep method
  writeBulkCreateRep();
  // updateRep method
  writeUpdateRep();
  // deleteRep method
  writeDeleteRep();
  // createObj method
  writeCreateObj();
  // bulkCreateObj method
  writeBulkCreateObj();
  // updateObj method
  writeUpdateObj();
}

//=============================================================================
// ordonnateMembersInAssoc
//=============================================================================
void CppCppDbCnvWriter::ordonnateMembersInAssoc(Assoc* as,
                                                Member** firstMember,
                                                Member** secondMember) {
  // N to N association
  // Here we will use a dedicated table for the association
  // Find out the parent and child in this table
  if (as->type.kind == COMPOS_PARENT ||
      as->type.kind == AGGREG_PARENT) {
    *firstMember = &(as->localPart);
    *secondMember = &(as->remotePart);
  } else if (as->type.kind == COMPOS_CHILD ||
             as->type.kind == AGGREG_CHILD) {
    *firstMember = &(as->remotePart);
    *secondMember = &(as->localPart);
  } else {
    // Case of a symetrical association
    if (as->remotePart.name < as->localPart.name) {
      *firstMember = &(as->remotePart);
      *secondMember = &(as->localPart);
    } else {
      *firstMember = &(as->localPart);
      *secondMember = &(as->remotePart);
    }
  }
}

//=============================================================================
// writeInsertStmtConstants
//=============================================================================
void CppCppDbCnvWriter::writeInsertStmtConstants(MemberList& members,
                                                 AssocList& assocs,
                                                 bool bulk) {
  *m_stream << getIndent()
            << "/// SQL statement for request "
            << (bulk ? "bulk insertion" : "insertion")
            << endl << getIndent()
            << "const std::string "
            << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className
            << (bulk ? "Cnv::s_bulkInsert" : "Cnv::s_insert")
            << "StatementString =" << endl << getIndent()
            << "\"INSERT"
  // The following comment is to overcome a limitation in Oracle whereby we can't use
  // the same statement string and object for both bulk and non-bulk operations with OCCI.
  // See also bug #78533 in savannah.
            << (bulk ? " /* bulk */ " : " ")
            << "INTO " << m_classInfo->className
            << " (";
  bool first = true;
  // Go through the members
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (!first) *m_stream << ", ";
    *m_stream << mem->name;
    first = false;
  }
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      // One to One associations
      if (!first) *m_stream << ", ";
      first = false;
      *m_stream << as->remotePart.name;
    }
  }
  *m_stream << ") VALUES (";
  int n = 1;
  first = true;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (!first) *m_stream << ",";
    first = false;
    if (mem->name == "id") {
      *m_stream << "ids_seq.nextval";
    } else if (mem->name == "nbAccesses") {
      *m_stream << "0";
    } else if (mem->name == "lastAccessTime") {
      *m_stream << "NULL";
    } else {
      *m_stream << ":" << n;
      n++;
    }
  }
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      if (!first) *m_stream << ",";
      first = false;
      *m_stream << ":" << n;
      n++;
    }
  }
  *m_stream << ") RETURNING id INTO :" << n
            << "\";" << endl << endl;
}
            
//=============================================================================
// writeConstants
//=============================================================================
void CppCppDbCnvWriter::writeConstants() {
  writeWideHeaderComment("Static constants initialization",
                         getIndent(),
                         *m_stream);
  // create a list of members
  MemberList members = createMembersList();
  // create a list of associations
  AssocList assocs = createAssocsList();
  // INSERT statements
  writeInsertStmtConstants(members, assocs, false);
  writeInsertStmtConstants(members, assocs, true);
  // DELETE statement
  *m_stream << getIndent() << "/// SQL statement for request deletion"
            << endl << getIndent()
            << "const std::string "
            << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className
            << "Cnv::s_deleteStatementString =" << endl
            << getIndent()
            << "\"DELETE FROM " << m_classInfo->className
            << " WHERE id = :1\";" << endl << endl
            << getIndent()
  // SELECT statements
            << "/// SQL statement for request selection"
            << endl << getIndent()
            << "const std::string "
            << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className
            << "Cnv::s_selectStatementString =" << endl
            << getIndent()
            << "\"SELECT ";
  // Go through the members
  int n = 0;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (n > 0) *m_stream << ", ";
    *m_stream << mem->name;
    n++;
  }
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      if (n > 0) *m_stream << ", ";
      *m_stream << as->remotePart.name;
      n++;
    }
  }
  *m_stream << " FROM " << m_classInfo->className
            << " WHERE id = :1\";" << endl << endl
            << getIndent()
            << "/// SQL statement for bulk request selection"
            << endl << getIndent()
            << "const std::string "
            << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className
            << "Cnv::s_bulkSelectStatementString =" << endl
            << getIndent()
            << "\"DECLARE \\" << endl;
  // We don't use ROWTYPE here and we explicitly provide the list of columns
  // so to support the case of extra pure PL/SQL columns not used by the framework
  *m_stream << getIndent()
            << "   TYPE RecordType IS RECORD (";
  n = 0;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (n > 0) *m_stream << ", ";
    *m_stream << mem->name << " " << getOraSQLType(mem->typeName);
    n++;
  }
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      if (n > 0) *m_stream << ", ";
      *m_stream << as->remotePart.name << " INTEGER";
      n++;
    }
  }
  *m_stream << "); \\" << endl << getIndent()
            << "   TYPE CurType IS REF CURSOR RETURN RecordType; \\"
            << endl;
  *m_stream << getIndent()
            << "   PROCEDURE bulkSelect(ids IN castor.\\\"cnumList\\\", \\"
            << endl << getIndent()
            << "                        objs OUT CurType) AS \\"
            << endl << getIndent()
            << "   BEGIN \\"
            << endl << getIndent()
            << "     FORALL i IN ids.FIRST..ids.LAST \\"
            << endl << getIndent()
            << "       INSERT INTO bulkSelectHelper VALUES(ids(i)); \\"
            << endl << getIndent()
            << "     OPEN objs FOR SELECT ";
  // Go through the members
  n = 0;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (n > 0) *m_stream << ", ";
    *m_stream << mem->name;
    n++;
  }
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      if (n > 0) *m_stream << ", ";
      *m_stream << as->remotePart.name;
      n++;
    }
  }
  *m_stream << " \\"
            << endl << getIndent()
            << "                     FROM " << m_classInfo->className
            << " t, bulkSelectHelper h \\"
            << endl << getIndent()
            << "                    WHERE t.id = h.objId; \\"
            << endl << getIndent()
            << "     DELETE FROM bulkSelectHelper; \\"
            << endl << getIndent()
            << "   END; \\"
            << endl << getIndent()
            << " BEGIN \\"
            << endl << getIndent()
            << "   bulkSelect(:1, :2); \\"
            << endl << getIndent()
            << " END;\";"
            << endl << endl
            << getIndent()
  // UPDATE statement
            << "/// SQL statement for request update"
            << endl << getIndent()
            << "const std::string "
            << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className
            << "Cnv::s_updateStatementString =" << endl
            << getIndent()
            << "\"UPDATE " << m_classInfo->className
            << " SET ";
  n = 0;
  bool first = true;
  // Go through the members
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (mem->name == "id" ||
        mem->name == "creationTime" ||
        mem->name == "lastAccessTime" ||
        mem->name == "nbAccesses") continue;
    if (!first) *m_stream << ", "; else first = false;
    n++;
    *m_stream << mem->name << " = :" << n;
  }
  // Go through dependant objects
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (isEnum(as->remotePart.typeName)) {
      if (n > 0) *m_stream << ", ";
      n++;
      *m_stream << as->remotePart.name << " = :" << n;
    }
  }
  *m_stream << " WHERE id = :" << n+1
            << "\";" << endl << endl;
  // Status dedicated statements
  if (isNewRequest()) {
    *m_stream << getIndent()
              << "/// SQL statement for request insertion into newRequests table"
              << endl << getIndent()
              << "const std::string "
              << m_classInfo->fullPackageName
              << "Db" << m_classInfo->className
              << "Cnv::s_insertNewReqStatementString =" << endl
              << getIndent()
              << "\"INSERT INTO newRequests (id, type, creation)"
              << " VALUES (:1, :2, SYSDATE)\";"
              << endl << endl;
  }
  // Associations dedicated statements
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name == "" ||
        isEnum(as->remotePart.typeName)) continue;
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_N &&
        as->type.multiLocal == MULT_N) {
      // N to N association
      // Here we will use a dedicated table for the association
      // Find out the parent and child in this table
      Member* firstMember = 0;
      Member* secondMember = 0;
      ordonnateMembersInAssoc(as, &firstMember, &secondMember);
      QString firstRole, secondRole;
      if (firstMember == &as->localPart) {
        firstRole = "Parent";
        secondRole = "Child";
      } else {
        firstRole = "Child";
        secondRole = "Parent";
      }
      QString compoundName =
        capitalizeFirstLetter(firstMember->typeName).mid(0, 12) + QString("2")
        + capitalizeFirstLetter(secondMember->typeName).mid(0, 13);
      *m_stream << getIndent()
                << "/// SQL insert statement for member "
                << as->remotePart.name
                << endl << getIndent()
                << "const std::string "
                << m_classInfo->fullPackageName
                << "Db" << m_classInfo->className
                << "Cnv::s_insert"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "StatementString =" << endl << getIndent()
                << "\"INSERT INTO " << compoundName
                << " (" << firstRole << ", " << secondRole
                << ") VALUES (:1, :2)\";"
                << endl << endl << getIndent()
                << "/// SQL delete statement for member "
                << as->remotePart.name
                << endl << getIndent()
                << "const std::string "
                << m_classInfo->fullPackageName
                << "Db" << m_classInfo->className
                << "Cnv::s_delete"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "StatementString =" << endl << getIndent()
                << "\"DELETE FROM " << compoundName
                << " WHERE " << firstRole << " = :1 AND "
                << secondRole << " = :2\";"
                << endl << endl << getIndent()
                << "/// SQL select statement for member "
                << as->remotePart.name
                << endl << getIndent()
                << "// The FOR UPDATE is needed in order to avoid deletion"
                << endl << getIndent()
                << "// of a segment after listing and before update/remove"
                << endl << getIndent()
                << "const std::string "
                << m_classInfo->fullPackageName
                << "Db" << m_classInfo->className
                << "Cnv::s_select"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "StatementString =" << endl << getIndent()
                << "\"SELECT " << secondRole
                << " FROM " << compoundName
                << " WHERE " << firstRole << " = :1 FOR UPDATE\";"
                << endl << endl;
    } else {
      if (as->type.multiLocal == MULT_ONE &&
          as->type.multiRemote != MULT_UNKNOWN &&
          !as->remotePart.abstract &&
          as->localPart.name != "") {
        // 1 to * association
        *m_stream << getIndent()
                  << "/// SQL select statement for member "
                  << as->remotePart.name
                  << endl << getIndent()
                  << "const std::string "
                  << m_classInfo->fullPackageName
                  << "Db" << m_classInfo->className
                  << "Cnv::s_select"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "StatementString =" << endl
                  << getIndent()
                  << "\"SELECT id FROM "
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << " WHERE " << as->localPart.name
                  << " = :1 FOR UPDATE\";" << endl << endl
                  << getIndent()
                  << "/// SQL delete statement for member "
                  << as->remotePart.name
                  << endl << getIndent()
                  << "const std::string "
                  << m_classInfo->fullPackageName
                  << "Db" << m_classInfo->className
                  << "Cnv::s_delete"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "StatementString =" << endl << getIndent()
                  << "\"UPDATE "
                  << as->remotePart.typeName
                  << " SET " << as->localPart.name
                  << " = 0 WHERE id = :1\";" << endl << endl
                  << getIndent()
                  << "/// SQL remote update statement for member "
                  << as->remotePart.name
                  << endl << getIndent()
                  << "const std::string "
                  << m_classInfo->fullPackageName
                  << "Db" << m_classInfo->className
                  << "Cnv::s_remoteUpdate"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "StatementString =" << endl << getIndent()
                  << "\"UPDATE "
                  << as->remotePart.typeName
                  << " SET " << as->localPart.name
                  << " = :1 WHERE id = :2\";" << endl << endl;
      }
      if (as->type.multiRemote == MULT_ONE) {
        // * to 1
        if (!as->remotePart.abstract) {
          *m_stream << getIndent()
                    << "/// SQL existence statement for member "
                    << as->remotePart.name
                    << endl << getIndent()
                    << "const std::string "
                    << m_classInfo->fullPackageName
                    << "Db" << m_classInfo->className
                    << "Cnv::s_check"
                    << capitalizeFirstLetter(as->remotePart.typeName)
                    << "ExistStatementString =" << endl
                    << getIndent()
                    << "\"SELECT id FROM "
                    << capitalizeFirstLetter(as->remotePart.typeName)
                    << " WHERE id = :1\";" << endl << endl;
        }
        *m_stream << getIndent()
                  << "/// SQL update statement for member "
                  << as->remotePart.name
                  << endl << getIndent()
                  << "const std::string "
                  << m_classInfo->fullPackageName
                  << "Db" << m_classInfo->className
                  << "Cnv::s_update"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "StatementString =" << endl
                  << getIndent()
                  << "\"UPDATE "
                  << m_classInfo->className
                  << " SET " << as->remotePart.name
                  << " = :1 WHERE id = :2\";" << endl << endl;
      }
    }
  }
}

//=============================================================================
// writeOraSqlStatements
//=============================================================================
void CppCppDbCnvWriter::writeOraSqlStatements() {
  QFile file, tFile, cFile;
  openFile(file,
           s_topNS + "/db/oracleGeneratedCore_create.sql",
           IO_WriteOnly | IO_Append);
  QTextStream stream(&file);
  openFile(tFile,
           s_topNS + "/db/oracleGeneratedTrailer_create.sql",
           IO_WriteOnly | IO_Append);
  QTextStream tStream(&tFile);

  stream << "/* SQL statements for type "
         << m_classInfo->className
         << " */"
         << endl
         << "CREATE TABLE "
         << m_classInfo->className
         << " (";
  int n = 0;
  // create a list of members
  MemberList members = createMembersList();
  // Go through the members
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (n > 0) stream << ", ";
    stream << mem->name << " "
           << getOraSQLType(mem->typeName);
    if (mem->name == "id") {
      stream << " CONSTRAINT PK_"
             << m_classInfo->className.left(24)
             << "_Id PRIMARY KEY";
    }
    n++;
  }
  // create a list of associations
  AssocList assocs = createAssocsList();
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      // One to One associations
      if (n > 0) stream << ", ";
      stream << as->remotePart.name << " INTEGER";
      n++;
    }
  }
  // These ORACLE parameters allow to have many
  // transactions on the same block and lot of
  // free space (meaning low number of rows) per block
  // The global aim is to reduce the contention on a
  // block because of too many transactions dealing with it
  stream << ") INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;" << endl;
  // Associations dedicated statements
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name == "" ||
        isEnum(as->remotePart.typeName)) continue;
    if (as->type.multiRemote == MULT_N &&
        as->type.multiLocal == MULT_N) {
      // N to N association
      // Here we will use a dedicated table for the association
      // Find out the parent and child in this table
      Member* firstMember = 0;
      Member* secondMember = 0;
      ordonnateMembersInAssoc(as, &firstMember, &secondMember);
      if (firstMember == &as->localPart) {
        QString compoundName =
          capitalizeFirstLetter(firstMember->typeName).mid(0, 12) + QString("2")
          + capitalizeFirstLetter(secondMember->typeName).mid(0, 13);
        stream << "CREATE TABLE "
               << compoundName
               << " (Parent INTEGER, Child INTEGER) INITRANS 50 PCTFREE 50;"
               << endl << getIndent()
               << "CREATE INDEX I_"
               << compoundName
               << "_C on "
               << compoundName
               << " (child);"
               << endl << getIndent()
               << "CREATE INDEX I_"
               << compoundName
               << "_P on "
               << compoundName
               << " (parent);"
               << endl;
        tStream << getIndent()
                << "/* SQL statements for constraints on "
                << m_classInfo->className
                << " */"
                << endl
                << "ALTER TABLE "
                << compoundName
                << endl << getIndent()
                << "  ADD CONSTRAINT FK_"
                << compoundName
                << "_P FOREIGN KEY (Parent) REFERENCES "
                << capitalizeFirstLetter(firstMember->typeName)
                << " (id)" << endl << getIndent()
                << "  ADD CONSTRAINT FK_"
                << compoundName
                << "_C FOREIGN KEY (Child) REFERENCES "
                << capitalizeFirstLetter(secondMember->typeName)
                << " (id);" << endl << endl;
      }
    }
  }
  stream << endl;

  // ObjStatus contents
  for (Assoc* as = assocs.first(); 0 != as; as = assocs.next()) {
    UMLClassifier* c = getClassifier(as->remotePart.typeName);
    if (isEnum(c)) {
      QPtrList<UMLAttribute> atl =
        getClassifier(as->remotePart.typeName)->getAttributeList();
      stream << "BEGIN" << endl;
      for (UMLAttribute *at = atl.first(); at; at = atl.next()) {
        stream << "  serObjStatusName('"
               << as->localPart.typeName << "', '"
               << as->remotePart.name << "', "
               << at->getInitialValue() << ", '"
               << at->getName() << "');"
               << endl;
      }
      stream << "END;" << endl;
             << "/" << endl << endl;
    }
  }

  file.close();
  tFile.close();
}

//=============================================================================
// writeConstructors
//=============================================================================
void CppCppDbCnvWriter::writeConstructors() {
  // constructor
  writeWideHeaderComment("Constructor", getIndent(), *m_stream);
  *m_stream << getIndent() << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className << "Cnv::Db"
            << m_classInfo->className << "Cnv("
            << fixTypeName("ICnvSvc*",
                           "castor",
                           m_classInfo->packageName)
            << " cnvSvc) :" << endl
            << getIndent() << "  DbBaseCnv(cnvSvc)," << endl
            << getIndent() << "  m_insertStatement(0)," << endl
            << getIndent() << "  m_bulkInsertStatement(0)," << endl
            << getIndent() << "  m_deleteStatement(0)," << endl
            << getIndent() << "  m_selectStatement(0)," << endl
            << getIndent() << "  m_bulkSelectStatement(0)," << endl
            << getIndent() << "  m_updateStatement(0)";
  if (isNewRequest()) {
    *m_stream << "," << endl << getIndent() << "  m_insertNewReqStatement(0)";
  }
  // Associations dedicated statements
  AssocList assocs = createAssocsList();
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name == "" ||
        isEnum(as->remotePart.typeName)) continue;
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_N &&
        as->type.multiLocal == MULT_N) {
      // N to N association
      // Here we will use a dedicated table for the association
      // Find out the parent and child in this table
      *m_stream << "," << endl << getIndent()
                << "  m_insert"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement(0)," << endl << getIndent()
                << "  m_delete"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement(0)," << endl << getIndent()
                << "  m_select"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement(0)";
    } else {
      if (as->type.multiLocal == MULT_ONE &&
          as->type.multiRemote != MULT_UNKNOWN &&
          !as->remotePart.abstract &&
          as->localPart.name != "") {
        // 1 to * association
        *m_stream << "," << endl << getIndent()
                  << "  m_select"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement(0)" << "," << endl
                  << getIndent() << "  m_delete"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement(0)," << endl << getIndent()
                  << "  m_remoteUpdate"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement(0)";
      }
      if (as->type.multiRemote == MULT_ONE) {
        // * to 1
        if (!as->remotePart.abstract) {
          *m_stream << "," << endl << getIndent()
                    << "  m_check"
                    << capitalizeFirstLetter(as->remotePart.typeName)
                    << "ExistStatement(0)";
        }
        *m_stream << "," << endl
                  << getIndent() << "  m_update"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement(0)";
      }
    }
  }
  *m_stream << " {}" << endl << endl;
  // Destructor
  writeWideHeaderComment("Destructor", getIndent(), *m_stream);
  *m_stream << getIndent() << m_classInfo->fullPackageName
            << "Db" << m_classInfo->className << "Cnv::~Db"
            << m_classInfo->className << "Cnv() throw() {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "//Here we attempt to delete the statements correctly"
            << endl << getIndent()
            << "// If something goes wrong, we just ignore it"
            << endl << getIndent() << "try {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "if(m_insertStatement) delete m_insertStatement;"
            << endl << getIndent()
            << "if(m_bulkInsertStatement) delete m_bulkInsertStatement;"
            << endl << getIndent()
            << "if(m_deleteStatement) delete m_deleteStatement;"
            << endl << getIndent()
            << "if(m_selectStatement) delete m_selectStatement;"
            << endl << getIndent()
            << "if(m_bulkSelectStatement) delete m_bulkSelectStatement;"
            << endl << getIndent()
            << "if(m_updateStatement) delete m_updateStatement;"
            << endl;
  if (isNewRequest()) {
    *m_stream << getIndent()
              << "if(m_insertNewReqStatement) delete m_insertNewReqStatement;"
              << endl;
  }
  // Associations dedicated statements
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name == "" ||
        isEnum(as->remotePart.typeName)) continue;
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_N &&
        as->type.multiLocal == MULT_N) {
      // N to N association
      // Here we will use a dedicated table for the association
      // Find out the parent and child in this table
      *m_stream << getIndent()
                << "if(m_insert"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement) delete m_insert"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement;" << endl << getIndent()
                << "if(m_delete"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement) delete m_delete"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement;" << endl << getIndent()
                << "if(m_select"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement) delete m_select"
                << capitalizeFirstLetter(as->remotePart.typeName)
                << "Statement;" << endl;
    } else {
      if (as->type.multiLocal == MULT_ONE &&
          as->type.multiRemote != MULT_UNKNOWN &&
          !as->remotePart.abstract &&
          as->localPart.name != "") {
        // 1 to * association
        *m_stream << getIndent()
                  << "if(m_delete"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement) delete m_delete"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement;" << endl << getIndent()
                  << "if(m_select"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement) delete m_select"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement;" << endl << getIndent()
                  << "if(m_remoteUpdate"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement) delete m_remoteUpdate"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement;" << endl;
      }
      if (as->type.multiRemote == MULT_ONE) {
        // * to 1
        if (!as->remotePart.abstract) {
          *m_stream << getIndent()
                    << "if(m_check"
                    << capitalizeFirstLetter(as->remotePart.typeName)
                    << "ExistStatement) delete m_check"
                    << capitalizeFirstLetter(as->remotePart.typeName)
                    << "ExistStatement;" << endl;
        }
        *m_stream << getIndent()
                  << "if(m_update"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement) delete m_update"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "Statement;" << endl;
      }
    }
  }
  m_indent--;
  *m_stream << getIndent()
            << "} catch (castor::exception::Exception& ignored) {};"
            << endl;
  // End of the method
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeFillRep
//=============================================================================
void CppCppDbCnvWriter::writeFillRep() {
  // First write the main function, dispatching the requests
  writeWideHeaderComment("fillRep", getIndent(), *m_stream);
  QString func = QString("void ") +
    m_classInfo->packageName + "::Db" +
    m_classInfo->className + "Cnv::fillRep(";
  *m_stream << getIndent() << func
            << fixTypeName("IAddress*",
                           "castor",
                           "")
            << "," << endl << getIndent();
  func.replace(QRegExp("."), " ");
  *m_stream << func  << fixTypeName("IObject*",
                                    "castor",
                                    "")
            << " object," << endl << getIndent()
            << func << "unsigned int type,"
            << endl << getIndent()
            << func << "bool endTransaction)"
            << endl << getIndent() << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           "")
            << ") {"
            << endl;
  m_indent++;
  // Get the precise object
  *m_stream << getIndent() << m_originalPackage
            << m_classInfo->className << "* obj = " << endl
            << getIndent() << "  dynamic_cast<"
            << m_originalPackage
            << m_classInfo->className << "*>(object);"
            << endl;
  // Call the dedicated method
  *m_stream << getIndent() << "try {" << endl;
  m_indent++;
  *m_stream << getIndent() << "switch (type) {" << endl;
  AssocList assocs = createAssocsList();
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name != "" &&
        !isEnum(as->remotePart.typeName)) {
      if (as->remoteStereotype == SQLONLY) continue;
      if (as->type.multiRemote == MULT_ONE ||
          as->type.multiRemote == MULT_N) {
        addInclude(QString("\"") + s_topNS + "/Constants.hpp\"");
        *m_stream << getIndent() << "case " + s_topNS + "::OBJ_"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << " :" << endl;
        m_indent++;
        *m_stream << getIndent() << "fillRep"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "(obj);" << endl << getIndent()
                  << "break;" << endl;
        m_indent--;
      }
    }
  }
  *m_stream << getIndent() << "default :" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "castor::exception::InvalidArgument ex;"
            << endl << getIndent()
            << "ex.getMessage() << \"fillRep called for type \" << type "
            << endl << getIndent()
            << "                << \" on object of type \" << obj->type() "
            << endl << getIndent()
            << "                << \". This is meaningless.\";"
            << endl << getIndent()
            << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << getIndent()
            << "if (endTransaction) {" << endl;
  m_indent++;
  *m_stream << getIndent() << "cnvSvc()->commit();"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent()
            << "} catch (castor::exception::SQLError& e) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << fixTypeName("Internal",
                           "castor.exception",
                           m_classInfo->packageName)
            << " ex;"
            << endl << getIndent()
            << "ex.getMessage() << \"Error in fillRep for type \" << type"
            << endl << getIndent()
            << "                << std::endl << e.getMessage().str() << std::endl;"
            << endl << getIndent() << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;

  // Now write the dedicated fillRep Methods
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name != "" &&
        !isEnum(as->remotePart.typeName)) {
      if (as->remoteStereotype == SQLONLY) continue;
      if (as->type.multiRemote == MULT_ONE) {
        writeBasicMult1FillRep(as);
      } else if  (as->type.multiRemote == MULT_N) {
        writeBasicMultNFillRep(as);
      }
    }
  }
}

//=============================================================================
// writeFillObj
//=============================================================================
void CppCppDbCnvWriter::writeFillObj() {
  // First write the main function, dispatching the requests
  writeWideHeaderComment("fillObj", getIndent(), *m_stream);
  QString func = QString("void ") +
    m_classInfo->packageName + "::Db" +
    m_classInfo->className + "Cnv::fillObj(";
  *m_stream << getIndent() << func
            << fixTypeName("IAddress*",
                           "castor",
                           "")
            << "," << endl << getIndent();
  func.replace(QRegExp("."), " ");
  *m_stream << func << fixTypeName("IObject*",
                                   "castor",
                                   "")
            << " object," << endl << getIndent()
            << func << "unsigned int type,"
            << endl << getIndent()
            << func << "bool endTransaction)"
            << endl << getIndent() << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           "")
            << ") {"
            << endl;
  m_indent++;
  // Get the precise object
  *m_stream << getIndent() << m_originalPackage
            << m_classInfo->className << "* obj = " << endl
            << getIndent() << "  dynamic_cast<"
            << m_originalPackage
            << m_classInfo->className << "*>(object);"
            << endl;
  // Call the dedicated method
  *m_stream << getIndent() << "switch (type) {" << endl;
  AssocList assocs = createAssocsList();
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remotePart.name != "" &&
        !isEnum(as->remotePart.typeName)) {
      if (as->remoteStereotype == SQLONLY) continue;
      if (as->type.multiRemote == MULT_ONE ||
          as->type.multiRemote == MULT_N) {
        addInclude(QString("\"") + s_topNS + "/Constants.hpp\"");
        *m_stream << getIndent() << "case " + s_topNS + "::OBJ_"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << " :" << endl;
        m_indent++;
        *m_stream << getIndent() << "fillObj"
                  << capitalizeFirstLetter(as->remotePart.typeName)
                  << "(obj);" << endl << getIndent()
                  << "break;" << endl;
        m_indent--;
      }
    }
  }
  *m_stream << getIndent() << "default :" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "castor::exception::InvalidArgument ex;"
            << endl << getIndent()
            << "ex.getMessage() << \"fillObj called on type \" << type "
            << endl << getIndent()
            << "                << \" on object of type \" << obj->type() "
            << endl << getIndent()
            << "                << \". This is meaningless.\";"
            << endl << getIndent()
            << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  *m_stream << getIndent() << "if (endTransaction) {" << endl;
  m_indent++;
  *m_stream << getIndent() << "cnvSvc()->commit();" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;

  // Now write the dedicated fillObj Methods
  unsigned int n = 0;
  MemberList members = createMembersList();
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    n++;
  }
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->remotePart.name != "" &&
        !isEnum(as->remotePart.typeName)) {
      if (as->type.multiRemote == MULT_ONE) {
        n++;
        writeBasicMult1FillObj(as, n);
      } else if  (as->type.multiRemote == MULT_N) {
        writeBasicMultNFillObj(as);
      }
    } else if (isEnum(as->remotePart.typeName)) {
      n++;
    }
  }
}

//=============================================================================
// writeBasicMult1FillRep
//=============================================================================
void CppCppDbCnvWriter::writeBasicMult1FillRep(Assoc* as) {
  writeWideHeaderComment("fillRep" +
                         capitalizeFirstLetter(as->remotePart.typeName),
                         getIndent(), *m_stream);
  *m_stream << getIndent() << "void "
            << m_classInfo->packageName << "::Db"
            << m_classInfo->className << "Cnv::fillRep"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "("
            << fixTypeName(m_classInfo->className + "*",
                           getNamespace(m_classInfo->className),
                           "")
            << " obj)" << endl << getIndent()
            << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           "")
            << ") {"
            << endl;
  m_indent++;
  if (as->type.multiLocal == MULT_ONE &&
      !as->remotePart.abstract &&
      as->localPart.name != "") {
    // 1 to 1, wee need to check whether the old remote object
    // should be updated
    *m_stream << getIndent() << "// Check select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << " statement" << endl << getIndent()
              << "if (0 == m_select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent() << "m_select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement = createStatement(s_select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "// retrieve the object from the database"
              << endl << getIndent() << "m_select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(1, obj->id());"
              << endl << getIndent()
              << "castor::db::IDbResultSet *rset = m_select"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->executeQuery();"
              << endl;
    *m_stream << getIndent()
              << "if (rset->next()) {"
              << endl;
    m_indent++;
    writeSingleGetFromSelect(as->remotePart, 1, true);
    *m_stream << getIndent()
              << "if (0 != " << as->remotePart.name
              << "Id &&" << endl
              << getIndent() << "    (0 == obj->"
              << as->remotePart.name << "() ||" << endl
              << getIndent() << "     obj->"
              << as->remotePart.name << "()->id() != "
              << as->remotePart.name << "Id)) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "if (0 == m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement = createStatement(s_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(1, "
              << as->remotePart.name
              << "Id);"
              << endl << getIndent() << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->execute();"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl;
    // Close request
    *m_stream << getIndent() << "// Close resultset" << endl
              << getIndent() << "delete rset;"
              << endl;
  }
  if (!as->remotePart.abstract) {
    // Common part to all * to 1 associations, except if remote
    // end is abstract :
    // See whether the remote end exists and create it if not
    *m_stream << getIndent() << "if (0 != obj->"
              << as->remotePart.name << "()) {" << endl;
    m_indent++;
    *m_stream << getIndent() << "// Check check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Exist statement" << endl << getIndent()
              << "if (0 == m_check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "ExistStatement) {" << endl;
    m_indent++;
    *m_stream << getIndent() << "m_check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "ExistStatement = createStatement(s_check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "ExistStatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "// retrieve the object from the database"
              << endl << getIndent() << "m_check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "ExistStatement->setUInt64(1, obj->"
              << as->remotePart.name << "()->id());"
              << endl << getIndent()
              << "castor::db::IDbResultSet *rset = m_check"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "ExistStatement->executeQuery();"
              << endl;
    *m_stream << getIndent()
              << "if (!rset->next()) {"
              << endl;
    m_indent++;
    addInclude("\"castor/Constants.hpp\"");
    *m_stream << getIndent()
              << fixTypeName("BaseAddress",
                             "castor",
                             "")
              << " ad;" << endl << getIndent()
              << "ad.setCnvSvcName(\"DbCnvSvc\");"
              << endl << getIndent()
              << "ad.setCnvSvcType(castor::SVC_DBCNV);"
              << endl << getIndent()
              << "cnvSvc()->createRep(&ad, obj->"
              << as->remotePart.name
              << "(), false";
    if (as->type.multiLocal == MULT_ONE) {
      *m_stream << ", OBJ_" << as->localPart.typeName;
    }
    *m_stream << ");" << endl;
    m_indent--;
    if (as->type.multiLocal == MULT_ONE &&
        as->localPart.name != "") {
      *m_stream << getIndent() << "} else {" << endl;
      m_indent++;
      *m_stream << getIndent() << "// Check remote update statement"
                << endl << getIndent()
                << "if (0 == m_remoteUpdate" << as->remotePart.typeName
                << "Statement) {" << endl;
      m_indent++;
      *m_stream << getIndent()
                << "m_remoteUpdate" << as->remotePart.typeName
                << "Statement = createStatement(s_remoteUpdate"
                << as->remotePart.typeName
                << "StatementString);"
                << endl;
      m_indent--;
      *m_stream << getIndent() << "}" << endl << getIndent()
                << "// Update remote object"
                << endl << getIndent()
                << "m_remoteUpdate" << as->remotePart.typeName
                << "Statement->setUInt64(1, obj->id());"
                << endl << getIndent()
                << "m_remoteUpdate" << as->remotePart.typeName
                << "Statement->setUInt64(2, obj->"
                << as->remotePart.name
                << "()->id());"
                << endl << getIndent()
                << "m_remoteUpdate" << as->remotePart.typeName
                << "Statement->execute();"
                << endl;
      m_indent--;
    }
    *m_stream << getIndent() << "}" << endl
              << getIndent() << "// Close resultset" << endl
              << getIndent() << "delete rset;"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl;
  }
  // Last bit, common to all * to 1 associations :
  // update the local object
  *m_stream << getIndent() << "// Check update statement"
            << endl << getIndent()
            << "if (0 == m_update" << as->remotePart.typeName
            << "Statement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_update" << as->remotePart.typeName
            << "Statement = createStatement(s_update"
            << as->remotePart.typeName
            << "StatementString);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << getIndent()
            << "// Update local object"
            << endl << getIndent()
            << "m_update" << as->remotePart.typeName
            << "Statement->setUInt64(1, 0 == obj->"
            << as->remotePart.name
            << "() ? 0 : obj->"
            << as->remotePart.name
            << "()->id());"
            << endl << getIndent()
            << "m_update" << as->remotePart.typeName
            << "Statement->setUInt64(2, obj->id());"
            << endl << getIndent()
            << "m_update" << as->remotePart.typeName
            << "Statement->execute();"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeBasicMult1FillObj
//=============================================================================
void CppCppDbCnvWriter::writeBasicMult1FillObj(Assoc* as,
                                               unsigned int n) {
  writeWideHeaderComment("fillObj" +
                         capitalizeFirstLetter(as->remotePart.typeName),
                         getIndent(), *m_stream);
  *m_stream << getIndent() << "void "
            << m_classInfo->packageName << "::Db"
            << m_classInfo->className << "Cnv::fillObj"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "("
            << fixTypeName(m_classInfo->className + "*",
                           getNamespace(m_classInfo->className),
                           "")
            << " obj)"
            << endl << getIndent() << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           m_classInfo->packageName)
            << ") {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << "// Check whether the statement is ok"
            << endl << getIndent()
            << "if (0 == m_selectStatement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_selectStatement = createStatement(s_selectStatementString);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent()
            << "// retrieve the object from the database"
            << endl << getIndent()
            << "m_selectStatement->setUInt64(1, obj->id());"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();"
            << endl << getIndent()
            << "if (!rset->next()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << fixTypeName("NoEntry",
                           "castor.exception",
                           m_classInfo->packageName)
            << " ex;" << endl << getIndent()
            << "ex.getMessage() << \"No object found for id :\""
            << " << obj->id();" << endl
            << getIndent() << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  writeSingleGetFromSelect(as->remotePart, n, true);
  *m_stream << getIndent() << "// Close ResultSet"
            << endl << getIndent() << "delete rset;"
            << endl;
  *m_stream << getIndent()
            << "// Check whether something should be deleted"
            << endl << getIndent() <<"if (0 != obj->"
            << as->remotePart.name << "() &&" << endl
            << getIndent()
            << "    (0 == " << as->remotePart.name
            << "Id ||" << endl
            << getIndent() << "     obj->"
            << as->remotePart.name
            << "()->id() != " << as->remotePart.name
            << "Id)) {" << endl;
  m_indent++;
  if (as->localPart.name != "") {
    *m_stream << getIndent()
              << "obj->"
              << as->remotePart.name << "()->";
    if (as->type.multiLocal == MULT_ONE) {
      *m_stream << "set"
                << capitalizeFirstLetter(as->localPart.name)
                << "(0)";
    } else {
      *m_stream << "remove"
                << capitalizeFirstLetter(as->localPart.name)
                << "(obj)";
    }
    *m_stream << ";" << endl;
  }
  *m_stream << getIndent()
            << "obj->set"
            << capitalizeFirstLetter(as->remotePart.name)
            << "(0);" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  *m_stream << getIndent()
            << "// Update object or create new one"
            << endl << getIndent()
            << "if (0 != " << as->remotePart.name
            << "Id) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "if (0 == obj->"
            << as->remotePart.name << "()) {" << endl;
  m_indent++;
  *m_stream << getIndent() << "obj->set"
            << capitalizeFirstLetter(as->remotePart.name)
            << endl << getIndent()
            << "  (dynamic_cast<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*>" << endl << getIndent()
            << "   (cnvSvc()->getObjFromId(" << as->remotePart.name
            << "Id, OBJ_" << as->remotePart.typeName << ")));"
            << endl;
  m_indent--;
  *m_stream << getIndent()
            << "} else {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "cnvSvc()->updateObj(obj->"
            << as->remotePart.name
            << "());"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  if (as->localPart.name != "") {
    // Update back link
    *m_stream << getIndent() << "obj->"
              << as->remotePart.name << "()->";
    if (as->type.multiLocal == MULT_ONE) {
      *m_stream << "set";
    } else {
      *m_stream << "add";
    }
    *m_stream << capitalizeFirstLetter(as->localPart.name)
              << "(obj);" << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeBasicMultNFillRep
//=============================================================================
void CppCppDbCnvWriter::writeBasicMultNFillRep(Assoc* as) {
  writeWideHeaderComment("fillRep" +
                         capitalizeFirstLetter(as->remotePart.typeName),
                         getIndent(), *m_stream);
  *m_stream << getIndent()
            << "void " << m_classInfo->packageName
            << "::Db" << m_classInfo->className
            << "Cnv::fillRep"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "("
            << fixTypeName(m_classInfo->className + "*",
                           getNamespace(m_classInfo->className),
                           "")
            << " obj)"
            << endl << getIndent() << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           "")
            << ") {"
            << endl;
  m_indent++;
  *m_stream << getIndent() << "// check select statement"
            << endl << getIndent()
            << "if (0 == m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement = createStatement(s_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "StatementString);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent() << "// Get current database data"
            << endl << getIndent()
            << fixTypeName("set", "", "")
            << "<u_signed64> " << as->remotePart.name
            << "List;" << endl << getIndent()
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement->setUInt64(1, obj->id());"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset = "
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement->executeQuery();"
            << endl << getIndent()
            << "while (rset->next()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << as->remotePart.name
            << "List.insert(rset->getUInt64(1));"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent() << "delete rset;"
            << endl << getIndent()
            << "// update " << as->remotePart.name
            << " and create new ones"
            << endl;
  if (as->type.multiLocal == MULT_ONE
      && as->remotePart.typeName != "longstring") {
    *m_stream << getIndent() << fixTypeName("vector", "", "")
              << "<castor::IObject*> toBeCreated;"
              << endl;
  }
  *m_stream << getIndent()
            << "for (" << fixTypeName("vector", "", "") << "<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*>::iterator it = obj->"
            << as->remotePart.name
            << "().begin();" << endl << getIndent()
            << "     it != obj->"
            << as->remotePart.name
            << "().end();" << endl << getIndent()
            << "     it++) {"  << endl;
  m_indent++;
  *m_stream << getIndent() << "if (0 == (*it)->id()) {"
            << endl;
  m_indent++;
  if (as->type.multiLocal == MULT_ONE
      && as->remotePart.typeName != "longstring") {
    *m_stream << getIndent()
              << "toBeCreated.push_back(*it);";
  } else {
    *m_stream << getIndent()
              << "cnvSvc()->createRep(0, *it, false);";
  }
  *m_stream << endl;
  if (as->type.multiLocal == MULT_ONE &&
      !as->remotePart.abstract) {
    m_indent--;
    *m_stream << getIndent() << "} else {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "// Check remote update statement"
              << endl << getIndent()
              << "if (0 == m_remoteUpdate" << as->remotePart.typeName
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "m_remoteUpdate" << as->remotePart.typeName
              << "Statement = createStatement(s_remoteUpdate"
              << as->remotePart.typeName
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "// Update remote object"
              << endl << getIndent()
              << "m_remoteUpdate" << as->remotePart.typeName
              << "Statement->setUInt64(1, obj->id());"
              << endl << getIndent()
              << "m_remoteUpdate" << as->remotePart.typeName
              << "Statement->setUInt64(2, (*it)->id());"
              << endl << getIndent()
              << "m_remoteUpdate" << as->remotePart.typeName
              << "Statement->execute();"
              << endl;
  }
  if (as->type.multiLocal == MULT_N) {
    m_indent--;
    *m_stream << getIndent() << "}" << endl;
  }
  *m_stream << getIndent()
            << fixTypeName("set", "", "")
            << "<u_signed64>::iterator item;" << endl
            << getIndent() << "if ((item = "
            << as->remotePart.name
            << "List.find((*it)->id())) != "
            << as->remotePart.name
            << "List.end()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent() << as->remotePart.name
            << "List.erase(item);"
            << endl;
  if (as->type.multiLocal == MULT_N) {
    // N to N association
    // Here we will use a dedicated table for the association
    // Find out the parent and child in this table
    m_indent--;
    *m_stream << getIndent() << "} else {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "if (0 == m_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "m_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement = createStatement(s_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl
              << getIndent()
              << "m_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(1, obj->id());"
              << endl << getIndent()
              << "m_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(2, (*it)->id());"
              << endl << getIndent()
              << "m_insert"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->execute();"
              << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  if (as->type.multiLocal == MULT_ONE &&
      !as->remotePart.abstract) {
    m_indent--;
    *m_stream << getIndent() << "}" << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  if (as->type.multiLocal == MULT_ONE
      && as->remotePart.typeName != "longstring") {
    *m_stream << getIndent() << "// create new objects" << endl
              << getIndent() << "cnvSvc()->bulkCreateRep(0, toBeCreated, false"
              << ", OBJ_" << as->localPart.typeName << ");" << endl;
  }
  *m_stream << getIndent() << "// Delete old links"
            << endl << getIndent() << "for ("
            << fixTypeName("set", "", "")
            << "<u_signed64>::iterator it = "
            << as->remotePart.name << "List.begin();"
            << endl << getIndent()
            << "     it != "
            << as->remotePart.name << "List.end();"
            << endl << getIndent()
            << "     it++) {"  << endl;
  m_indent++;
  if (as->type.multiLocal == MULT_N) {
    // N to N association
    // Here we will use a dedicated table for the association
    // Find out the parent and child in this table
    *m_stream << getIndent()
              << "if (0 == m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement = createStatement(s_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(1, obj->id());"
              << endl << getIndent() << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(2, *it);"
              << endl << getIndent() << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->execute();"
              << endl;
  } else {
    // 1 to N association
    // We need to update the remote table that contains the link
    *m_stream << getIndent()
              << "if (0 == m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement) {" << endl;
    m_indent++;
    *m_stream << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement = createStatement(s_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "StatementString);"
              << endl;
    m_indent--;
    *m_stream << getIndent() << "}" << endl << getIndent()
              << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->setUInt64(1, *it);"
              << endl << getIndent() << "m_delete"
              << capitalizeFirstLetter(as->remotePart.typeName)
              << "Statement->execute();"
              << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeBasicMultNFillObj
//=============================================================================
void CppCppDbCnvWriter::writeBasicMultNFillObj(Assoc* as) {
  writeWideHeaderComment("fillObj" +
                         capitalizeFirstLetter(as->remotePart.typeName),
                         getIndent(), *m_stream);
  *m_stream << getIndent()
            << "void " << m_classInfo->packageName
            << "::Db" << m_classInfo->className
            << "Cnv::fillObj"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "("
            << fixTypeName(m_classInfo->className + "*",
                           getNamespace(m_classInfo->className),
                           "")
            << " obj)"
            << endl << getIndent() << "  throw ("
            << fixTypeName("Exception",
                           "castor.exception",
                           "")
            << ") {"
            << endl;
  m_indent++;

  *m_stream << getIndent() << "// Check select statement"
            << endl << getIndent()
            << "if (0 == m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement = createStatement(s_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "StatementString);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent()
            << "// retrieve the object from the database"
            << endl << getIndent()
            << fixTypeName("vector", "", "")
            << "<u_signed64> " << as->remotePart.name
            << "List;" << endl << getIndent()
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement->setUInt64(1, obj->id());"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset = "
            << "m_select"
            << capitalizeFirstLetter(as->remotePart.typeName)
            << "Statement->executeQuery();"
            << endl << getIndent()
            << "while (rset->next()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << as->remotePart.name
            << "List.push_back(rset->getUInt64(1));"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent() << "// Close ResultSet"
            << endl << getIndent() << "delete rset;"
            << endl << getIndent()
            << "// Update objects and mark old ones for deletion"
            << endl << getIndent()
            << fixTypeName("vector", "", "") << "<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*> toBeDeleted;"
            << endl << getIndent()
            << "for ("
            << fixTypeName("vector", "", "")
            << "<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*>::iterator it = obj->"
            << as->remotePart.name
            << "().begin();" << endl << getIndent()
            << "     it != obj->"
            << as->remotePart.name
            << "().end();" << endl << getIndent()
            << "     it++) {"  << endl;
  m_indent++;
  addInclude("<algorithm>");
  *m_stream << getIndent()
            << fixTypeName("vector", "", "")
            << "<u_signed64>::iterator item =" << endl
            << getIndent() << "  std::find("
            << as->remotePart.name
            << "List.begin(), "
            << as->remotePart.name
            << "List.end(), (*it)->id());" << endl
            << getIndent() << "if (item == "
            << as->remotePart.name
            << "List.end()) {" << endl;
  m_indent++;
  *m_stream << getIndent() << "toBeDeleted.push_back(*it);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "} else {" << endl;
  m_indent++;
  *m_stream << getIndent() << as->remotePart.name
            << "List.erase(item);"
            << endl << getIndent()
            << "cnvSvc()->updateObj((*it));"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  *m_stream << getIndent()
            << "// Delete old objects"
            << endl << getIndent()
            << "for ("
            << fixTypeName("vector", "", "")
            << "<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*>::iterator it = toBeDeleted.begin();"
            << endl << getIndent()
            << "     it != toBeDeleted.end();"
            << endl << getIndent()
            << "     it++) {"  << endl;
  m_indent++;
  *m_stream << getIndent() << "obj->remove"
            << capitalizeFirstLetter(as->remotePart.name)
            << "(*it);" << endl;
  if (as->localPart.name != "") {
    *m_stream << getIndent()
              << "(*it)->";
    if (as->type.multiLocal == MULT_ONE) {
      *m_stream << "set"
                << capitalizeFirstLetter(as->localPart.name)
                << "(0)";
    } else {
      *m_stream << "remove"
                << capitalizeFirstLetter(as->localPart.name)
                << "(obj)";
    }
    *m_stream << ";" << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent()
            << "// Create new objects"
            << endl << getIndent()
            << fixTypeName("vector", "", "")
            << "<castor::IObject*> new"
            << capitalizeFirstLetter(as->remotePart.name)
            << " =" << endl << getIndent()
            << "  cnvSvc()->getObjsFromIds("
            << as->remotePart.name << "List, OBJ_"
            << as->remotePart.typeName << ");"
            << endl << getIndent() << "for ("
            << fixTypeName("vector", "", "")
            << "<castor::IObject*>::iterator it = new"
            << capitalizeFirstLetter(as->remotePart.name)
            << ".begin();"
            << endl << getIndent()
            << "     it != new"
            << capitalizeFirstLetter(as->remotePart.name)
            << ".end();"
            << endl << getIndent()
            << "     it++) {"  << endl;
  m_indent++;
  *m_stream << getIndent()
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "* remoteObj = " << endl << getIndent()
            << "  dynamic_cast<"
            << fixTypeName(as->remotePart.typeName,
                           getNamespace(as->remotePart.typeName),
                           m_classInfo->packageName)
            << "*>(*it);" << endl << getIndent()
            << "obj->add"
            << capitalizeFirstLetter(as->remotePart.name)
            << "(remoteObj);" << endl;
  if (as->localPart.name != "") {
    // Update back link
    *m_stream << getIndent() << "remoteObj->";
    if (as->type.multiLocal == MULT_ONE) {
      *m_stream << "set";
    } else {
      *m_stream << "add";
    }
    *m_stream << capitalizeFirstLetter(as->localPart.name)
              << "(obj);" << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;

  m_indent--;
  *m_stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeCreateRepCheckStatements
//=============================================================================
void CppCppDbCnvWriter::writeCreateRepCheckStatements(QTextStream &stream,
                                                      MemberList &members,
                                                      AssocList &assocs,
                                                      bool bulk) {
  // First check the statements
  stream << getIndent()
         << "// Check whether the statements are ok"
         << endl << getIndent()
         << "if (0 == " << (bulk ? "m_bulkInsert" : "m_insert")
         << "Statement) {" << endl;
  m_indent++;
  // Go through the members and assoc to find the number for id
  int n = 1;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (mem->name != "id" &&
        mem->name != "nbAccesses" &&
        mem->name != "lastAccessTime") {
      n++;
    }
  }
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      n++;
    }
  }
  stream << getIndent()
         << (bulk ? "m_bulkInsert" : "m_insert")
         << "Statement = createStatement(s_"
         << (bulk ? "bulkInsert" : "insert")
         << "StatementString);"
         << endl << getIndent()
         << (bulk ? "m_bulkInsert" : "m_insert")
         << "Statement->registerOutParam(" << n
         << ", castor::db::DBTYPE_UINT64);" << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  if (isNewRequest()) {
    stream << getIndent()
           << "if (0 == m_insertNewReqStatement) {" << endl;
    m_indent++;
    stream << getIndent()
           << "m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);"
           << endl;
    m_indent--;
    stream << getIndent() << "}" << endl;
  }
}

//=============================================================================
// writeCreateRepContent
//=============================================================================
void CppCppDbCnvWriter::writeCreateRepContent(QTextStream &stream, bool &addressUsed,
                                              bool &endTransUsed, bool &typeUsed) {
  // check whether something needs to be done
  stream << getIndent()
         << "// check whether something needs to be done"
         << endl << getIndent()
         << "if (0 == obj) return;" << endl
         << getIndent()
         << "if (0 != obj->id()) return;" << endl;
  // Start of try block for database statements
  stream << getIndent() << "try {" << endl;
  m_indent++;
  // Check the statements
  MemberList members = createMembersList();
  AssocList assocs = createAssocsList();
  writeCreateRepCheckStatements(stream, members, assocs, false);
  // Insert the object into the database
  stream << getIndent()
         << "// Now Save the current object"
         << endl;
  // create a list of members to be saved
  int n = 1;
  // Go through the members
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (mem->name != "id" &&
        mem->name != "nbCopyAccesses" &&
        mem->name != "creationTime" &&
        mem->name != "lastModificationTime" &&
        mem->name != "lastAccessTime") {
      writeSingleSetIntoStatement(stream, "insert", *mem, n);
      n++;
    } else if (mem->name == "creationTime" ||
               mem->name == "lastModificationTime") {
      stream << getIndent()
             << "m_insertStatement->setInt("
             << n << ", time(0));" << endl;
      n++;
    }
  }
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (isEnum(as->remotePart.typeName)) {
      writeSingleSetIntoStatement(stream, "insert", as->remotePart, n, true);
      n++;
    } else if (as->type.multiRemote == MULT_ONE &&
               as->remotePart.name != "") {
      stream << getIndent()
             << "m_insertStatement->setUInt64("
             << n << ", ";
      if (!isEnum(as->remotePart.typeName)) {
        typeUsed = true;
        stream << "(type == OBJ_"
               << as->remotePart.typeName
               << " && obj->" << as->remotePart.name
               << "() != 0) ? obj->"
               << as->remotePart.name
               << "()->id() : ";
      }
      stream << "0);" << endl;
      n++;
    }
  }
  stream << getIndent()
         << "m_insertStatement->execute();"
         << endl << getIndent()
         << "obj->setId(m_insertStatement->getUInt64("
         << n << "));" << endl;
  if (isNewRequest()) {
    stream << getIndent()
           << "m_insertNewReqStatement->setUInt64(1, obj->id());"
           << endl << getIndent()
           << "m_insertNewReqStatement->setUInt64(2, obj->type());"
           << endl << getIndent()
           << "m_insertNewReqStatement->execute();"
           << endl;
  }
  // Commit if needed
  endTransUsed = true;
  stream << getIndent()
         << "if (endTransaction) {" << endl;
  m_indent++;
  stream << getIndent() << "cnvSvc()->commit();"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  m_indent--;
  // Catch exceptions if any
  stream << getIndent()
         << "} catch (castor::exception::SQLError& e) {"
         << endl;
  m_indent++;
  printSQLError(stream, "insert", members, assocs, true);
  m_indent--;
  stream << getIndent() << "}" << endl;
}

//=============================================================================
// writeCreateBufferForSelect
//=============================================================================
void CppCppDbCnvWriter::writeCreateBufferForSelect(QTextStream &stream,
                                                   bool& typeUsed,
                                                   QString name,
                                                   QString typeName,
                                                   int n,
                                                   QString stmt,
                                                   bool isEnum,
                                                   bool isAssoc,
                                                   QString remoteTypeName) {
  QString cTypeName = typeName;
  if (typeName == "longstring") {
    stream << getIndent() << "// here we should build the buffers for "
           << name << endl << getIndent()
           << "// but this is a longstring (CLOB) field, and there's"
           << endl << getIndent()
           << "// no support for bulk CLOBs insertion for the time being."
           << endl << getIndent()
           << "// Note that this method just compiles fine and is never called!"
           << endl;
    return;
  }
  if (typeName == "string") cTypeName = "const char*";
  if (typeName == "u_signed64" || typeName == "signed64") {
    cTypeName = "double";
  }
  stream << getIndent() << "// build the buffers for "
         << name
         << endl;
  if (typeName == "string") {
    // we need to compute the max len first
    stream << getIndent() << "unsigned int "
           << name << "MaxLen = 0;" << endl << getIndent()
           << "for (int i = 0; i < nb; i++) {" << endl;
    m_indent++;
    stream << getIndent() << "if (objs[i]->"
           << name << "().length()+1 > " << name
           << "MaxLen)" << endl << getIndent()
           << "  " << name << "MaxLen = objs[i]->"
           << name << "().length()+1;" << endl;
    m_indent--;
    stream << getIndent() << "}" << endl
           << getIndent()
           << "char* " << name << "Buffer = (char*) calloc(nb, "
           << name << "MaxLen);" << endl
           << getIndent()
           << "if (" << name << "Buffer == 0) {" << endl
           << getIndent()
           << "  castor::exception::OutOfMemory e;" << endl
           << getIndent()
           << "  throw e;" << endl
           << getIndent()
           << "}" << endl
           << getIndent()
           << "allocMem.push_back(" << name << "Buffer);" << endl;
  } else {
    addInclude("<stdlib.h>");
    stream << getIndent()
           << cTypeName << "* " << name << "Buffer = ("
           << cTypeName << "*) malloc(nb * sizeof("
           << cTypeName << "));" << endl
           << getIndent()
           << "if (" << name << "Buffer == 0) {" << endl
           << getIndent()
           << "  castor::exception::OutOfMemory e;" << endl
           << getIndent()
           << "  throw e;" << endl
           << getIndent()
           << "}" << endl
           << getIndent()
           << "allocMem.push_back(" << name << "Buffer);" << endl;
  }
  addInclude("<stdlib.h>");
  stream << getIndent()
         << "unsigned short* " << name
         << "BufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));"
         << endl << getIndent()
         << "if (" << name << "BufLens == 0) {" << endl
         << getIndent()
         << "  castor::exception::OutOfMemory e;" << endl
         << getIndent()
         << "  throw e;" << endl
         << getIndent()
         << "}" << endl
         << getIndent()
         << "allocMem.push_back(" << name << "BufLens);" << endl
         << getIndent()
         << "for (int i = 0; i < nb; i++) {" << endl;
  m_indent++;
  if (name == "creationTime" ||
      name == "lastModificationTime") {
    stream << getIndent()
           << name << "Buffer[i] = " << "time(0)";
  } else {
    if (isAssoc) {
      if (isEnum) {
        stream << getIndent()
               << name << "Buffer[i] = "
               << "(int) objs[i]->" << name << "()";
      } else {
        typeUsed = true;
        stream << getIndent()
               << name << "Buffer[i] = "
               << "(type == OBJ_"
               << remoteTypeName
               << " && objs[i]->" << name
               << "() != 0) ? objs[i]->"
               << name
               << "()->id() : 0";
      }
    } else {
      if (typeName == "string") {
        stream << getIndent()
               << "strncpy(" << name << "Buffer+(i*"
               << name << "MaxLen), objs[i]->"
               << name << "().c_str(), "
               << name << "MaxLen)";
      } else {
        stream << getIndent()
               << name << "Buffer[i] = "
               << "objs[i]->"
               << name << "()";
      }
    }
  }
  stream << ";" << endl << getIndent()
         << name << "BufLens[i] = ";
  if (typeName == "string") {
    stream << "objs[i]->" << name << "().length()+1; // + 1 for the trailing \\0";
  } else {
    stream << "sizeof(" << cTypeName << ");";
  }
  stream << endl;
  m_indent--;
  stream << getIndent() << "}" << endl
         << getIndent()
         << stmt << "->setDataBuffer" << endl
         << getIndent()
         << "  (" << n << ", "
         << name << "Buffer, "
         << getDbTypeConstant(typeName) << ", ";
  if (typeName == "string") {
    stream << name << "MaxLen";
  } else {
    stream << "sizeof("
           << name << "Buffer[0])";
  }
  stream << ", " << name << "BufLens);" << endl;
}

//=============================================================================
// writeBulkCreateRepContent
//=============================================================================
void CppCppDbCnvWriter::writeBulkCreateRepContent(QTextStream &stream, bool &addressUsed,
                                                  bool &typeUsed) {
  // check whether something needs to be done
  stream << getIndent()
         << "// check whether something needs to be done"
         << endl << getIndent()
         << "int nb = objects.size();" << endl
         << getIndent()
         << "if (0 == nb) return;" << endl;
  // Cast all objects
  stream << getIndent()
         << "// Casts all objects"
         << endl << getIndent()
         << fixTypeName("vector", "", "") << "<"
         << m_originalPackage
         << m_classInfo->className << "*> objs;" << endl
         << getIndent()
         << "for (int i = 0; i < nb; i++) {" << endl;
  m_indent++;
  stream << getIndent() << "objs.push_back(dynamic_cast<"
         << m_originalPackage
         << m_classInfo->className << "*>(objects[i]));"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  // Start of try block for database statements
  stream << getIndent() << fixTypeName("vector", "", "") << "<void *> allocMem;" << endl;
  stream << getIndent() << "try {" << endl;
  m_indent++;
  // Check the statements
  MemberList members = createMembersList();
  AssocList assocs = createAssocsList();
  writeCreateRepCheckStatements(stream, members, assocs, true);
  // Go through members and create buffers for the bulk insertion
  int n = 1;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    // declare buffers, one for the data, one for the lengths
    if (mem->name != "id" &&
        mem->name != "nbAccesses" &&
        mem->name != "lastAccessTime") {
      writeCreateBufferForSelect(stream, typeUsed, mem->name,
                                 getDbCType(mem->typeName),
                                 n, "m_bulkInsertStatement");
      n++;
    }
  }
  addInclude("\"castor/exception/OutOfMemory.hpp\"");
  // Go through the associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (isEnum(as->remotePart.typeName)) {
      writeCreateBufferForSelect(stream, typeUsed, as->remotePart.name,
                                 "int", n, "m_bulkInsertStatement", true);
      n++;
    } else if (as->type.multiRemote == MULT_ONE &&
               as->remotePart.name != "") {
      writeCreateBufferForSelect(stream, typeUsed, as->remotePart.name,
                                 "u_signed64", n, "m_bulkInsertStatement", false,
                                 true, as->remotePart.typeName);
      n++;
    }
  }
  // Prepare the buffer for the returned ids
  stream << getIndent() << "// build the buffers for returned ids"
         << endl << getIndent()
         << "double* idBuffer = (double*) calloc(nb, sizeof(double));"
         << endl << getIndent()
         << "if (idBuffer == 0) {"
         << endl << getIndent()
         << "  castor::exception::OutOfMemory e;"
         << endl << getIndent()
         << "  throw e;"
         << endl << getIndent()
         << "}"
         << endl << getIndent()
         << "allocMem.push_back(idBuffer);"
         << endl << getIndent()
         << "unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));"
         << endl << getIndent()
         << "if (idBufLens == 0) {"
         << endl << getIndent()
         << "  castor::exception::OutOfMemory e;"
         << endl << getIndent()
         << "  throw e;"
         << endl << getIndent()
         << "}"
         << endl << getIndent()
         << "allocMem.push_back(idBufLens);"
         << endl << getIndent()
         << "m_bulkInsertStatement->setDataBuffer" << endl
         << getIndent()
         << "  (" << n << ", "
         << "idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);" << endl;
  // Execute bulk insertion
  stream << getIndent()
         << "m_bulkInsertStatement->execute(nb);"
         << endl;
  // Store returned ids into the objects
  stream << getIndent()
         << "for (int i = 0; i < nb; i++) {" << endl;
  m_indent++;
  stream << getIndent()
         << "objects[i]->setId((u_signed64)idBuffer[i]);"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  if (isNewRequest()) {
    // Prepare the buffers for bulk insertion into NewRequest
    stream  << getIndent()
            << "// reuse idBuffer for bulk insertion into NewRequest"
            << endl << getIndent()
            << "m_insertNewReqStatement->setDataBuffer" << endl
            << getIndent()
            << "  (1, idBuffer, "
            << getDbTypeConstant("u_signed64")
            << ", sizeof(idBuffer[0]), idBufLens);" << endl;
    writeCreateBufferForSelect(stream, typeUsed, "type", "int",
                               2, "m_insertNewReqStatement");
    // Execute bulk insertion into NewRequest
    stream << getIndent()
           << "m_insertNewReqStatement->execute(nb);"
           << endl;
  }
  // Release all buffers
  stream << getIndent() << "// release the buffers"
         << endl << getIndent()
         << "for (unsigned int i = 0; i < allocMem.size(); i++) {"
         << endl << getIndent()
         << "  free(allocMem[i]);"
         << endl << getIndent()
         << "}"
         << endl;
  // Commit if needed
  stream << getIndent()
         << "if (endTransaction) {" << endl;
  m_indent++;
  stream << getIndent() << "cnvSvc()->commit();"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  m_indent--;
  // Catch exceptions if any
  stream << getIndent()
         << "} catch (castor::exception::SQLError& e) {"
         << endl;
  // Release all buffers
  m_indent++;
  stream << getIndent() << "// release the buffers"
         << endl << getIndent()
         << "for (unsigned int i = 0; i < allocMem.size(); i++) {"
         << endl << getIndent()
         << "  free(allocMem[i]);"
         << endl << getIndent()
         << "}"
         << endl;
  printSQLError(stream, "bulkInsert", members, assocs, true);
  m_indent--;
  stream << getIndent() << "}" << endl;
}

//=============================================================================
// writeUpdateRepContent
//=============================================================================
void CppCppDbCnvWriter::writeUpdateRepContent(QTextStream &stream,
                                              bool &addressUsed) {
  // Get the precise object
  stream << getIndent() << m_originalPackage
         << m_classInfo->className << "* obj = " << endl
         << getIndent() << "  dynamic_cast<"
         << m_originalPackage
         << m_classInfo->className << "*>(object);"
         << endl;
  // check whether something needs to be done
  stream << getIndent()
         << "// check whether something needs to be done"
         << endl << getIndent()
         << "if (0 == obj) return;" << endl;
  // Start of try block for database statements
  stream << getIndent() << "try {" << endl;
  m_indent++;
  // First check the statements
  stream << getIndent()
         << "// Check whether the statements are ok"
         << endl;
  stream << getIndent()
         << "if (0 == m_updateStatement) {" << endl;
  m_indent++;
  stream << getIndent()
         << "m_updateStatement = createStatement(s_updateStatementString);"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  // Updates the objects in the database
  stream << getIndent()
         << "// Update the current object"
         << endl;
  // Go through the members
  MemberList members = createMembersList();
  Member* idMem = 0;
  unsigned int n = 1;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    if (mem->name == "id") {
      idMem = mem;
      continue;
    }
    if (mem->name == "id" ||
        mem->name == "creationTime" ||
        mem->name == "lastAccessTime" ||
        mem->name == "nbAccesses") continue;
    if (mem->name == "lastModificationTime") {
      stream << getIndent()
             << "m_updateStatement->setInt("
             << n << ", time(0));" << endl;
    } else {
      writeSingleSetIntoStatement(stream, "update", *mem, n);
    }
    n++;
  }
  // Go through dependant objects
  AssocList assocs = createAssocsList();
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (isEnum(as->remotePart.typeName)) {
      writeSingleSetIntoStatement(stream, "update", as->remotePart, n, true);
      n++;
    }
  }
  // Last thing : the id
  if (0 == idMem) {
    stream << getIndent()
           << "INTERNAL ERROR : NO ID ATTRIBUTE" << endl;
  } else {
    writeSingleSetIntoStatement(stream, "update", *idMem, n);
  }
  // Now execute statement
  stream << getIndent()
         << "m_updateStatement->execute();"
         << endl;
  // Commit if needed
  stream << getIndent()
         << "if (endTransaction) {" << endl;
  m_indent++;
  stream << getIndent() << "cnvSvc()->commit();"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  m_indent--;
  // Catch exceptions if any
  stream << getIndent()
         << "} catch (castor::exception::SQLError& e) {"
         << endl;
  m_indent++;
  AssocList emptyList;
  printSQLError(stream, "update", members, emptyList, true);
  m_indent--;
  stream << getIndent() << "}" << endl;
}

//=============================================================================
// writeDeleteRepContent
//=============================================================================
void CppCppDbCnvWriter::writeDeleteRepContent(QTextStream &stream,
                                              bool &addressUsed) {
  // Get the precise object
  stream << getIndent() << m_originalPackage
         << m_classInfo->className << "* obj = " << endl
         << getIndent() << "  dynamic_cast<"
         << m_originalPackage
         << m_classInfo->className << "*>(object);"
         << endl;
  // check whether something needs to be done
  stream << getIndent()
         << "// check whether something needs to be done"
         << endl << getIndent()
         << "if (0 == obj) return;" << endl;
  // Start of try block for database statements
  stream << getIndent() << "try {" << endl;
  m_indent++;
  // First check the statements
  stream << getIndent()
         << "// Check whether the statements are ok"
         << endl << getIndent()
         << "if (0 == m_deleteStatement) {" << endl;
  m_indent++;
  stream << getIndent()
         << "m_deleteStatement = createStatement(s_deleteStatementString);"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  // Delete the object from the database
  stream << getIndent()
         << "// Now Delete the object"
         << endl << getIndent()
         << "m_deleteStatement->setUInt64(1, obj->id());"
         << endl << getIndent()
         << "m_deleteStatement->execute();"
         << endl;
  // create a list of associations
  AssocList assocs = createAssocsList();
  // Go through dependant objects
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (!isEnum(as->remotePart.typeName) &&
        as->type.kind == COMPOS_PARENT) {
      fixTypeName(as->remotePart.typeName,
                  getNamespace(as->remotePart.typeName),
                  m_classInfo->packageName);
      if (as->type.multiRemote == MULT_ONE) {
        // One to one association
        stream << getIndent() << "if (obj->"
               << as->remotePart.name
               << "() != 0) {" << endl;
        m_indent++;
        fixTypeName("DbCnvSvc",
                    "castor::db",
                    m_classInfo->packageName);
        stream << getIndent()
               << "cnvSvc()->deleteRep(0, obj->"
               << as->remotePart.name << "(), false);"
               << endl;
        m_indent--;
        stream << getIndent() << "}" << endl;
      } else if (as->type.multiRemote == MULT_N) {
        // One to n association, loop over the vector
        stream << getIndent() << "for ("
               << fixTypeName("vector", "", "")
               << "<"
               << fixTypeName(as->remotePart.typeName,
                              getNamespace(as->remotePart.typeName),
                              m_classInfo->packageName)
               << "*>::iterator it = obj->"
               << as->remotePart.name
               << "().begin();" << endl << getIndent()
               << "     it != obj->"
               << as->remotePart.name
               << "().end();" << endl << getIndent()
               << "     it++) {"  << endl;
        m_indent++;
        fixTypeName("DbCnvSvc",
                    "castor::db",
                    m_classInfo->packageName);
        stream << getIndent()
               << "cnvSvc()->deleteRep(0, *it, false);"
               << endl;
        m_indent--;
        stream << getIndent() << "}" << endl;
      } else {
        stream << getIndent() << "UNKNOWN MULT for association to "
               << as->remotePart.typeName << endl;
      }
    }
  }
  // Delete dependant objects
  if (! assocs.isEmpty()) {
    // Delete links to objects
    for (Assoc* as = assocs.first();
         0 != as;
         as = assocs.next()) {
      if ((as->type.kind == COMPOS_PARENT ||
           as->type.kind == AGGREG_PARENT) &&
          as->type.multiRemote == MULT_N &&
          as->type.multiLocal == MULT_N) {
        stream << getIndent()
               << "// Delete "
               << as->remotePart.name << " object"
               << endl << getIndent()
               << "if (0 != obj->"
               << as->remotePart.name << "()) {" << endl;
        m_indent++;
        // N to N association
        // Here we will use a dedicated table for the association
        // Find out the parent and child in this table
        stream << getIndent()
               << "// Check whether the statement is ok"
               << endl << getIndent()
               << "if (0 == m_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "Statement) {" << endl;
        m_indent++;
        stream << getIndent()
               << "m_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "Statement = createStatement(s_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "StatementString);"
               << endl;
        m_indent--;
        stream << getIndent() << "}" << endl
               << getIndent()
               << "// Delete links to objects"
               << endl << getIndent()
               << "m_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "Statement->setUInt64(1, obj->"
               << as->remotePart.name << "()->id());"
               << endl << getIndent()
               << "m_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "Statement->setUInt64(2, obj->id());"
               << endl << getIndent()
               << "m_delete"
               << capitalizeFirstLetter(as->remotePart.typeName)
               << "Statement->execute();"
               << endl;
        m_indent--;
        stream << getIndent() << "}" << endl;
      }
    }
  }
  // Commit if needed
  stream << getIndent()
         << "if (endTransaction) {" << endl;
  m_indent++;
  stream << getIndent() << "cnvSvc()->commit();"
         << endl;
  m_indent--;
  stream << getIndent() << "}" << endl;
  m_indent--;
  // Catch exceptions if any
  stream << getIndent()
         << "} catch (castor::exception::SQLError& e) {"
         << endl;
  m_indent++;
  MemberList emptyList;
  printSQLError(stream, "delete", emptyList, assocs, true);
  m_indent--;
  stream << getIndent() << "}" << endl;
}

//=============================================================================
// writeCreateObjCheckStatements
//=============================================================================
void CppCppDbCnvWriter::writeCreateObjCheckStatements(QString name) {
  // First check the select statement
  *m_stream << getIndent()
            << "// Check whether the statement is ok"
            << endl << getIndent()
            << "if (0 == m_" << name << "Statement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_" << name << "Statement = createStatement(s_"
            << name << "StatementString);"
            << endl;
  if (name == "bulkSelect") {
    *m_stream << getIndent()
              << "m_" << name
              << "Statement->registerOutParam(2, castor::db::DBTYPE_CURSOR);"
              << endl;
  }
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
}

//=============================================================================
// writeBulkCreateObjCreateObject
//=============================================================================
void CppCppDbCnvWriter::writeBulkCreateObjCreateObject(MemberList& members,
                                                       AssocList& assocs) {
  *m_stream << getIndent()
            << "// create the new Object" << endl
            << getIndent() << m_originalPackage
            << m_classInfo->className << "* object = new "
            << m_originalPackage << m_classInfo->className
            << "();" << endl << getIndent()
            << "// Now retrieve and set members" << endl;
  // Go through the members
  unsigned int n = 1;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    writeSingleGetFromSelect(*mem, n);
    n++;
  }
  // Go through the one to one associations dealing with enums
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      bool isenum = isEnum(as->remotePart.typeName);
      if (isenum) {
        writeSingleGetFromSelect
          (as->remotePart, n, !isenum, isenum);
      }
      n++;
    }
  }
}

//=============================================================================
// writeCreateObjContent
//=============================================================================
void CppCppDbCnvWriter::writeCreateObjContent() {
  *m_stream << getIndent() << fixTypeName("BaseAddress",
                                          "castor",
                                          m_classInfo->packageName)
            << "* ad = " << endl
            << getIndent() << "  dynamic_cast<"
            << fixTypeName("BaseAddress",
                           "castor",
                           m_classInfo->packageName)
            << "*>(address);"
            << endl;
  // Start of try block for database statements
  *m_stream << getIndent() << "try {" << endl;
  m_indent++;
  // Check the statements
  writeCreateObjCheckStatements("select");
  // Execute statement
  *m_stream << getIndent() << "// retrieve the object from the database"
            << endl << getIndent()
            << "m_selectStatement->setUInt64(1, ad->target());"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();"
            << endl << getIndent()
            << "if (!rset->next()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << fixTypeName("NoEntry",
                           "castor.exception",
                           m_classInfo->packageName)
            << " ex;" << endl << getIndent()
            << "ex.getMessage() << \"No object found for id :\""
            << " << ad->target();" << endl
            << getIndent() << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl;
  // create a list of members
  MemberList members = createMembersList();
  // create a list of associations
  AssocList assocs = createAssocsList();
  // create the object
  writeBulkCreateObjCreateObject(members, assocs);
  // Close request
  *m_stream << getIndent() << "delete rset;"
            << endl;
  // Return result
  *m_stream << getIndent() << "return object;" << endl;
  // Catch exceptions if any
  m_indent--;
  *m_stream << getIndent()
            << "} catch (castor::exception::SQLError& e) {"
            << endl;
  m_indent++;
  printSQLError(*m_stream, "select", members, assocs, false);
  m_indent--;
  *m_stream << getIndent()
            << "}" << endl;
}

//=============================================================================
// writeBulkCreateObjContent
//=============================================================================
void CppCppDbCnvWriter::writeBulkCreateObjContent() {
  // check whether something needs to be done
  *m_stream << getIndent()
            << "// Prepare result"
            << endl << getIndent()
            << fixTypeName("vector", "", "")
            << "<castor::IObject*> res;"
            << endl << getIndent()
            << "// check whether something needs to be done"
            << endl << getIndent()
            << fixTypeName("VectorAddress",
                           "castor",
                           m_classInfo->packageName)
            << "* ad = " << endl
            << getIndent() << "  dynamic_cast<"
            << fixTypeName("VectorAddress",
                           "castor",
                           m_classInfo->packageName)
            << "*>(address);"
            << endl << getIndent()
            << "int nb = ad->target().size();" << endl
            << getIndent()
            << "if (0 == nb) return res;" << endl;
  // Cast address
  // Start of try block for database statements
  *m_stream << getIndent() << "try {" << endl;
  m_indent++;
  // Check the statements
  writeCreateObjCheckStatements("bulkSelect");
  // Prepare the buffer for the ids
  *m_stream << getIndent() << "// set the buffer for input ids"
            << endl << getIndent()
            << "m_bulkSelectStatement->setDataBufferUInt64Array(1, ad->target());"
            << endl;
  // Execute statement
  *m_stream << getIndent() << "// Execute statement"
            << endl << getIndent()
            << "m_bulkSelectStatement->execute();"
            << endl;
  // Go through the results
  *m_stream << getIndent()
            << "// get the result, that is a cursor on the selected rows"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset ="
            << endl << getIndent()
            << "  m_bulkSelectStatement->getCursor(2);"
            << endl << getIndent()
            << "// loop and create the new objects"
            << endl << getIndent()
            << "bool status = rset->next();"
            << endl << getIndent()
            << "while (status) {"
            << endl;
  m_indent++;
  // create a list of members to be saved
  MemberList members = createMembersList();
  // Go through the one to one associations dealing with enums
  AssocList assocs = createAssocsList();
  // Create the object associated to the next row
  writeBulkCreateObjCreateObject(members, assocs);
  *m_stream << getIndent() << "// store object in results and loop;" << endl
            << getIndent() << "res.push_back(object);" << endl
            << getIndent() << "status = rset->next();" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent()
            << "delete rset;"
            << endl << getIndent() << "return res;" << endl;
  // Catch exceptions if any
  m_indent--;
  *m_stream << getIndent()
            << "} catch (castor::exception::SQLError& e) {"
            << endl;
  m_indent++;
  printSQLError(*m_stream, "bulkSelect", members, assocs, false);
  m_indent--;
  *m_stream << getIndent()
            << "}" << endl;
}

//=============================================================================
// writeUpdateObjContent
//=============================================================================
void CppCppDbCnvWriter::writeUpdateObjContent() {
  // Start of try block for database statements
  *m_stream << getIndent() << "try {" << endl;
  m_indent++;
  // First check the select statement
  *m_stream << getIndent()
            << "// Check whether the statement is ok"
            << endl << getIndent()
            << "if (0 == m_selectStatement) {" << endl;
  m_indent++;
  *m_stream << getIndent()
            << "m_selectStatement = createStatement(s_selectStatementString);"
            << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl
            << getIndent()
            << "// retrieve the object from the database"
            << endl << getIndent()
            << "m_selectStatement->setUInt64(1, obj->id());"
            << endl << getIndent()
            << "castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();"
            << endl << getIndent()
            << "if (!rset->next()) {"
            << endl;
  m_indent++;
  *m_stream << getIndent()
            << fixTypeName("NoEntry",
                           "castor.exception",
                           m_classInfo->packageName)
            << " ex;" << endl << getIndent()
            << "ex.getMessage() << \"No object found for id :\""
            << " << obj->id();" << endl
            << getIndent() << "throw ex;" << endl;
  m_indent--;
  *m_stream << getIndent() << "}" << endl << getIndent()
            << "// Now retrieve and set members" << endl;
  // Get the precise object
  *m_stream << getIndent() << m_originalPackage
            << m_classInfo->className << "* object = " << endl
            << getIndent() << "  dynamic_cast<"
            << m_originalPackage
            << m_classInfo->className << "*>(obj);"
            << endl;
  // Go through the members
  MemberList members = createMembersList();
  unsigned int n = 1;
  for (Member* mem = members.first();
       0 != mem;
       mem = members.next()) {
    if (mem->stereotype == SQLONLY) continue;
    writeSingleGetFromSelect(*mem, n);
    n++;
  }
  // create a list of associations
  AssocList assocs = createAssocsList();
  // Go through the one to one associations
  for (Assoc* as = assocs.first();
       0 != as;
       as = assocs.next()) {
    if (as->remoteStereotype == SQLONLY) continue;
    if (as->type.multiRemote == MULT_ONE &&
        as->remotePart.name != "") {
      // Enums are a simple case
      if (isEnum(as->remotePart.typeName)) {
        writeSingleGetFromSelect(as->remotePart, n, false, true);
      }
      n++;
    }
  }
  // Close request
  *m_stream << getIndent()
            << "delete rset;"
            << endl;
  // Catch exceptions if any
  m_indent--;
  *m_stream << getIndent()
            << "} catch (castor::exception::SQLError& e) {"
            << endl;
  m_indent++;
  printSQLError(*m_stream, "update", members, assocs, false);
  m_indent--;
  *m_stream << getIndent()
            << "}" << endl;
}

//=============================================================================
// writeSingleSetIntoStatement
//=============================================================================
void
CppCppDbCnvWriter::writeSingleSetIntoStatement(QTextStream &stream,
                                               QString statement,
                                               Member mem,
                                               int n,
                                               bool isEnum) {
  // deal with arrays of chars
  bool isArray = mem.typeName.find('[') > 0;
  if (isArray) {
    int i1 = mem.typeName.find('[');
    int i2 = mem.typeName.find(']');
    QString length = mem.typeName.mid(i1+1, i2-i1-1);
    stream << getIndent() << fixTypeName("string", "", "")
           << " " << mem.name << "S((const char*)" << "obj->"
           << mem.name << "(), " << length << ");"
           << endl;
  }
  stream << getIndent()
         << "m_" << statement << "Statement->set";
  if (isEnum) {
    stream << "Int";
  } else {
    stream << getDbType(mem.typeName);
  }
  stream << "(" << n << ", ";
  if (isArray) {
    stream << mem.name << "S";
  } else {
    if (isEnum) {
      stream << "(int)";
    }
    stream << "obj->"
           << mem.name
           << "()";
  }
  stream << ");" << endl;
}

//=============================================================================
// writeSingleGetFromSelect
//=============================================================================
void CppCppDbCnvWriter::writeSingleGetFromSelect(Member mem,
                                                 int n,
                                                 bool isAssoc,
                                                 bool isEnum) {
  *m_stream << getIndent();
  // deal with arrays of chars
  bool isArray = mem.typeName.find('[') > 0;
  if (isAssoc) {
    *m_stream << "u_signed64 " << mem.name
              << "Id = ";
  } else {
    *m_stream << "object->set"
              << capitalizeFirstLetter(mem.name)
              << "(";
  }
  if (isArray) {
    *m_stream << "("
              << mem.typeName.left(mem.typeName.find('['))
              << "*)";
  }
  if (isEnum) {
    *m_stream << "(enum "
              << fixTypeName(mem.typeName,
                             getNamespace(mem.typeName),
                             m_classInfo->packageName)
              << ")";
  }
  //if (isAssoc || mem.typeName == "u_signed64") {
  //  *m_stream << "(u_signed64)";
  //}
  *m_stream << "rset->get";
  if (isEnum) {
    *m_stream << "Int";
  } else if (isAssoc) {
    *m_stream << "Int64";
  } else {
    *m_stream << getDbType(mem.typeName);
  }
  *m_stream << "(" << n << ")";
  if (isArray) *m_stream << ".data()";
  if (!isAssoc) *m_stream << ")";
  *m_stream << ";" << endl;
}

//=============================================================================
// getDbType
//=============================================================================
QString CppCppDbCnvWriter::getDbType(QString& type) {
  QString dbType = getSimpleType(type);
  if (dbType == "short" ||
      dbType == "long" ||
      dbType == "bool" ||
      dbType == "int") {
    dbType = "Int";
  } else if (dbType == "u_signed64") {
    dbType = "UInt64";
  } else if (dbType == "signed64") {
    dbType = "Int64";
  } else if (dbType == "longstring") {
    dbType = "Clob";
  }
  if (dbType.startsWith("char")) {
    if (type.startsWith("unsigned")) {
      dbType = "Int";
    } else {
      dbType = "String";
    }
  }
  dbType = capitalizeFirstLetter(dbType);
  return dbType;
}

//=============================================================================
// getDbCType
//=============================================================================
QString CppCppDbCnvWriter::getDbCType(QString& type) {
  QString dbType = getSimpleType(type);
  if (dbType == "longstring") {
    return dbType;
  }
  if (dbType.startsWith("char")) {
    if (type.startsWith("unsigned")) {
      dbType = "unsigned char";
    } else {
      dbType = "char*";
    }
  }
  return dbType;
}

//=============================================================================
// getOraSQLType
//=============================================================================
QString CppCppDbCnvWriter::getOraSQLType(QString& type) {
  QString SQLType = getSimpleType(type);
  if (SQLType == "short" ||
      SQLType == "long" ||
      SQLType == "bool" ||
      SQLType == "int" ||
      SQLType == "float" ||
      SQLType == "double") {
    SQLType = "NUMBER";
  } else if ((type == "u_signed64") || (type == "signed64")) {
    SQLType = "INTEGER";
  } else if (SQLType == "string") {
    SQLType = "VARCHAR2(2048)";
  } else if (SQLType == "longstring") {
    SQLType = "CLOB";
  } else if (SQLType.left(5) == "char["){
    QString res = "CHAR(";
    res.append(SQLType.mid(5, SQLType.find("]")-5));
    res.append(")");
    SQLType = res;
  } else if (m_castorTypes.find(SQLType) != m_castorTypes.end()) {
    SQLType = "NUMBER";
  } else if (type == "unsigned char") {
    SQLType = "INTEGER";
  }
  return SQLType;
}

//=============================================================================
// getDbTypeConstant
//=============================================================================
QString CppCppDbCnvWriter::getDbTypeConstant(QString type) {
  QString dbType = getSimpleType(type);
  if (dbType == "short" ||
      dbType == "long" ||
      dbType == "bool" ||
      dbType == "int") {
    dbType = "DBTYPE_INT";
  } else if (dbType == "u_signed64") {
    dbType = "DBTYPE_UINT64";
  } else if (dbType == "signed64") {
    dbType = "DBTYPE_INT64";
  } else if (dbType == "float") {
    dbType = "DBTYPE_FLOAT";
  } else if (dbType == "double") {
    dbType = "DBTYPE_DOUBLE";
  } else if (dbType == "longstring") {
    dbType = "DBTYPE_CLOB";
  } else if (dbType == "string") {
    dbType = "DBTYPE_STRING";
  } else if (dbType.startsWith("char")) {
    if (type.startsWith("unsigned")) {
      dbType = "DBTYPE_INT";
    } else {
      dbType = "DBTYPE_STRING";
    }
  } else if (m_castorTypes.find(dbType) != m_castorTypes.end()) {
    dbType = "DBTYPE_UINT64";
  }
  return "castor::db::" + dbType;
}

//=============================================================================
// printSQLError
//=============================================================================
void CppCppDbCnvWriter::printSQLError(QTextStream &stream,
                                      QString name,
                                      MemberList& members,
                                      AssocList& assocs,
                                      bool useAutoCommit) {
  fixTypeName("DbCnvSvc", "castor::db", m_classInfo->packageName);
  if (useAutoCommit) {
    stream << getIndent()
           << "// Always try to rollback" << endl
           << getIndent()
           << "try {" << endl
           << getIndent()
           << "  if (endTransaction) cnvSvc()->rollback();" << endl
           << getIndent()
           << "} catch (castor::exception::Exception& ignored) {}"
           << endl;
  }
  stream << getIndent()
         << fixTypeName("InvalidArgument",
                        "castor.exception",
                        m_classInfo->packageName)
         << " ex;"
         << endl << getIndent()
         << "ex.getMessage() << \"Error in " << name
         << " request :\""
         << endl << getIndent()
         << "                << std::endl << e.getMessage().str() << std::endl";
  if (name == "select") {
    stream << endl << getIndent()
           << "                << \"Statement was : \" << std::endl"
           << endl << getIndent()
           << "                << s_" << name
           << "StatementString << std::endl"
           << endl << getIndent()
           << "                << \" and id was \" << ad->target() << std::endl;";
  } else if (name == "delete") {
    stream << endl << getIndent()
           << "                << \"Statement was : \" << std::endl"
           << endl << getIndent()
           << "                << s_" << name
           << "StatementString << std::endl"
           << endl << getIndent()
           << "                << \" and id was \" << obj->id() << std::endl;";
  } else if (name == "update") {
    stream << endl << getIndent()
           << "                << \"Statement was : \" << std::endl"
           << endl << getIndent()
           << "                << s_" << name
           << "StatementString << std::endl"
           << endl << getIndent()
           << "                << \" and id was \" << obj->id() << std::endl;";
  } else if (name == "insert") {
    stream << endl << getIndent()
           << "                << \"Statement was : \" << std::endl"
           << endl << getIndent()
           << "                << s_" << name
           << "StatementString << std::endl"
           << endl << getIndent()
           << "                << \" and parameters' values were :\" << std::endl";
    for (Member* mem = members.first();
         0 != mem;
         mem = members.next()) {
      if (mem->stereotype == SQLONLY) continue;
      stream << endl << getIndent()
             << "                << \"  "
             << mem->name << " : \" << obj->"
             << mem->name << "() << std::endl";
    }
    // Go through the associations
    for (Assoc* as = assocs.first();
         0 != as;
         as = assocs.next()) {
      if (as->type.multiRemote == MULT_ONE &&
          as->remotePart.name != "") {
        if (as->remoteStereotype == SQLONLY) continue;
        stream << endl << getIndent()
               << "                << \"  "
               << as->remotePart.name << " : \" << ";
        if (isEnum(as->remotePart.typeName)) {
          stream << "obj->" << as->remotePart.name
                 << "() << std::endl";
        } else {
          stream << "(obj->" << as->remotePart.name
                 << "() ? obj->" << as->remotePart.name
                 << "()->id() : 0) << std::endl";
        }
      }
    }
  } else if (name == "bulkInsert") {
    stream << endl << getIndent()
           << "                << \" was called in bulk with \""
           << endl << getIndent()
           << "                << nb << \" items.\" << std::endl";
  } else if (name == "bulkSelect") {
    stream << endl << getIndent()
           << "                << \" was called in bulk with \""
           << endl << getIndent()
           << "                << nb << \" items.\" << std::endl";
  }
  stream << ";" << endl << getIndent()
         << "throw ex;" << endl;
}

//=============================================================================
// isNewRequest
//=============================================================================
bool CppCppDbCnvWriter::isNewRequest() {
  UMLObject* obj = getClassifier(QString("Request"));
  const UMLClassifier *concept = dynamic_cast<UMLClassifier*>(obj);
  UMLObject* obj2 = getClassifier(QString("FileRequest"));
  const UMLClassifier *concept2 = dynamic_cast<UMLClassifier*>(obj2);
  return m_classInfo->allSuperclasses.contains(concept) &&
    !m_classInfo->allSuperclasses.contains(concept2);
}

