/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/source/SourceImpl.h
 *
*/
#ifndef ZYPP_SOURCE_SOURCEIMPL_H
#define ZYPP_SOURCE_SOURCEIMPL_H

#include <iosfwd>
#include <string>

#include "zypp/base/ReferenceCounted.h"
#include "zypp/base/NonCopyable.h"
#include "zypp/base/PtrTypes.h"
#include "zypp/Source.h"

#include "zypp/ResStore.h"

#include "zypp/Pathname.h"
#include "zypp/media/MediaManager.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  namespace source
  { /////////////////////////////////////////////////////////////////

    DEFINE_PTR_TYPE(SourceImpl);

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : SourceImpl
    //
    /** Base class for concrete Source implementations.
     *
     * Constructed by \ref SourceFactory. Public access via \ref Source
     * interface.
    */
    class SourceImpl : public base::ReferenceCounted, private base::NonCopyable
    {
      friend std::ostream & operator<<( std::ostream & str, const SourceImpl & obj );

    public:
      /** SourceImpl MediaVerifier. */
      class Verifier;

    public:

      /** All resolvables provided by this source. */
      const ResStore & resolvables(Source_Ref) const
      { return resolvables(); }
      const ResStore & resolvables() const;

      /**
       * Provide a file to local filesystem
       *
       * \throws Exception
       *
       */
      const Pathname provideFile(const Pathname & file,
				 const unsigned media_nr = 1,
				 bool cached = false,
				 bool checkonly = false);

      /**
       * Provide a directory to local filesystem
       *
       * \throws Exception
       *
       */
      const Pathname provideDir(const Pathname & path,
				const unsigned media_nr = 1,
				const bool recursive = false);

      const bool enabled() const
      { return _enabled; }

      void enable()
      { _enabled = true; }

      void disable()
      { _enabled = false; }

      virtual void storeMetadata(const Pathname & cache_dir_r);

      std::string alias (void) const
      { return _alias; }

      virtual std::string id (void) const;
      virtual void setId (const std::string id_r);
      virtual unsigned priority (void) const;
      virtual void setPriority (unsigned p);
      virtual unsigned priorityUnsubscribed (void) const;
      virtual void setPriorityUnsubscribed (unsigned p);

      Url url (void) const;

      const Pathname & path (void) const;

      /**
       * ZMD backend specific stuff
       * default source only provides dummy implementations
       */
      virtual std::string zmdName (void) const;
      virtual void setZmdName (const std::string name_r) const;
      virtual std::string zmdDescription (void) const;
      virtual void setZmdDescription (const std::string desc_r) const;

    protected:
      /** Provide Source_Ref to \c this. */
      Source_Ref selfSourceRef()
      { return Source_Ref( this ); }

    protected:
      /** All resolvables provided by this source. */
      ResStore _store;
      /** Handle to media which contains this source */
      media::MediaId _media;
      /** Path to the source on the media */
      Pathname _path;
      /** The source is enabled */
      bool _enabled;
      /** (user defined) alias of the source */
      std::string _alias;
      /** Directory holding metadata cache */
      Pathname _cache_dir;
      /** (user defined) id of the source
          mostly used for ZENworks */
      std::string _id;
      /** (user defined) default priority of the source */
      unsigned _priority;
      /** (user defined) unsubscribed priority of the source */
      unsigned _priority_unsubscribed;

      ///////////////////////////////////////////////////////////////////
      // no playgroung below this line ;)
      ///////////////////////////////////////////////////////////////////
    protected:
      /** Ctor. */
      SourceImpl(media::MediaId & media_r,
                 const Pathname & path_r = "/",
		 const std::string & alias = "",
		 const Pathname cache_dir_r = "");
      /** Dtor. */
      virtual ~SourceImpl();

      /** Overload to realize stream output. */
      virtual std::ostream & dumpOn( std::ostream & str ) const;

    private:
      /** Late initialize the ResStore. */
      virtual void createResolvables(Source_Ref source_r);
      /** Whether the ResStore is initialized. */
      bool _res_store_initialized;

    private:
      /** Ctor, excl. for nullimpl only. */
      SourceImpl()
      : _res_store_initialized(true)
      {}

    public:
      /** Offer default Impl. */
      static SourceImpl_Ptr nullimpl()
      {
        static SourceImpl_Ptr _nullimpl( new SourceImpl );
        return _nullimpl;
      }

    };
    ///////////////////////////////////////////////////////////////////

    /** \relates SourceImpl Stream output */
    inline std::ostream & operator<<( std::ostream & str, const SourceImpl & obj )
    { return obj.dumpOn( str ); }

    ///////////////////////////////////////////////////////////////////

    /** SourceImpl MediaVerifier.
    */
    class SourceImpl::Verifier : public media::MediaVerifierBase
      {
      public:
	/** ctor */
	Verifier (const std::string & vendor_r, const std::string & id_r);
	/*
	 ** Check if the specified attached media contains
	 ** the desired media number (e.g. SLES10 CD1).
	 */
	virtual bool
	isDesiredMedia(const media::MediaAccessRef &ref, media::MediaNr mediaNr);

      private:
	std::string _media_vendor;
	std::string _media_id;
	SourceImpl_Ptr _source;
      };

    /////////////////////////////////////////////////////////////////
  } // namespace source
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_SOURCE_SOURCEIMPL_H
