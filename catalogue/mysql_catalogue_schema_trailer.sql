ALTER TABLE CTA_CATALOGUE ADD CONSTRAINT 
  CATALOGUE_STATUS_CONTENT_CK CHECK((NEXT_SCHEMA_VERSION_MAJOR IS NULL AND NEXT_SCHEMA_VERSION_MINOR IS NULL AND STATUS='COMPLETE') OR (STATUS='UPGRADING'));