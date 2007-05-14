// Include files
#include <qstring.h>
#include <qregexp.h>
#include <qvaluelist.h>
#include <vector>

// local
#include "cpphclasswriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CppHClassWriter
//
// 2004-01-13 : Sebastien Ponce
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CppHClassWriter::CppHClassWriter(UMLDoc *parent, const char *name) :
  CppHWriter(parent, name) {}

//=============================================================================
// writeClass
//=============================================================================
void CppHClassWriter::writeClass(UMLClassifier *c) {

  // Generates class declaration
  int ClassIndentLevel = m_indent;
  writeClassDecl(c, *m_stream);

  // Deal with enumerations
  if (!m_classInfo->isInterface && isEnum(c)) {
    QPtrList<UMLAttribute> atl = c->getAttributeList();
    for (UMLAttribute *at=atl.first(); at ; ) {
      QString attrName = at->getName();
      *m_stream << getIndent() << attrName;
      QString value = at->getInitialValue();
      if (!value.isEmpty()) *m_stream << " = " << value;
      UMLAttribute *next = atl.next();
      bool isLast = next == 0;
      if (!isLast) *m_stream << ",";
      if (!at->getDoc().isEmpty())
        *m_stream << " //" << at->getDoc();
      if (!isLast) *m_stream << endl;
      at = next;
    }
    *m_stream << endl;
  } else {
    // Here, we have a real class
    //declarations of operations

    // write out field and operations decl grouped by visibility

    // PUBLIC constructor/methods/accessors
    *m_stream << getIndent(INDENT*ClassIndentLevel)
              << scopeToCPPDecl(Uml::Visibility::Public) << ":" << endl << endl;
    writeConstructorDecls(*m_stream);
    QValueList<std::pair<QString, int> > alreadyGeneratedMethods;
    writeOperations(c,true,Uml::Visibility::Public,*m_stream, alreadyGeneratedMethods);
    writeHeaderAccessorMethodDecl(c, Uml::Visibility::Public, *m_stream);

    // PROTECTED methods/accessors
    if (m_classInfo->hasMethods[Uml::Visibility::Protected]) {
      *m_stream << getIndent(INDENT*ClassIndentLevel)
                << scopeToCPPDecl(Uml::Visibility::Protected) << ":" << endl << endl;
      writeOperations(c,true,Uml::Visibility::Protected,*m_stream, alreadyGeneratedMethods);
      writeHeaderAccessorMethodDecl(c, Uml::Visibility::Protected, *m_stream);
    }

    // PRIVATE attribs/methods
    if (m_classInfo->hasMethods[Uml::Visibility::Private] ||
        m_classInfo->hasAttributes[Uml::Visibility::Private] ||
        m_classInfo->hasAssociations) {
      if (m_classInfo->hasMethods[Uml::Visibility::Private]) {
        // print visibility decl.
        *m_stream << getIndent(INDENT*ClassIndentLevel)
                  << scopeToCPPDecl(Uml::Visibility::Private) << ":" << endl << endl;
        writeOperations(c,true,Uml::Visibility::Private,*m_stream, alreadyGeneratedMethods);
      }
      if (m_classInfo->hasAttributes[Uml::Visibility::Private] ||
          m_classInfo->hasAssociations) {
        *m_stream << getIndent(INDENT*ClassIndentLevel)
                  << scopeToCPPDecl(Uml::Visibility::Private) << ":" << endl << endl;
        writeHeaderFieldDecls(c, *m_stream);
      }
    }
  }

  // end of class header
  m_indent--;
  *m_stream << getIndent() << "}; /* end of ";
  if (!m_classInfo->isInterface && isEnum(c)) {
    *m_stream << "enum";
  } else {
    *m_stream << "class";
  }
  *m_stream << " " << m_classInfo->className << " */"
            << endl << endl;

  // Enum strings
  if (!m_classInfo->isInterface && isEnum(c)) {
    QPtrList<UMLAttribute> atl = c->getAttributeList();
    int n = 0;
    for (UMLAttribute *at=atl.first(); at; at = atl.next()) {
      QString value = at->getInitialValue();
      if (!value.isEmpty()) {
        int index = atoi(value.ascii());
        if (index > n) n = index;
      }
      n++;
    }
    *m_stream << getIndent() << "/**" << endl
              << getIndent()
              << " * Names of the differents representations, used to display"
              << endl << getIndent()
              << " * correctly the " << m_classInfo->className
              << " enum" << endl << getIndent()
              << " */" << endl << getIndent()
              << "extern const char* "
              << m_classInfo->className << "Strings["
              << n << "];" << endl << endl;
  }

}

//=============================================================================
// writeAfterNSClose
//=============================================================================
void CppHClassWriter::writeAfterNSClose() {
  if (0 == m_classInfo->className.compare(QString("IObject"))) {
    // Write declaration of << operator
    *m_mainStream
      << getIndent() << "/**" << endl
      << getIndent() << " * outputs this "
      << m_classInfo->className
      << " to an output stream"
      << endl << getIndent()
      << " * This method is actually not virtual as is always the case for"
      << endl << getIndent()
      << " * streaming operators. However, it makes use of the print method"
      << endl << getIndent()
      << " * which is pure virtual." << endl
      << getIndent()<<" */" << endl << getIndent()
      << fixTypeName("ostream&", "", "")
      << " operator<<("
      << fixTypeName("ostream&", "", "")
      << " s, const "
      << fixTypeName(m_classInfo->className + "&",
                     m_classInfo->packageName, "")
      << " addr);" << endl << endl;
  }
}

//=============================================================================
// writeClassDecl
//=============================================================================
void CppHClassWriter::writeClassDecl(UMLClassifier *c,
                                     QTextStream &stream) {
  // write documentation for class
  stream << getIndent() << "/**" << endl
         << getIndent() << " * ";

  if (!m_classInfo->isInterface && isEnum(c)) {
    stream << "enum";
  } else {
    stream << "class";
  }
  stream << " " << m_classInfo->className << endl;
  stream << formatDoc(c->getDoc(),getIndent()+" * ");
  stream << getIndent()<<" */" << endl;

  // Deal with enumerations
  if (!m_classInfo->isInterface && isEnum(c)) {
    stream << getIndent() << "enum "
           << m_classInfo->className << " {" << endl;
    m_indent++;
    return;
  }

  // Generates class declaration
  stream << getIndent() << "class " << m_classInfo->className;
  bool first= true;
  UMLClassifier *superClass = m_classInfo->superclasses.first();
  UMLClassifier *superInterface = m_classInfo->superInterfaces.first();
  while (superInterface != 0 &&
         m_ignoreClasses.find(superInterface->getName()) !=
         m_ignoreClasses.end()) {
    superInterface = m_classInfo->superInterfaces.next();
  }
  while (superClass != 0) {
    if (first) {
      stream << " : ";
      first = false;
    }
    stream << "public ";
    if (superClass->getAbstract()) stream << "virtual ";
    stream << fixTypeName(superClass->getName(),
                          getNamespace(superClass->getName()),
                          m_classInfo->packageName);
    superClass = m_classInfo->superclasses.next();
    if (superClass != 0 || superInterface != 0) stream << ", ";
  }
  while (superInterface != 0) {
    if (first) {
      stream << " : ";
      first = false;
    }
    stream << "public virtual "
           << fixTypeName(superInterface->getName(),
                          getNamespace(superInterface->getName()),
                          m_classInfo->packageName);
    superInterface = m_classInfo->superInterfaces.next();
    /// Do not take into account classes to be ignored
    while (superInterface != 0 &&
           m_ignoreClasses.find(superInterface->getName()) !=
           m_ignoreClasses.end()) {
      superInterface = m_classInfo->superInterfaces.next();
    }
    if (superInterface != 0) stream << ", ";
  }
  // begin the body of the class
  stream << " {" << endl << endl;
  m_indent++;
}

//=============================================================================
// writeConstructorDecls
//=============================================================================
void CppHClassWriter::writeConstructorDecls(QTextStream &stream) {
  if (!m_classInfo->isInterface) {
    writeDocumentation("", "Empty Constructor", "", stream);
    stream << getIndent() << m_classInfo->className << "() throw();"
           << endl << endl;
  }
  writeDocumentation("", "Empty Destructor", "", stream);
  stream << getIndent() << "virtual " << "~"
         << m_classInfo->className << "() throw()";
  if (m_classInfo->isInterface) {
    stream << " {}";
  }
  stream << ";" << endl << endl;
}

//=============================================================================
// writeHeaderAccessorMethodDecl
//=============================================================================
void CppHClassWriter::writeHeaderAccessorMethodDecl(UMLClassifier *c,
                                                    Uml::Visibility permitVisibility,
                                                    QTextStream &stream) {
  // attributes (write static attributes first)
  writeHeaderAttributeAccessorMethods(permitVisibility, true, stream);
  writeHeaderAttributeAccessorMethods(permitVisibility, false, stream);
  // associations
  writeAssociationMethods
    (m_classInfo->plainAssociations, permitVisibility,
     true, c->getID(), stream);
  writeAssociationMethods
    (m_classInfo->aggregations, permitVisibility,
     true, c->getID(), stream);
  writeAssociationMethods
    (m_classInfo->compositions, permitVisibility,
     true, c->getID(), stream);
}

//=============================================================================
// writeHeaderAttributeAccessorMethods
//=============================================================================
void
CppHClassWriter::writeHeaderAttributeAccessorMethods (Uml::Visibility visibility,
                                                      bool writeStatic,
                                                      QTextStream &stream ) {
  QPtrList <UMLAttribute> * list;
  switch (visibility) {
  case Uml::Visibility::Private:
    if(writeStatic)
      list = &(m_classInfo->static_atpriv);
    else
      list = &(m_classInfo->atpriv);
    break;
  case Uml::Visibility::Protected:
    if(writeStatic)
      list = &(m_classInfo->static_atprot);
    else
      list = &(m_classInfo->atprot);
    break;
  case Uml::Visibility::Public:
  default:
    if(writeStatic)
      list = &(m_classInfo->static_atpub);
    else
      list = &(m_classInfo->atpub);
    break;
  }
  // write accessor methods for attribs we found
  writeAttributeMethods (list, stream);
}

//=============================================================================
// writeHeaderFieldDecls
//=============================================================================
void CppHClassWriter::writeHeaderFieldDecls(UMLClassifier *c,
                                            QTextStream &stream) {
  // attributes
  writeAttributeDecls(Uml::Visibility::Public, true, stream); // write static attributes first
  writeAttributeDecls(Uml::Visibility::Public, false, stream);
  writeAttributeDecls(Uml::Visibility::Protected, true, stream); // write static attributes first
  writeAttributeDecls(Uml::Visibility::Protected, false, stream);
  writeAttributeDecls(Uml::Visibility::Private, true, stream); // write static attributes first
  writeAttributeDecls(Uml::Visibility::Private, false, stream);
  // associations
  writeAssociationDecls(m_classInfo->plainAssociations,
                        Uml::Visibility::Public, c->getID(), stream);
  writeAssociationDecls(m_classInfo->aggregations,
                        Uml::Visibility::Public, c->getID(), stream);
  writeAssociationDecls(m_classInfo->compositions,
                        Uml::Visibility::Public, c->getID(), stream);
  writeAssociationDecls(m_classInfo->plainAssociations,
                        Uml::Visibility::Protected, c->getID(), stream);
  writeAssociationDecls(m_classInfo->aggregations,
                        Uml::Visibility::Protected, c->getID(), stream);
  writeAssociationDecls(m_classInfo->compositions,
                        Uml::Visibility::Protected, c->getID(), stream);
  writeAssociationDecls(m_classInfo->plainAssociations,
                        Uml::Visibility::Private, c->getID(), stream);
  writeAssociationDecls(m_classInfo->aggregations,
                        Uml::Visibility::Private, c->getID(), stream);
  writeAssociationDecls(m_classInfo->compositions,
                        Uml::Visibility::Private, c->getID(), stream);
}

//=============================================================================
// writeAttributeDecls
//=============================================================================
void CppHClassWriter::writeAttributeDecls(Uml::Visibility visibility,
                                          bool writeStatic,
                                          QTextStream &stream) {
  if(m_classInfo->isInterface)
    return;
  QPtrList <UMLAttribute> * list;
  switch (visibility) {
  case Uml::Visibility::Private:
    if(writeStatic)
      list = &(m_classInfo->static_atpriv);
    else
      list = &(m_classInfo->atpriv);
    break;
  case Uml::Visibility::Protected:
    if(writeStatic)
      list = &(m_classInfo->static_atprot);
    else
      list = &(m_classInfo->atprot);
    break;
  case Uml::Visibility::Public:
  default:
    if(writeStatic)
      list = &(m_classInfo->static_atpub);
    else
      list = &(m_classInfo->atpub);
    break;
  }
  if (list->count() > 0) {
    // write attrib declarations now
    bool isFirstAttrib = true;
    QString documentation;
    for(UMLAttribute *at=list->first(); at; at=list->next()) {
      if (m_ignoreButForDB.find(at->getName()) == m_ignoreButForDB.end()) {
        documentation = at->getDoc();
        isFirstAttrib = false;
        QString varName = "m_" + at->getName();
        QString staticValue = at->getStatic() ? "static " : "";
        QString typeName = fixTypeName(at->getTypeName(),
                                       getNamespace(at->getTypeName()),
                                       m_classInfo->packageName );
        if(!documentation.isEmpty())
          writeComment(documentation, getIndent(), stream);
        stream << getIndent() << staticValue << typeName
               << " " << varName;
        if (isLastTypeArray()) stream << arrayPart();
        stream << ";" << endl << endl;
      }
    }
  }
}

//=============================================================================
// writeAssociationDecls
//=============================================================================
void CppHClassWriter::writeAssociationDecls(QPtrList<UMLAssociation> associations,
                                            Uml::Visibility permitVisibility,
                                            Uml::IDType id,
                                            QTextStream &h) {
  if( forceSections() || !associations.isEmpty() ) {
    bool printRoleA = false, printRoleB = false;
    for(UMLAssociation *a = associations.first();
        0 != a;
        a = associations.next()) {
      if (m_ignoreButForDB.find(a->getRoleName(Uml::B)) != m_ignoreButForDB.end()) continue;
      // it may seem counter intuitive, but you want to insert the role of the
      // *other* class into *this* class.
      if (a->getUMLRole(Uml::A)->getObject()->getID() == id && a->getRoleName(Uml::B) != "")
        printRoleB = true;
      if (a->getUMLRole(Uml::B)->getObject()->getID() == id && a->getRoleName(Uml::A) != "")
        printRoleA = true;
      // First: we insert documentaion for association IF it has either role AND some documentation (!)
      if ((printRoleA || printRoleB) && !(a->getDoc().isEmpty()))
        writeComment(a->getDoc(), getIndent(), h);
      // print RoleB decl
      if (printRoleB &&
          a->getVisibility(Uml::B) == permitVisibility &&
          !a->getRoleName(Uml::B).isEmpty()) {
        QString rawClassName = a->getObject(Uml::B)->getName();
        if (!isEnum(rawClassName)) rawClassName.append("*");
        QString fieldClassName = fixTypeName(rawClassName,
                                             a->getObject(Uml::B)->getPackage(),
                                             m_classInfo->packageName);
        writeAssociationRoleDecl(fieldClassName,
                                 a->getRoleName(Uml::B),
                                 parseMulti(a->getMulti(Uml::B)),
                                 a->getRoleDoc(Uml::B),
                                 h);
      }
      // print RoleA decl
      if (printRoleA &&
          a->getVisibility(Uml::A) == permitVisibility &&
          !a->getRoleName(Uml::A).isEmpty()) {
        QString rawClassName = a->getObject(Uml::A)->getName();
        if (!isEnum(rawClassName)) rawClassName.append("*");
        QString fieldClassName = fixTypeName(rawClassName,
                                             a->getObject(Uml::A)->getPackage(),
                                             m_classInfo->packageName);
        writeAssociationRoleDecl(fieldClassName,
                                 a->getRoleName(Uml::A),
                                 parseMulti(a->getMulti(Uml::A)),
                                 a->getRoleDoc(Uml::A),
                                 h);
      }
      // reset for next association in our loop
      printRoleA = false;
      printRoleB = false;
    }
  }
}

//=============================================================================
// writeAssociationRoleDecl
//=============================================================================
void CppHClassWriter::writeAssociationRoleDecl(QString fieldClassName,
                                               QString roleName,
                                               Multiplicity multi,
                                               QString doc,
                                               QTextStream &stream) {
  // ONLY write out IF there is a rolename given
  // otherwise its not meant to be declared in the code
  if (roleName.isEmpty())
    return;
  QString indent = getIndent();
  // always put space between this and prior decl, if any
  if (!doc.isEmpty())
    writeComment(doc, indent, stream);
  // declare the association based on whether it is a single variable
  // or a List (Vector).
  if (multi == MULT_ONE) {
    QString fieldVarName = "m_" + roleName;
    stream << indent << fieldClassName << " "
           << fieldVarName << ";" << endl << endl;
  } else if (multi == MULT_N) {
    QString fieldVarName = "m_" + roleName + "Vector";
    stream << indent << fixTypeName("vector" , "", "")
           << "<" << fixTypeName (fieldClassName,
                                  getNamespace(fieldClassName),
                                  m_classInfo->packageName)
           << "> " << fieldVarName << ";"
           << endl << endl;
  } else if (multi == MULT_UNKNOWN) {
    stream << indent << "Unknown Multiplicity : "
           << fieldClassName << " " << roleName << endl;
  }
}

//=============================================================================
// writeAssociationMethods
//=============================================================================
void CppHClassWriter::writeAssociationMethods (QPtrList<UMLAssociation> associations,
                                               Uml::Visibility permitVisib,
                                               bool writePointerVar,
                                               Uml::IDType myID,
                                               QTextStream &stream) {
  if (forceSections() || !associations.isEmpty()) {
    for(UMLAssociation *a = associations.first(); a; a = associations.next()) {
      // insert the methods to access the role of the other
      // class in the code of this one
      if (a->getUMLRole(Uml::A)->getObject()->getID() == myID && a->getVisibility(Uml::A) == permitVisib) {
        // only write out IF there is a rolename given
        if(!a->getRoleName(Uml::B).isEmpty()) {
          QString fieldClassName = a->getObject(Uml::B)->getName();
          if (m_ignoreButForDB.find(a->getRoleName(Uml::B)) == m_ignoreButForDB.end()) {
            if (writePointerVar && !isEnum(fieldClassName))
              fieldClassName.append("*");
            writeAssociationRoleMethod(fieldClassName,
                                       getNamespace(fieldClassName),
                                       a->getRoleName(Uml::B),
                                       parseMulti(a->getMulti(Uml::B)),
                                       a->getRoleDoc(Uml::B),
                                       a->getChangeability(Uml::B),
                                       stream);
          }
        }
      }
      if (a->getUMLRole(Uml::B)->getObject()->getID() == myID && a->getVisibility(Uml::B) == permitVisib) {
        // only write out IF there is a rolename given
        if(!a->getRoleName(Uml::A).isEmpty()) {
          QString fieldClassName = a->getObject(Uml::A)->getName();
          if (m_ignoreButForDB.find(a->getRoleName(Uml::A)) == m_ignoreButForDB.end()) {
            if (writePointerVar && !isEnum(fieldClassName))
              fieldClassName.append("*");
            writeAssociationRoleMethod(fieldClassName,
                                       getNamespace(fieldClassName),
                                       a->getRoleName(Uml::A),
                                       parseMulti(a->getMulti(Uml::A)),
                                       a->getRoleDoc(Uml::A),
                                       a->getChangeability(Uml::A),
                                       stream);
          }
        }
      }
    }
  }
}

//=============================================================================
// writeAssociationRoleMethod
//=============================================================================
void CppHClassWriter::writeAssociationRoleMethod (QString fieldClassName,
                                                  QString fieldNamespace,
                                                  QString roleName,
                                                  Multiplicity multi,
                                                  QString description,
                                                  Uml::Changeability_Type change,
                                                  QTextStream &stream) {
  if (multi == MULT_ONE) {
    QString fieldVarName = "m_" + roleName;
    writeSingleAttributeAccessorMethods(fieldClassName,
                                        fieldNamespace,
                                        fieldVarName,
                                        roleName,
                                        description,
                                        change,
                                        false,
                                        stream);
  } else if (multi == MULT_N) {
    QString fieldVarName = "m_" + roleName + "Vector";
    writeVectorAttributeAccessorMethods (fieldClassName,
                                         fieldNamespace,
                                         fieldVarName,
                                         roleName,
                                         description,
                                         change,
                                         stream);
  } else if (multi == MULT_UNKNOWN) {
    stream << getIndent() << "Unknown Multiplicity : "
           << fieldClassName << " " << roleName << endl;
  }
}

//=============================================================================
// writeAttributeMethods
//=============================================================================
void CppHClassWriter::writeAttributeMethods(QPtrList <UMLAttribute> *attribs,
                                            QTextStream &stream) {
  for(UMLAttribute *at = attribs->first();
      at != 0;
      at=attribs->next()) {
    QString varName = getAttributeVariableName(at);
    QString methodBaseName = at->getName();
    methodBaseName.stripWhiteSpace();
    if (m_ignoreButForDB.find(methodBaseName) == m_ignoreButForDB.end()) {
      writeSingleAttributeAccessorMethods(at->getTypeName(),
                                          getNamespace(at->getTypeName()),
                                          varName,
                                          methodBaseName,
                                          at->getDoc(),
                                          Uml::chg_Changeable,
                                          at->getStatic(),
                                          stream);
    }
  }
}

//=============================================================================
// writeVectorAttributeAccessorMethods
//=============================================================================
void CppHClassWriter::writeVectorAttributeAccessorMethods (QString fieldClassName,
                                                           QString fieldNamespace,
                                                           QString fieldVarName,
                                                           QString fieldName,
                                                           QString description,
                                                           Uml::Changeability_Type changeType,
                                                           QTextStream &stream) {
  fieldClassName = fixTypeName(fieldClassName,
                               fieldNamespace,
                               m_classInfo->packageName);
  // ONLY IF changeability is NOT Frozen
  if (changeType != Uml::chg_Frozen) {
    writeDocumentation
      ("Add a " + fieldClassName + " object to the " + fieldVarName + " list",
       description,
       "",
       stream);
    stream << getIndent() << "void " << "add"
           << capitalizeFirstLetter(fieldName)
           << "(" << fieldClassName << " add_object) {"
           << endl;
    m_indent++;
    stream << getIndent() << fieldVarName
           << ".push_back(add_object);" << endl;
    m_indent--;
    stream << getIndent() << "}" << endl << endl;
  }

  // ONLY IF changeability is Changeable
  if (changeType == Uml::chg_Changeable) {
    writeDocumentation
      ("Remove a " + fieldClassName + " object from " +
       fieldVarName,
       "",
       "",
       stream);
    stream << getIndent() << "void " << "remove"
           << capitalizeFirstLetter(fieldName)
           << "(" << fieldClassName << " remove_object) {"
           << endl;
    m_indent++;
    stream << getIndent() << "for (unsigned int i = 0; i < "
           << fieldVarName << ".size(); i++) {" << endl;
    m_indent++;
    stream << getIndent() << fieldClassName << " item = "
           << fieldVarName << "[i];" << endl << getIndent()
           << "if (item == remove_object) {" << endl;
    m_indent++;
    stream << getIndent() << fixTypeName("vector" , "", "")
           << "<" << fixTypeName (fieldClassName,
                                  fieldNamespace,
                                  m_classInfo->packageName)
           << ">::iterator it = "
           << fieldVarName << ".begin() + i;" << endl
           << getIndent() << fieldVarName << ".erase(it);"
           << endl << getIndent() << "return;" << endl;
    m_indent--;
    stream << getIndent() << "}" << endl;
    m_indent--;
    stream << getIndent() << "}" << endl;
    m_indent--;
    stream << getIndent() << "}" << endl << endl;
  }

  // always allow getting the list of stuff
  writeDocumentation
    ("Get the list of " + fieldClassName + " objects held by " + fieldVarName,
     "",
     "@return list of " + fieldClassName +
     " objects held by " + fieldVarName,
     stream);
  stream << getIndent() << fixTypeName("vector" , "", "")
         << "<" << fixTypeName (fieldClassName,
                                fieldNamespace,
                                m_classInfo->packageName)
         << ">& " << fieldName << "() {" << endl;
  m_indent++;
  stream << getIndent() << "return " << fieldVarName << ";" << endl;
  m_indent--;
  stream << getIndent() << "}" << endl << endl;
}

//=============================================================================
// writeSingleAttributeAccessorMethods
//=============================================================================
void CppHClassWriter::writeSingleAttributeAccessorMethods(QString fieldClassName,
                                                          QString fieldNamespace,
                                                          QString fieldVarName,
                                                          QString fieldName,
                                                          QString description,
                                                          Uml::Changeability_Type change,
                                                          bool isStatic,
                                                          QTextStream &stream) {
  fieldClassName = fixTypeName(fieldClassName,
                               fieldNamespace,
                               m_classInfo->packageName);
  // get method
  writeDocumentation
    ("Get the value of " + fieldVarName,
     description,
     "@return the value of " + fieldVarName,
     stream);
  stream << getIndent();
  if (isLastTypeArray()) stream << "const ";
  stream << fieldClassName;
  if (isLastTypeArray()) stream << "*";
  stream << " "
         << fieldName << "() const"
         << " {" << endl;
  m_indent++;
  stream << getIndent() << "return ";
  if(isStatic)
    stream << m_classInfo->className << "::";
  stream << fieldVarName << ";" << endl;
  m_indent--;
  stream << getIndent() << "}" << endl << endl;
  if (!fieldClassName.contains("const ")) {
    // set method
    if (change == Uml::chg_Changeable && !isStatic) {
      writeDocumentation
        ("Set the value of " + fieldVarName,
         description,
         "@param new_var the new value of " + fieldVarName,
         stream);
      stream << getIndent() << "void "
             << "set" << capitalizeFirstLetter(fieldName)
             << "(";
      if (isLastTypeArray()) stream << "const ";
      stream << fieldClassName;
      if (isLastTypeArray()) stream << "*";
      stream << " new_var" << ")" << " {" << endl;
      m_indent++;
      stream << getIndent();
      if (isLastTypeArray()) {
        stream << "memcpy(" << fieldVarName
               << ", new_var, " << arrayLength()
               << " * sizeof(" << fieldClassName
               << "));" << endl;
      } else {
        if(isStatic)
          stream << m_classInfo->className << "::";
        stream << fieldVarName << " = new_var;" << endl;
      }
      m_indent--;
      stream << getIndent() << "}" << endl << endl;
    }
  }
}


