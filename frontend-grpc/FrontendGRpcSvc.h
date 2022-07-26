
#pragma once

#include "version.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>

#include <scheduler/Scheduler.hpp>
#include "common/log/Logger.hpp"
#include "cta_grpc_frontend.grpc.pb.h"

using cta::Scheduler;
using cta::catalogue::Catalogue;
using cta::frontend::rpc::CtaRpc;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class CtaRpcImpl : public CtaRpc::Service {

private:
    std::unique_ptr <cta::catalogue::Catalogue> m_catalogue;
    std::unique_ptr <cta::Scheduler> m_scheduler;
    cta::log::Logger  *m_log;

public:
    CtaRpcImpl(cta::log::Logger *logger, std::unique_ptr<cta::catalogue::Catalogue> &catalogue, std::unique_ptr<cta::Scheduler> &scheduler);

    Status Version(::grpc::ServerContext *context, const ::google::protobuf::Empty *request, ::cta::admin::Version *response);

    Status Archive(::grpc::ServerContext* context, const ::cta::frontend::rpc::ArchiveRequest* request, ::cta::frontend::rpc::ArchiveResponse* response);
    Status Retrieve(::grpc::ServerContext* context, const ::cta::frontend::rpc::RetrieveRequest* request, ::cta::frontend::rpc::RetrieveResponse* response);
    Status Delete(::grpc::ServerContext* context, const ::cta::frontend::rpc::DeleteRequest* request, ::google::protobuf::Empty* response);
    Status CancelRetrieve(::grpc::ServerContext* context, const ::cta::frontend::rpc::CancelRetrieveRequest* request, ::google::protobuf::Empty* response);
};

