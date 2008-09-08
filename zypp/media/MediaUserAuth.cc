/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/media/MediaUserAuth.cc
 *
 */

#include <list>
#include <boost/format.hpp>

#include "zypp/base/Gettext.h"
#include "zypp/base/String.h"

#include "zypp/media/MediaException.h"
#include "zypp/media/MediaUserAuth.h"


namespace zypp {
  namespace media {


AuthData::AuthData(const Url & url)
  : _url(url)
{
  _username = url.getUsername();
  _password = url.getPassword();
}


bool AuthData::valid() const
{
  return username().size() && password().size();
}

std::ostream & AuthData::dumpOn( std::ostream & str ) const
{
  str << "username: '" << _username << "'" << std::endl
      << "password: " << (_password.empty() ? "<empty>" : "<non-empty>")
      << std::endl;
  return str;
}


bool CurlAuthData::valid() const
{
  return username().size() && password().size() && _auth_type != CURLAUTH_NONE;
}


std::ostream & CurlAuthData::dumpOn( std::ostream & str ) const
{
  AuthData::dumpOn(str) << " auth_type: " << _auth_type_str
    << " (" << _auth_type << ")" << std::endl;
  return str;
}

long CurlAuthData::auth_type_str2long(std::string & auth_type_str)
{
  curl_version_info_data *curl_info = curl_version_info(CURLVERSION_NOW);

  std::vector<std::string>                  list;
  std::vector<std::string>::const_iterator  it;
  long                                      auth_type = CURLAUTH_NONE;

  zypp::str::split(auth_type_str, std::back_inserter(list), ",");

  for(it = list.begin(); it != list.end(); ++it)
  {
    if(*it == "basic")
    {
      auth_type |= CURLAUTH_BASIC;
    }
    else
    if(*it == "digest")
    {
      auth_type |= CURLAUTH_DIGEST;
    }
    else
    if((curl_info && (curl_info->features & CURL_VERSION_NTLM)) &&
       (*it == "ntlm"))
    {
      auth_type |= CURLAUTH_NTLM;
    }
    else
    if((curl_info && (curl_info->features & CURL_VERSION_SPNEGO)) &&
       (*it == "spnego" || *it == "negotiate"))
    {
      // there is no separate spnego flag for this auth type
      auth_type |= CURLAUTH_GSSNEGOTIATE;
    }
    else
    if((curl_info && (curl_info->features & CURL_VERSION_GSSNEGOTIATE)) &&
       (*it == "gssnego" || *it == "negotiate"))
    {
      auth_type |= CURLAUTH_GSSNEGOTIATE;
    }
    else
    {
      std::string msg = boost::str(
        boost::format (_("Unsupported HTTP authentication method '%s'")) % *it);

      ZYPP_THROW(MediaException(msg));
    }
  }

  return auth_type;
}

std::string CurlAuthData::auth_type_long2str(long auth_type)
{
  std::list<std::string> auth_list;

  if(auth_type & CURLAUTH_GSSNEGOTIATE)
    auth_list.push_back("negotiate");

  if(auth_type & CURLAUTH_NTLM)
    auth_list.push_back("ntlm");

  if(auth_type & CURLAUTH_DIGEST)
    auth_list.push_back("digest");

  if(auth_type & CURLAUTH_BASIC)
    auth_list.push_back("basic");

  return str::join(auth_list, ",");
}


std::ostream & operator << (std::ostream & str, AuthData & auth_data)
{
  auth_data.dumpOn(str);
  return str;
}

std::ostream & operator << (std::ostream & str, CurlAuthData & auth_data)
{
  auth_data.dumpOn(str);
  return str;
}


  } // namespace media
} // namespace zypp
