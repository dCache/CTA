
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
 *      Date   : Fri Jun 20 2003
 */
#include <cstdlib> //to get the user name

#include <kconfig.h>
#include <kdeversion.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include <qstringlist.h>
#include <qregexp.h>
#include <qdatastream.h>

#include "dialogs/codegenerationpolicypage.h"
#include "codegenerationpolicy.h"

using namespace std;

#define MAXLINES 256

const CodeGenerationPolicy::OverwritePolicy CodeGenerationPolicy::DEFAULT_OVERWRITE_POLICY = Ask;
const bool CodeGenerationPolicy::DEFAULT_VERBOSE_SECTION_COMMENTS = true;
const bool CodeGenerationPolicy::DEFAULT_VERBOSE_DOCUMENT_COMMENTS = true;
const bool CodeGenerationPolicy::DEFAULT_INCLUDE_HEADINGS = true;
const CodeGenerationPolicy::NewLineType CodeGenerationPolicy::DEFAULT_LINE_ENDING_TYPE = UNIX;
const CodeGenerationPolicy::IndentationType CodeGenerationPolicy::DEFAULT_INDENT_TYPE = SPACE;
const int CodeGenerationPolicy::DEFAULT_INDENT_AMOUNT = 2;
const CodeGenerationPolicy::ModifyNamePolicy CodeGenerationPolicy::DEFAULT_MODIFY_NAME_POLICY = No;

// Constructors/Destructors
//

CodeGenerationPolicy::CodeGenerationPolicy ( QObject * parent, CodeGenerationPolicy * clone )
    : QObject ( parent , "ACodeGenPolicyDoc" )
{

	initFields();
	setDefaults(clone,false);
}

CodeGenerationPolicy::CodeGenerationPolicy ( QObject * parent, KConfig * config )
    : QObject ( parent, "ACodeGenPolicyDoc")
{
	initFields();
	setDefaults(config,false);
}

CodeGenerationPolicy::~CodeGenerationPolicy ( ) { }

//
// Methods
//


// Accessor methods
//


// Public attribute accessor methods
//

/**
 * Set the value of m_overwritePolicy
 * Policy of how to deal with overwriting existing files. Allowed values are "ask",
 * "yes" and "no".
 * @param new_var the new value of m_overwritePolicy
 */
void CodeGenerationPolicy::setOverwritePolicy ( OverwritePolicy new_var ) {
	m_overwritePolicy = new_var;
}

/**
 * Get the value of m_overwritePolicy
 * Policy of how to deal with overwriting existing files. Allowed values are "ask",
 * "yes" and "no".
 * @return the value of m_overwritePolicy
 */
CodeGenerationPolicy::OverwritePolicy CodeGenerationPolicy::getOverwritePolicy ( ) const {
	return m_overwritePolicy;
}

/**
 * Set the value of m_codeVerboseSectionComments
 * Whether or not verbose code commenting for sections is desired. If true, comments
 * for sections will be written even if the section is empty.
 * @param new_var the new value of m_codeVerboseSectionComments
 */
void CodeGenerationPolicy::setCodeVerboseSectionComments ( bool new_var ) {
	m_codeVerboseSectionComments = new_var;
	emit modifiedCodeContent();
}

/**
 * Get the value of m_codeVerboseSectionComments
 * Whether or not verbose code commenting for sections is desired. If true, comments
 * for sections will be written even if the section is empty.
 * @return the value of m_codeVerboseSectionComments
 */
bool CodeGenerationPolicy::getCodeVerboseSectionComments ( ) const {
	return m_codeVerboseSectionComments;
}

/**
 * Set the value of m_codeVerboseDocumentComments
 * Whether or not verbose code commenting for documentation is desired. If true,
 * documentation for various code will be written even if no code would normally be
 * created at that point in the file.
 * @param new_var the new value of m_codeVerboseDocumentComments
 */
void CodeGenerationPolicy::setCodeVerboseDocumentComments ( bool new_var ) {
	m_codeVerboseDocumentComments = new_var;
	emit modifiedCodeContent();
}

/**
 * Get the value of m_codeVerboseDocumentComments
 * Whether or not verbose code commenting for documentation is desired. If true,
 * documentation for various code will be written even if no code would normally be
 * created at that point in the file.
 * @return the value of m_codeVerboseDocumentComments
 */
bool CodeGenerationPolicy::getCodeVerboseDocumentComments ( ) const {
	return m_codeVerboseDocumentComments;
}

/**
 * Set the value of m_headingFileDir
 * location of the header file template.
 * @param new_var the new value of m_headingFileDir
 */
void CodeGenerationPolicy::setHeadingFileDir ( const QString & path) {
        m_headingFiles.setPath(path);
}

/**
 * Get the value of m_headingFileDir
 * location of the header file template.
 * @return the value of m_headingFileDir
 */
QString CodeGenerationPolicy::getHeadingFileDir ( ) const {
        return m_headingFiles.absPath();
}

/**
 * Set the value of m_includeHeadings
 * @param new_var the new value of m_includeHeadings
 */
void CodeGenerationPolicy::setIncludeHeadings ( bool new_var ) {
	m_includeHeadings = new_var;
	emit modifiedCodeContent();
}

/**
 * Get the value of m_includeHeadings
 * @return the value of m_includeHeadings
 */
bool CodeGenerationPolicy::getIncludeHeadings ( ) const {
	return m_includeHeadings;
}

/**
 * Set the value of m_outputDirectory
 * location of where output files will go.
 * @param new_var the new value of m_outputDirectory
 */
void CodeGenerationPolicy::setOutputDirectory ( QDir new_var ) {
	m_outputDirectory = new_var;
}

/**
 * Get the value of m_outputDirectory
 * location of where output files will go.
 * @return the value of m_outputDirectory
 */
QDir CodeGenerationPolicy::getOutputDirectory ( ) {
	return m_outputDirectory;
}

/**
 * Set the value of m_lineEndingType
 * What line ending characters to use.
 * @param new_var the new value of m_lineEndingType
 */
void CodeGenerationPolicy::setLineEndingType ( NewLineType type) {
	m_lineEndingType = type;
        switch (m_lineEndingType) {
                case MAC:
                        m_lineEndingChars = QString("\r\n");
                        break;
                case DOS:
                        m_lineEndingChars = QString("\r");
                        break;
                case UNIX:
                default:
                        m_lineEndingChars = QString("\n");
                        break;
        }
	emit modifiedCodeContent();
}

/**
 * Get the value of m_lineEndingType
 * What line ending characters to use.
 * @return the value of m_lineEndingType
 */
CodeGenerationPolicy::NewLineType CodeGenerationPolicy::getLineEndingType ( ) {
	return m_lineEndingType;
}

/** Utility function to get the actual characters.
 */
QString CodeGenerationPolicy::getNewLineEndingChars ( ) const {
	return m_lineEndingChars;
}

/**
 * Set the value of m_indentationType
 * The amount and type of whitespace to indent with.
 * @param new_var the new value of m_indentationType
 */
void CodeGenerationPolicy::setIndentationType ( IndentationType new_var ) {
	m_indentationType = new_var;
	calculateIndentation();
	emit modifiedCodeContent();
}

CodeGenerationPolicy::IndentationType CodeGenerationPolicy::getIndentationType ( ) {
	return m_indentationType;
}

void CodeGenerationPolicy::setIndentationAmount ( int amount ) {
	if(amount > -1)
	{
		m_indentationAmount = amount;
		calculateIndentation();
		emit modifiedCodeContent();
	}
}

int CodeGenerationPolicy::getIndentationAmount ( ) {
	return m_indentationAmount;
}

/**
 * Utility method to get the amount (and type of whitespace) to indent with.
 * @return the value of the indentation
 */
QString CodeGenerationPolicy::getIndentation ( ) const {
	return m_indentation;
}

void CodeGenerationPolicy::calculateIndentation ( ) {
	QString indent = "";
	m_indentation = "";
        switch (m_indentationType) {
                case NONE:
                        break;
                case TAB:
                        indent = QString("\t");
                        break;
                default:
                case SPACE:
                        indent = QString(" ");
                        break;
        }

	for (int i=0; i < m_indentationAmount; i++)
		m_indentation += indent;
}

/**
 * Set the value of m_modifyPolicy
 * @param new_var the new value of m_modifyPolicy
 */
void CodeGenerationPolicy::setModifyPolicy ( ModifyNamePolicy new_var ) {
	m_modifyPolicy = new_var;
}

/**
 * Get the value of m_modifyPolicy
 * @return the value of m_modifyPolicy
 */
CodeGenerationPolicy::ModifyNamePolicy CodeGenerationPolicy::getModifyPolicy ( ) const {
	return m_modifyPolicy;
}

/**
 * Create a new dialog interface for this object.
 * @return dialog object
 */
CodeGenerationPolicyPage * CodeGenerationPolicy::createPage ( QWidget *pWidget, const char *name ) {
	return new CodeGenerationPolicyPage ( pWidget, name, this );
}

// Other methods
//

void CodeGenerationPolicy::emitModifiedCodeContentSig() {
	emit modifiedCodeContent();
}

void CodeGenerationPolicy::setDefaults ( CodeGenerationPolicy * clone , bool emitUpdateSignal)
{

	if(!clone)
		return;

	blockSignals(true); // we need to do this because otherwise most of these
			   // settors below will each send the modifiedCodeContent() signal
			   // needlessly (we can just make one call at the end).

	setCodeVerboseSectionComments ( clone->getCodeVerboseSectionComments() );
	setCodeVerboseDocumentComments ( clone->getCodeVerboseDocumentComments() );
	setHeadingFileDir ( clone->getHeadingFileDir());
	setIncludeHeadings ( clone->getIncludeHeadings());
	setOutputDirectory ( clone->getOutputDirectory());
	setLineEndingType ( clone->getLineEndingType());
	setIndentationAmount ( clone->getIndentationAmount());
	setIndentationType ( clone->getIndentationType());
	setModifyPolicy ( clone->getModifyPolicy());
	setOverwritePolicy ( clone->getOverwritePolicy() );

	blockSignals(false); // "as you were citizen"

	if(emitUpdateSignal)
		emit modifiedCodeContent();

}

void CodeGenerationPolicy::setDefaults( KConfig * config, bool emitUpdateSignal)
{

	if(!config)
		return;

	blockSignals(true); // we need to do this because otherwise most of these
			    // settors below will each send the modifiedCodeContent() signal
			    // needlessly (we can just make one call at the end).

        config -> setGroup("Code Generation");
	setCodeVerboseDocumentComments ( config-> readBoolEntry("forceDoc",DEFAULT_VERBOSE_DOCUMENT_COMMENTS) );
	setCodeVerboseSectionComments ( config -> readBoolEntry("forceSections",DEFAULT_VERBOSE_SECTION_COMMENTS) );

        setLineEndingType ( (NewLineType) config->readNumEntry("lineEndingType",DEFAULT_LINE_ENDING_TYPE) );
        setIndentationType ( (IndentationType) config->readNumEntry("indentationType",DEFAULT_INDENT_TYPE) );
	setIndentationAmount( config->readNumEntry("indentationAmount",DEFAULT_INDENT_AMOUNT));

        QString path = config -> readPathEntry("outputDirectory");
        if(path.isEmpty())
                path = QDir::homeDirPath() + "/uml-generated-code/";
        setOutputDirectory ( QDir (path) );

        path = config -> readPathEntry("headingsDirectory");
        if(path.isEmpty()) {
                KStandardDirs stddirs;
                path =  stddirs.findDirs("data","umbrello/headings").first();
        }
	setHeadingFileDir ( path );

        setIncludeHeadings ( config->readBoolEntry("includeHeadings",DEFAULT_INCLUDE_HEADINGS) );
        setOverwritePolicy ( (OverwritePolicy)config -> readNumEntry("overwritePolicy",DEFAULT_OVERWRITE_POLICY));
        setModifyPolicy ( (ModifyNamePolicy)config -> readNumEntry("modnamePolicy",DEFAULT_MODIFY_NAME_POLICY));

	blockSignals(false); // "as you were citizen"

	if(emitUpdateSignal)
		emit modifiedCodeContent();

}

void CodeGenerationPolicy::writeConfig (KConfig * config) {

	config->setGroup("Code Generation");

        config->writeEntry("forceDoc",getCodeVerboseDocumentComments());
        config->writeEntry("forceSections",getCodeVerboseSectionComments());

        config->writeEntry("lineEndingType",getLineEndingType());
        config->writeEntry("indentationType",getIndentationType());
        config->writeEntry("indentationAmount",getIndentationAmount());

#if KDE_IS_VERSION(3,1,3)
        config->writePathEntry("outputDirectory",getOutputDirectory().absPath());
        config->writePathEntry("headingsDirectory",getHeadingFileDir());
#else
        config->writeEntry("outputDirectory",getOutputDirectory().absPath());
        config->writeEntry("headingsDirectory",getHeadingFileDir());
#endif
        config->writeEntry("includeHeadings",getIncludeHeadings());
        config->writeEntry("overwritePolicy",getOverwritePolicy());
        config->writeEntry("modnamePolicy",  getModifyPolicy());

}

QString getUserName() {
  // get login
  QString login = QString(getenv("USER"));
  // try to get real name of the user
  login.replace(QRegExp("sponce.*"), "Sebastien Ponce, sebastien.ponce@cern.ch");
  login.replace(QRegExp("obarring.*"), "Olof Barring, olof.barring@cern.ch");
  login.replace(QRegExp("bcouturi.*"), "Benjamin Couturier, benjamin.couturier@cern.ch");
  login.replace(QRegExp("jdurand.*"), "Jean-Damien Durand, jean-damien.durand@cern.ch");
  return login;
}

// return the actual text
QString CodeGenerationPolicy::getHeadingFile(QString str) {

        if(!getIncludeHeadings() || str.isEmpty())
                return QString("");
        if(str.contains(" ") ||str.contains(";")) {
                kdWarning() << "File folder must not have spaces or semi colons!" << endl;
                return QString("");
        }
        //if we only get the extension, then we look for the default
        // heading.[extension]. If there is no such file, we try to
        // get any file with the same extension
        QString filename;
        if(str.startsWith(".")) {
                if(QFile::exists(m_headingFiles.absFilePath("heading"+str)))
                        filename = m_headingFiles.absFilePath("heading"+str);
                else {
                        str.prepend('*');
                        m_headingFiles.setNameFilter(str);
                        //if there is more than one match we just take the first one
                        filename = m_headingFiles.absFilePath(m_headingFiles.entryList().first());
                       // kdWarning() << "header file name set to " << filename << " because it was *" << endl;
                }
        } else {   //we got a file name (not only extension)
                filename = m_headingFiles.absFilePath(str);
        }

        QFile f(filename);
        if(!f.open(IO_ReadOnly)) {
//                kdWarning() << "Error opening heading file: " << f.name() << endl;
//                kdWarning() << "Headings directory was " << m_headingFiles.absPath() << endl;
                return QString("");
        }

        QTextStream ts(&f);
        QString retstr = QString("");
        QString endLine = getNewLineEndingChars();
        for(int l = 0; l < MAXLINES && !ts.atEnd(); l++)
                retstr += ts.readLine()+endLine;

        //do variable substitution
        retstr.replace( QRegExp("%author%"),getUserName());  //get the user name from some where else
        retstr.replace( QRegExp("%headingpath%"),filename );
        //the replace filepath, time parts are now in the code document updateHeader method (which you may override as needed)...

        return retstr;
}

void CodeGenerationPolicy::initFields ( ) {

	blockSignals(true);

	m_overwritePolicy = DEFAULT_OVERWRITE_POLICY;
	m_codeVerboseSectionComments = DEFAULT_VERBOSE_SECTION_COMMENTS;
	m_codeVerboseDocumentComments = DEFAULT_VERBOSE_DOCUMENT_COMMENTS;
	m_includeHeadings = DEFAULT_INCLUDE_HEADINGS;
	setLineEndingType(DEFAULT_LINE_ENDING_TYPE);
	m_indentationType = DEFAULT_INDENT_TYPE;
	m_indentationAmount = DEFAULT_INDENT_AMOUNT;
	m_modifyPolicy = DEFAULT_MODIFY_NAME_POLICY;

	m_outputDirectory.setPath(QDir::home().absPath() + "/uml-generated-code/");
	m_headingFiles.setPath(QDir::home().absPath() + "/headings/");

	calculateIndentation();

	blockSignals(false);
}

#include "codegenerationpolicy.moc"
