/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          CTA Frontend Tape Namespace query class
 * @copyright      Copyright 2019 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <xroot_plugins/GrpcClient.hpp>

namespace cta { 

class Namespace
{
public:
  Namespace(const std::string &endpoint, const std::string &token) :
    m_grpcClient(::eos::client::GrpcClient::Create(endpoint, token)) { }

  std::string getPath(const std::string &diskFileId);

private:
  std::unique_ptr<::eos::client::GrpcClient> m_grpcClient;
};

} // namespace cta
