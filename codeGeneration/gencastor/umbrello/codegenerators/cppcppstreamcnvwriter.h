#ifndef CODEGENERATORS_CPPCPPSTREAMCNVWRITER_H 
#define CODEGENERATORS_CPPCPPSTREAMCNVWRITER_H 1

// Include files

// Local includes
#include "cppcppbasecnvwriter.h"

class CppCppStreamCnvWriter : public CppCppBaseCnvWriter {

 public:
  /**
   * Constructor
   */
  CppCppStreamCnvWriter(UMLDoc *parent, const char *name);

  /**
   * Destructor
   */
  ~CppCppStreamCnvWriter() {};

 public:

  /**
   * Initializes the writer. Only calls the method of
   * the parent, then fixes the namespace and calls postinit.
   * It also inserts the header file in the list of includes
   */
  virtual bool init(UMLClassifier* c, QString fileName);

  /**
   * write the content of the file
   */
  void writeClass(UMLClassifier *c);

 private:

  /// writes constructor and destructor
  void writeConstructors();
    
  /// writes createRep method's content
  void writeCreateRepContent();
    
  /// writes updateRep method's content
  void writeUpdateRepContent();
    
  /// writes deleteRep method's content
  void writeDeleteRepContent();
    
  /// writes createObj method's content
  void writeCreateObjContent();
    
  /// writes updateObj method's content
  void writeUpdateObjContent();
    
};

#endif // CODEGENERATORS_CPPCPPSTREAMCNVWRITER_H
