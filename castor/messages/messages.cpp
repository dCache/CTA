/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/messages/messages.hpp"
#include "castor/utils/utils.hpp"

#include <string.h>

//------------------------------------------------------------------------------
// preFillHeader
//------------------------------------------------------------------------------
castor::messages::Header castor::messages::protoTapePreFillHeader() {
  return genericPreFillHeader<TPMAGIC, PROTOCOL_TYPE_TAPE,
    PROTOCOL_VERSION_1>();
}

//------------------------------------------------------------------------------
// computeSHA1Base64
//------------------------------------------------------------------------------
std::string castor::messages::computeSHA1Base64(const std::string &data) {
  return computeSHA1Base64(data.c_str(), data.length());
}

//------------------------------------------------------------------------------
// computeSHA1Base64
//------------------------------------------------------------------------------
std::string castor::messages::computeSHA1Base64(void const* const data,
  const int len){
  // Create a context and hash the data
  EVP_MD_CTX ctx;
  EVP_MD_CTX_init(&ctx);
  EVP_SignInit(&ctx, EVP_sha1());
  if (!EVP_SignUpdate(&ctx,data, len)) {
    EVP_MD_CTX_cleanup(&ctx);
    throw castor::exception::Exception("cant compute SHA1");
  }
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  EVP_DigestFinal_ex(&ctx, md_value, &md_len);
  // cleanup context
  EVP_MD_CTX_cleanup(&ctx);
  
  // base64 encode 
  BIO *b64 = BIO_new(BIO_f_base64());
  BIO *bmem = BIO_new(BIO_s_mem());
  if (NULL == b64 || NULL == bmem) {
    throw castor::exception::Exception("cant set up the environnement for computing the SHA1 in base64");
  }
  b64 = BIO_push(b64, bmem);
  BIO_write(b64, md_value, md_len);
  (void)BIO_flush(b64);
  BUF_MEM* bptr;
  BIO_get_mem_ptr(b64, &bptr);

  std::string ret(bptr->data,bptr->length);
  BIO_free(bmem);
  BIO_free(b64);
  
  return ret;
}
