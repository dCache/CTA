
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Sep Mon 1 2003
 */

#include <cstdlib> //to get the user name

#include <qdatetime.h>
#include <qregexp.h>
#include <qdir.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <kapplication.h>

#include "../dialogs/overwritedialogue.h"

#include "simplecodegenerator.h"

#include <qregexp.h>
#include "../attribute.h"
#include "../class.h"
#include "../classifier.h"
#include "../codedocument.h"
#include "../operation.h"

// Constructors/Destructors
//

SimpleCodeGenerator::SimpleCodeGenerator (UMLDoc * parentDoc , const char * name, bool /*createDirHierarchyForPackages*/)
  : CodeGenerator( parentDoc, name)
{
	parentDoc->disconnect(this); // disconnect from UMLDoc.. we arent planning to be synced at all
	initFields(parentDoc);
}

SimpleCodeGenerator::~SimpleCodeGenerator ( ) { }

//
// Methods
//

// Accessor methods
//


// Other methods
//

QString SimpleCodeGenerator::findFileName(UMLClassifier* concept, QString ext) {

        //if we already know to which file this class was written/should be written, just return it.
        if(m_fileMap->contains(concept))
                return ((*m_fileMap)[concept]);

        //else, determine the "natural" file name
        QString name;
        // Get the package name
        QString package = concept->getPackage();

        // Replace all white spaces with blanks
        package.simplifyWhiteSpace();

        // Replace all blanks with underscore
        package.replace(QRegExp(" "), "_");

        // if package is given add this as a directory to the file name
        if (!package.isEmpty()) {
                name = package + "." + concept->getName();
                package.replace(QRegExp("\\."), "/");
                package = "/" + package;
        } else {
                name = concept->getName();
        }

        // Convert all "." to "/" : Platform-specific path separator
        name.replace(QRegExp("\\."),"/"); // Simple hack!
        if (ext != ".java" && ext != ".pm" && ext != ".py" &&
            ext != ".cpp" && ext != ".hpp" && ext != ".h") {
                package = package.lower();
                name = name.lower();
        }

        // if a package name exists check the existence of the package directory
        if (!package.isEmpty()) {
                QDir packageDir(m_outputDirectory.absPath() + package);
                if (! (packageDir.exists() || packageDir.mkdir(packageDir.absPath()) ) ) {
                        KMessageBox::error(0, i18n("Cannot create the package folder:\n") +
                                           packageDir.absPath() + i18n("\nPlease check the access rights"),
                                           i18n("Cannot Create Folder"));
                        return NULL;
                }
        }

        name.simplifyWhiteSpace();
        name.replace(QRegExp(" "),"_");

        ext.simplifyWhiteSpace();
        ext.replace(QRegExp(" "),"_");

        return overwritableName(concept, name, ext);
}

QString SimpleCodeGenerator::overwritableName(UMLClassifier* concept, QString name, QString ext) {
        //check if a file named "name" with extension "ext" already exists
        if(!m_outputDirectory.exists(name+ext)) {
                m_fileMap->insert(concept,name);
                return name; //if not, "name" is OK and we have not much to to
        }

        int suffix;
        OverwriteDialogue overwriteDialogue( name+ext, m_outputDirectory.absPath(),
                                             m_applyToAllRemaining, kapp -> mainWidget() );
        switch(m_overwrite) {  //if it exists, check the OverwritePolicy we should use
                case CodeGenerationPolicy::Ok:                //ok to overwrite file
                        break;
                case CodeGenerationPolicy::Ask:               //ask if we can overwrite
                        switch(overwriteDialogue.exec()) {
                                case KDialogBase::Yes:  //overwrite file
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                m_overwrite = CodeGenerationPolicy::Ok;
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        break;
                                case KDialogBase::No: //generate similar name
                                        suffix = 1;
                                        while( m_outputDirectory.exists(name + "__" + QString::number(suffix) + ext) ) {
                                                suffix++;
                                        }
                                        name = name + "__" + QString::number(suffix);
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                m_overwrite = CodeGenerationPolicy::Never;
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        break;
                              case KDialogBase::Cancel: //don't output anything
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                m_overwrite = CodeGenerationPolicy::Cancel;
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        return NULL;
                                        break;
                        }

                        break;
                case CodeGenerationPolicy::Never: //generate similar name
                        suffix = 1;
                        while( m_outputDirectory.exists(name + "__" + QString::number(suffix) + ext) ) {
                                suffix++;
                        }
			name = name + "__" + QString::number(suffix);
                        break;
                case CodeGenerationPolicy::Cancel: //don't output anything
                        return NULL;
                        break;
        }

        m_fileMap->insert(concept,name);
        return name;
}


bool SimpleCodeGenerator::hasDefaultValueAttr(UMLClass *c) {
        QPtrList<UMLAttribute> *atl = c->getFilteredAttributeList();
        for(UMLAttribute *at = atl->first(); at; at = atl->next())
                if(!at->getInitialValue().isEmpty())
                        return true;
        return false;
}

bool SimpleCodeGenerator::hasAbstractOps(UMLClassifier *c) {
        UMLOperationList opl = c->getFilteredOperationsList();
        for(UMLOperation *op = opl.first(); op ; op = opl.next())
                if(op->getAbstract())
                        return true;
        return false;
}

/**
 * @return	ClassifierCodeDocument
 * @param	classifier
 * @param	this This package generator object.
 */
CodeDocument * SimpleCodeGenerator::newClassifierCodeDocument(UMLClassifier* /*classifier*/)
{
	return (CodeDocument*)NULL;
}

// write all concepts in project to file
void SimpleCodeGenerator::writeCodeToFile ( ) {
  m_fileMap->clear(); // yeah, need to do this, if not, just keep getting same damn directory to write to.
  UMLClassifierList concepts = m_doc->getClassesAndInterfaces();
  for (UMLClassifier *c = concepts.first(); c; c = concepts.next())
    this->writeClass(c); // call the writer for each class.
}

// write only selected concepts to file
void SimpleCodeGenerator::writeCodeToFile ( UMLClassifierList & concepts) {
        m_fileMap->clear(); // ??
        for (UMLClassifier *c = concepts.first(); c; c = concepts.next())
		this->writeClass(c); // call the writer for each class.
}

void SimpleCodeGenerator::initFields ( UMLDoc * parentDoc ) {

        // load Classifier documents from parent document
        // initFromParentDocument();

        m_fileMap = new QMap<UMLClassifier*,QString>;
        m_applyToAllRemaining = true;
	m_doc = parentDoc;

	// this really is just being used to sync the internal params
	// to the codegenpolicy as there are no code documents to really sync.
	syncCodeToDocument();
}

// a little method to provide some compatability between
// the newer codegenpolicy object and the older class fields.
void SimpleCodeGenerator::syncCodeToDocument() {

	CodeGenerationPolicy *policy = getPolicy();

        m_overwrite = policy->getOverwritePolicy();
        m_modname = policy->getModifyPolicy();
        m_forceDoc = policy->getCodeVerboseDocumentComments();
        m_forceSections = policy->getCodeVerboseSectionComments();
        m_outputDirectory = QDir(policy->getOutputDirectory().absPath());
        m_headingFiles = QDir(policy->getHeadingFileDir());

}


// override parent method
void SimpleCodeGenerator::initFromParentDocument( )
{
	// Do nothing
}


#include "simplecodegenerator.moc"
