/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_VDQM_TAPEDRIVESTATUSCODES_HPP
#define CASTOR_VDQM_TAPEDRIVESTATUSCODES_HPP

#ifdef __cplusplus
namespace castor {

  namespace vdqm {

#endif
    /**
     * enum TapeDriveStatusCodes
     * Contains all  possible status of the TapeDrive
     */
    enum TapeDriveStatusCodes {
      UNIT_UP = 0, //This means, that the unit is up and free. So it is ready to accept a new request.
      UNIT_STARTING = 1, //The tape drive is reserved for one tape request and waits to be ASSIGNED to it.
      UNIT_ASSIGNED = 2, //A tape is in the tape drive but is not MOUNTED, yet. It can happen that it still receives a message to release the tape. In this case it will returns to UNIT_UP.
      VOL_MOUNTED = 3, //A tape is mounted and the assigned request for this tape is currently in process.
      FORCED_UNMOUNT = 4, //A FORCE_UNMOUNT can interrupt a request in process. In this case the tape drive waits, that the tpdaemon sends an unmount message. 
      UNIT_DOWN = 5, //When the tape drive is down, it means that it is not ready to receive any requests. It must first be set back to UNIT_UP status.
      WAIT_FOR_UNMOUNT = 6, //The tape is still in the tape drive, but there are no more requests for it in the queue. So it just waits for the unmount command from the tpdaemon to come back to UNIT_UP status or to go immediatly to UNIT_STARTING status.
      STATUS_UNKNOWN = 7, //If an unexpected command comes from the client, which stays in conflict with the current status of the tape drive, the status will be set to STATUS_UNKNOWN.
      id //The id of this object
    }; // end of enum TapeDriveStatusCodes

    /**
     * Names of the differents representations, used to display
     * correctly the TapeDriveStatusCodes enum
     */
    extern const char* TapeDriveStatusCodesStrings[9];

#ifdef __cplusplus
  }; // end of namespace vdqm

}; // end of namespace castor

#endif
#endif // CASTOR_VDQM_TAPEDRIVESTATUSCODES_HPP
