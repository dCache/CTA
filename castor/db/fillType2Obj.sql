/* Fill Type2Obj metatable */
CREATE TABLE Type2Obj (type INTEGER PRIMARY KEY NOT NULL, object VARCHAR2(100) NOT NULL, svcHandler VARCHAR2(100));
INSERT INTO Type2Obj (type, object) VALUES (0, 'INVALID');
INSERT INTO Type2Obj (type, object) VALUES (1, 'Ptr');
INSERT INTO Type2Obj (type, object) VALUES (2, 'CastorFile');
INSERT INTO Type2Obj (type, object) VALUES (3, 'OldClient');
INSERT INTO Type2Obj (type, object) VALUES (4, 'Cuuid');
INSERT INTO Type2Obj (type, object) VALUES (5, 'DiskCopy');
INSERT INTO Type2Obj (type, object) VALUES (6, 'DiskFile');
INSERT INTO Type2Obj (type, object) VALUES (7, 'DiskPool');
INSERT INTO Type2Obj (type, object) VALUES (8, 'DiskServer');
INSERT INTO Type2Obj (type, object) VALUES (10, 'FileClass');
INSERT INTO Type2Obj (type, object) VALUES (12, 'FileSystem');
INSERT INTO Type2Obj (type, object) VALUES (13, 'IClient');
INSERT INTO Type2Obj (type, object) VALUES (14, 'MessageAck');
INSERT INTO Type2Obj (type, object) VALUES (16, 'ReqIdRequest');
INSERT INTO Type2Obj (type, object) VALUES (17, 'Request');
INSERT INTO Type2Obj (type, object) VALUES (18, 'Segment');
INSERT INTO Type2Obj (type, object) VALUES (21, 'StageGetNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (26, 'Stream');
INSERT INTO Type2Obj (type, object) VALUES (27, 'SubRequest');
INSERT INTO Type2Obj (type, object) VALUES (28, 'SvcClass');
INSERT INTO Type2Obj (type, object) VALUES (29, 'Tape');
INSERT INTO Type2Obj (type, object) VALUES (30, 'TapeCopy');
INSERT INTO Type2Obj (type, object) VALUES (31, 'TapePool');
INSERT INTO Type2Obj (type, object) VALUES (33, 'StageFileQueryRequest');
INSERT INTO Type2Obj (type, object) VALUES (34, 'StageFindRequestRequest');
INSERT INTO Type2Obj (type, object) VALUES (35, 'StageGetRequest');
INSERT INTO Type2Obj (type, object) VALUES (36, 'StagePrepareToGetRequest');
INSERT INTO Type2Obj (type, object) VALUES (37, 'StagePrepareToPutRequest');
INSERT INTO Type2Obj (type, object) VALUES (38, 'StagePrepareToUpdateRequest');
INSERT INTO Type2Obj (type, object) VALUES (39, 'StagePutDoneRequest');
INSERT INTO Type2Obj (type, object) VALUES (40, 'StagePutRequest');
INSERT INTO Type2Obj (type, object) VALUES (41, 'StageRequestQueryRequest');
INSERT INTO Type2Obj (type, object) VALUES (42, 'StageRmRequest');
INSERT INTO Type2Obj (type, object) VALUES (43, 'StageUpdateFileStatusRequest');
INSERT INTO Type2Obj (type, object) VALUES (44, 'StageUpdateRequest');
INSERT INTO Type2Obj (type, object) VALUES (45, 'FileRequest');
INSERT INTO Type2Obj (type, object) VALUES (46, 'QryRequest');
INSERT INTO Type2Obj (type, object) VALUES (48, 'StagePutNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (49, 'StageUpdateNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (50, 'StageAbortRequest');
INSERT INTO Type2Obj (type, object) VALUES (51, 'StageReleaseFilesRequest');
INSERT INTO Type2Obj (type, object) VALUES (58, 'DiskCopyForRecall');
INSERT INTO Type2Obj (type, object) VALUES (59, 'TapeCopyForMigration');
INSERT INTO Type2Obj (type, object) VALUES (60, 'GetUpdateStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (62, 'BaseAddress');
INSERT INTO Type2Obj (type, object) VALUES (64, 'Disk2DiskCopyDoneRequest');
INSERT INTO Type2Obj (type, object) VALUES (65, 'MoverCloseRequest');
INSERT INTO Type2Obj (type, object) VALUES (66, 'StartRequest');
INSERT INTO Type2Obj (type, object) VALUES (67, 'PutStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (69, 'IObject');
INSERT INTO Type2Obj (type, object) VALUES (70, 'IAddress');
INSERT INTO Type2Obj (type, object) VALUES (71, 'QueryParameter');
INSERT INTO Type2Obj (type, object) VALUES (72, 'DiskCopyInfo');
INSERT INTO Type2Obj (type, object) VALUES (73, 'Files2Delete');
INSERT INTO Type2Obj (type, object) VALUES (74, 'FilesDeleted');
INSERT INTO Type2Obj (type, object) VALUES (76, 'GCLocalFile');
INSERT INTO Type2Obj (type, object) VALUES (78, 'GetUpdateDone');
INSERT INTO Type2Obj (type, object) VALUES (79, 'GetUpdateFailed');
INSERT INTO Type2Obj (type, object) VALUES (80, 'PutFailed');
INSERT INTO Type2Obj (type, object) VALUES (81, 'GCFile');
INSERT INTO Type2Obj (type, object) VALUES (82, 'GCFileList');
INSERT INTO Type2Obj (type, object) VALUES (83, 'FilesDeletionFailed');
INSERT INTO Type2Obj (type, object) VALUES (84, 'TapeRequest');
INSERT INTO Type2Obj (type, object) VALUES (85, 'ClientIdentification');
INSERT INTO Type2Obj (type, object) VALUES (86, 'TapeServer');
INSERT INTO Type2Obj (type, object) VALUES (87, 'TapeDrive');
INSERT INTO Type2Obj (type, object) VALUES (88, 'DeviceGroupName');
INSERT INTO Type2Obj (type, object) VALUES (89, 'ErrorHistory');
INSERT INTO Type2Obj (type, object) VALUES (90, 'TapeDriveDedication');
INSERT INTO Type2Obj (type, object) VALUES (91, 'TapeAccessSpecification');
INSERT INTO Type2Obj (type, object) VALUES (92, 'TapeDriveCompatibility');
INSERT INTO Type2Obj (type, object) VALUES (93, 'PutDoneStart');
INSERT INTO Type2Obj (type, object) VALUES (95, 'SetFileGCWeight');
INSERT INTO Type2Obj (type, object) VALUES (96, 'RepackRequest');
INSERT INTO Type2Obj (type, object) VALUES (97, 'RepackSubRequest');
INSERT INTO Type2Obj (type, object) VALUES (98, 'RepackSegment');
INSERT INTO Type2Obj (type, object) VALUES (99, 'RepackAck');
INSERT INTO Type2Obj (type, object) VALUES (101, 'DiskServerDescription');
INSERT INTO Type2Obj (type, object) VALUES (102, 'FileSystemDescription');
INSERT INTO Type2Obj (type, object) VALUES (103, 'DiskPoolQuery');
INSERT INTO Type2Obj (type, object) VALUES (104, 'EndResponse');
INSERT INTO Type2Obj (type, object) VALUES (105, 'FileResponse');
INSERT INTO Type2Obj (type, object) VALUES (106, 'StringResponse');
INSERT INTO Type2Obj (type, object) VALUES (107, 'Response');
INSERT INTO Type2Obj (type, object) VALUES (108, 'IOResponse');
INSERT INTO Type2Obj (type, object) VALUES (109, 'AbortResponse');
INSERT INTO Type2Obj (type, object) VALUES (110, 'RequestQueryResponse');
INSERT INTO Type2Obj (type, object) VALUES (111, 'FileQueryResponse');
INSERT INTO Type2Obj (type, object) VALUES (112, 'FindReqResponse');
INSERT INTO Type2Obj (type, object) VALUES (113, 'GetUpdateStartResponse');
INSERT INTO Type2Obj (type, object) VALUES (114, 'BasicResponse');
INSERT INTO Type2Obj (type, object) VALUES (115, 'StartResponse');
INSERT INTO Type2Obj (type, object) VALUES (116, 'GCFilesResponse');
INSERT INTO Type2Obj (type, object) VALUES (117, 'FileQryResponse');
INSERT INTO Type2Obj (type, object) VALUES (118, 'DiskPoolQueryResponse');
INSERT INTO Type2Obj (type, object) VALUES (119, 'StageRepackRequest');
INSERT INTO Type2Obj (type, object) VALUES (120, 'DiskServerStateReport');
INSERT INTO Type2Obj (type, object) VALUES (121, 'DiskServerMetricsReport');
INSERT INTO Type2Obj (type, object) VALUES (122, 'FileSystemStateReport');
INSERT INTO Type2Obj (type, object) VALUES (123, 'FileSystemMetricsReport');
INSERT INTO Type2Obj (type, object) VALUES (124, 'DiskServerAdminReport');
INSERT INTO Type2Obj (type, object) VALUES (125, 'FileSystemAdminReport');
INSERT INTO Type2Obj (type, object) VALUES (126, 'StreamReport');
INSERT INTO Type2Obj (type, object) VALUES (127, 'FileSystemStateAck');
INSERT INTO Type2Obj (type, object) VALUES (128, 'MonitorMessageAck');
INSERT INTO Type2Obj (type, object) VALUES (129, 'Client');
INSERT INTO Type2Obj (type, object) VALUES (130, 'JobSubmissionRequest');
INSERT INTO Type2Obj (type, object) VALUES (131, 'VersionQuery');
INSERT INTO Type2Obj (type, object) VALUES (132, 'VersionResponse');
INSERT INTO Type2Obj (type, object) VALUES (133, 'StageDiskCopyReplicaRequest');
INSERT INTO Type2Obj (type, object) VALUES (134, 'RepackResponse');
INSERT INTO Type2Obj (type, object) VALUES (135, 'RepackFileQry');
INSERT INTO Type2Obj (type, object) VALUES (136, 'CnsInfoMigrationPolicy');
INSERT INTO Type2Obj (type, object) VALUES (137, 'DbInfoMigrationPolicy');
INSERT INTO Type2Obj (type, object) VALUES (138, 'CnsInfoRecallPolicy');
INSERT INTO Type2Obj (type, object) VALUES (139, 'DbInfoRecallPolicy');
INSERT INTO Type2Obj (type, object) VALUES (140, 'DbInfoStreamPolicy');
INSERT INTO Type2Obj (type, object) VALUES (141, 'PolicyObj');
INSERT INTO Type2Obj (type, object) VALUES (142, 'NsFilesDeleted');
INSERT INTO Type2Obj (type, object) VALUES (143, 'NsFilesDeletedResponse');
INSERT INTO Type2Obj (type, object) VALUES (144, 'Disk2DiskCopyStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (145, 'Disk2DiskCopyStartResponse');
INSERT INTO Type2Obj (type, object) VALUES (146, 'ThreadNotification');
INSERT INTO Type2Obj (type, object) VALUES (147, 'FirstByteWritten');
INSERT INTO Type2Obj (type, object) VALUES (148, 'VdqmTape');

