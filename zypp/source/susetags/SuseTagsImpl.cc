/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/source/susetags/SuseTagsImpl.cc
 *
*/
#include <iostream>
#include <fstream>
#include "zypp/base/Logger.h"
#include "zypp/base/Exception.h"

#include "zypp/PathInfo.h"

#include "zypp/source/susetags/SuseTagsImpl.h"
#include "zypp/source/susetags/PackagesParser.h"
#include "zypp/source/susetags/SelectionTagFileParser.h"
#include "zypp/source/susetags/PatternTagFileParser.h"

#include "zypp/SourceFactory.h"

using std::endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  namespace source
  { /////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    namespace susetags
    { /////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      //
      //	METHOD NAME : SuseTagsImpl::SuseTagsImpl
      //	METHOD TYPE : Ctor
      //
      SuseTagsImpl::SuseTagsImpl()
      {}

      void SuseTagsImpl::factoryInit()
      {
        media::MediaManager media_mgr;
	
	std::string vendor;
	std::string media_id;

        try {
	  Pathname media_file = Pathname("media.1/media");
	  media_mgr.provideFile (_media, 1, media_file);
	  media_file = media_mgr.localPath (_media, media_file);
	  
	  std::ifstream pfile( media_file.asString().c_str() );

    	  if ( pfile.bad() ) {
    	    ERR << "Error parsing media.1/media" << endl;
    	    ZYPP_THROW(Exception("Error parsing media.1/media") );
	  }

	  vendor = str::getline( pfile, str::TRIM );

    	  if ( pfile.fail() ) {
    	    ERR << "Error parsing media.1/media" << endl;
    	    ZYPP_THROW(Exception("Error parsing media.1/media") );
	  }

	  media_id = str::getline( pfile, str::TRIM );
	  
    	  if ( pfile.fail() ) {
    	    ERR << "Error parsing media.1/media" << endl;
    	    ZYPP_THROW(Exception("Error parsing media.1/media") );
	  }

	}
	catch ( const Exception & excpt_r )
	{
	  ERR << "Cannot read /media.1/media file, cannot initialize source" << endl;
	  ZYPP_THROW( Exception("Cannot read /media.1/media file, cannot initialize source") );
	}

        try {
          MIL << "Adding susetags media verifier: " << endl;
          MIL << "Vendor: " << vendor << endl;
          MIL << "Media ID: " << media_id << endl;

          media_mgr.delVerifier(_media);
          media_mgr.addVerifier(_media, media::MediaVerifierRef(
	    new SourceImpl::Verifier (vendor, media_id) ));
        }
        catch (const Exception & excpt_r)
        {
#warning FIXME: If media data is not set, verifier is not set. Should the media
          ZYPP_CAUGHT(excpt_r);
          WAR << "Verifier not found" << endl;
        }
      }

      void SuseTagsImpl::createResolvables()
      {
        Source_Ref source_r = SourceFactory().createFrom(this);
#warning We use suse instead of <DATADIR> for now
        Pathname p = provideFile(_path + "suse/setup/descr/packages");
        DBG << "Going to parse " << p << endl;
        std::list<Package::Ptr> content( parsePackages( source_r, this, p ) );
        _store.insert( content.begin(), content.end() );
        DBG << "SuseTagsImpl (fake) from " << p << ": "
            << content.size() << " packages" << endl;


	bool file_found = true;

	// parse selections
	try {
	  p = provideFile(_path + "suse/setup/descr/selections");
	} catch (...)
	{
	    MIL << "'selections' file not found" << endl;

	    file_found = false;
	}

	if (file_found)
	{
	    std::ifstream sels (p.asString().c_str());

	    while (sels && !sels.eof())
	    {
        	std::string selfile;

		getline(sels,selfile);

		if (selfile.empty() ) continue;

		DBG << "Going to parse selection " << selfile << endl;

		Pathname file = provideFile(_path + "suse/setup/descr/" + selfile);
	        MIL << "Selection file to parse " << file << endl;

		Selection::Ptr sel( parseSelection( file ) );

		DBG << "Selection:" << sel << endl;

		if (sel)
    		    _store.insert( sel );

		DBG << "Parsing of " << file << " done" << endl;
	    }
	}

	// parse patterns
	file_found = true;

	try {
	    p = provideFile(_path + "suse/setup/descr/patterns");
	} catch (...)
	{
	    MIL << "'patterns' file not found" << endl;
	    file_found = false;
	}

	if ( file_found )
	{
	    std::ifstream pats (p.asString().c_str());

	    while (pats && !pats.eof())
	    {
        	std::string patfile;

    		getline(pats,patfile);

		if (patfile.empty() ) continue;

		DBG << "Going to parse pattern " << patfile << endl;

		Pathname file = provideFile(_path + "suse/setup/descr/" + patfile);
		MIL << "Pattern file to parse " << file << endl;

		Pattern::Ptr pat( parsePattern( file ) );

		DBG << "Pattern:" << pat << endl;

		if (pat)
    		    _store.insert( pat );

		DBG << "Parsing of " << file << " done" << endl;
	    }
        }
      }
      ///////////////////////////////////////////////////////////////////
      //
      //	METHOD NAME : SuseTagsImpl::~SuseTagsImpl
      //	METHOD TYPE : Dtor
      //
      SuseTagsImpl::~SuseTagsImpl()
      {}

      Pathname SuseTagsImpl::sourceDir( const NVRAD& nvrad )
      {
#warning Not using <DATADIR>
        return Pathname( "/suse/" + nvrad.arch.asString() + "/");
      }

      ///////////////////////////////////////////////////////////////////
      //
      //	METHOD NAME : SuseTagsImpl::dumpOn
      //	METHOD TYPE : std::ostream &
      //
      std::ostream & SuseTagsImpl::dumpOn( std::ostream & str ) const
      {
        return str << "SuseTagsImpl";
      }

      /////////////////////////////////////////////////////////////////
    } // namespace susetags
    ///////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
  } // namespace source
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
