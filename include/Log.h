#pragma once
#ifndef GUARD_LOG_H
#define GUARD_LOG_H

/*****************************************************************************************
------------------------------------------------------------------------------------------
* Author: Jorge Luis Martinez
* Provides logging capabilities to report errors into a .txt file.
* Creation date: 10/05/2008
* Version :		LOG 1.0
------------------------------------------------------------------------------------------
- NOTES:  The Log is made of two classes.  The first, LogIO, is the one used to comunicate
directly with the log streams such as files or stringstreams.  The second is Log, which
is created by each class with logging capabilities and they are the ones which comunicate with
LogIO.
*****************************************************************************************/

// INCLUDES //////////////////////////////////////////////////////////////////////////////

/* STD */
#include <fstream>
#include <ostream>
#include <vector>
#include <string>

/*** LUDOS - Game namespace ***/
namespace Ludos{

// CONSTANTS /////////////////////////////////////////////////////////////////////////////

const unsigned int	LogIO_InvalidStream = -1;
const unsigned int	LogIO_FlushWait = 8;
const char			LogIO_TabChar = '\t';

// CLASSES ///////////////////////////////////////////////////////////////////////////////

/* Represents the id of a stream linked to the Log */
class StreamID{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	StreamID();
	StreamID( int ID );
	StreamID( const StreamID& Other );

	// OPERATORS /////////////////////////////////////////////////////////////////////////
	StreamID& operator=( const StreamID& Other );
	operator int();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	bool isValid();

private:
	int m_ID;
};

/* This is the object that will have the I/O capabilities. Only one must exist, and it may
be shared by all the Log objects */
class LogIO{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	LogIO();
	
	// UTILITIES /////////////////////////////////////////////////////////////////////////

	StreamID	AddStream( std::ostream &Stream );
	bool		RemoveStream( StreamID& ID );
	
	bool 		Write( const std::string &szString );
	bool		Flush();

	/* Adds one tab to the IO, returns the total number of tabs */
	int			AddTab() { return ++m_nTabs; }
	/* Removes on tab from the IO, return the total number of tabs */
	int			RemoveTab() { return (m_nTabs == 0 ? m_nTabs : --m_nTabs); }
	
private:
// TYPES /////////////////////////////////////////////////////////////////////////////////

	/* This type will hold sizes of the streams vector */
	typedef std::vector<std::ostream*>::size_type vec_sz;

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	/* This will handle all the streams were the log will write */
	std::vector<std::ostream*> m_vStreams;

	/* Keeps track of how many tabs the log has to write before a message */
	int m_nTabs;

	/* Keeps track of the number of writes done to the log */
	unsigned int m_nWrites;
};

class Log{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	Log( const std::string& szName = "" );

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	/* Mirrors AddStream of LogIO */
	StreamID AddStream( std::ostream &Stream ){
		return StaticLog.AddStream( Stream );
	}

	/* Mirrors RemoveStream of LogIO */
	bool RemoveStream( StreamID& ID ){
		return StaticLog.RemoveStream( ID );
	}

	bool Write( const std::string& szMsg );
	bool ProcWrite( const std::string& szMsg, int Status );
	bool ProcWrite( const std::string& szMsg, bool Status );

	bool BlockStart( const std::string& szMsg );
	bool BlockEnd();

	bool Error( const std::string& szMsg );
	bool Warning( const std::string& szMsg );

	bool LineChange();

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	std::string GetName() const { return m_szName; }
	std::string GetName() { return m_szName; }

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	inline std::string NameString();

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	/* Name of the interface using the log */
	std::string m_szName;

	/* This will keep a LogIO instance for all the Log instances */
	static LogIO StaticLog;
};

namespace LogUtilities{
/* This is used to associate a file stream to the Log */
class LogFile{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	LogFile( const std::string& szFileName = "" );
	~LogFile();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	/* Opens the file and registers it to the LogIO */
	bool FileOpen( const std::string& szFileName );

	/* Closes the file and unregisters it from the LogIO */
	bool FileClose();

	/* Returns true if the file is valid */
	bool isValid();
private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////

	/* File stream */
	std::ofstream m_sFile;

	/* Stream ID */
	StreamID m_FileID;
};

/* This will start a block on the global log at creation and kill it at destruction */
class LogBlock{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	LogBlock( const std::string& szBlockName, const Log& AssociatedLog = Log( "" ) );
	~LogBlock();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	/* This will kill the block before destruction */
	void Kill();
private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Log m_AssociatedLog;
	bool m_bValid;
};

} // Log namespace

} // Ludos namespace

#endif