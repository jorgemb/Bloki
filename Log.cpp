#include <cassert>
#include "Log.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// StreamID :: IMPLEMENTATION //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	StreamID
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID::StreamID() : m_ID( LogIO_InvalidStream )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	StreamID
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID::StreamID( int ID ) : m_ID( ID )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	StreamID (copy constructor)
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID::StreamID( const Ludos::StreamID &Other ) : m_ID( Other.m_ID )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	operator=
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID& Ludos::StreamID::operator =( const Ludos::StreamID &Other )
{
	m_ID = Other.m_ID;

	return *this;
}

//----------------------------------------------------------------------------------------
/*  Function	:	operator <int>
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID::operator int()
{
	return m_ID;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isValid
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
bool Ludos::StreamID::isValid()
{
	assert( m_ID >= 0 || m_ID == LogIO_InvalidStream );

	if( m_ID != LogIO_InvalidStream )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// LogIO :: IMPLEMENTATION /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	LogIO
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::LogIO::LogIO() : m_nTabs(0), m_nWrites(0)
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	AddStream
*	Description	:	Adds the pointer of a stream to the array and returns it's ID.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::StreamID Ludos::LogIO::AddStream( std::ostream &Stream )
{
	// Search the vector for an empty space
	vec_sz nCounter = 0;
	while( nCounter != m_vStreams.size() )
	{
		// Found an empty spot
		if( m_vStreams[nCounter] == NULL )
		{
			m_vStreams[nCounter] = &Stream;
			return StreamID( (int)nCounter );
		}

		++nCounter;
	}

	// The vector is full, so we add a new space
	m_vStreams.push_back( &Stream );

	return StreamID( (int)m_vStreams.size() - 1 );
}

//----------------------------------------------------------------------------------------
/*  Function	:	RemoveStream
*	Description	:	Removes a given stream from the array of streams, return 1 on success
*					and 0 on failure.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogIO::RemoveStream( Ludos::StreamID& ID )
{
	// Check if the ID is in range
	if( (vec_sz)ID >= m_vStreams.size() || ID < 0 )
		return false;

	// Erase the stream
	m_vStreams[ID] = NULL;
	return true;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Write
*	Description	:	Writes a given text to all the streams in the array. On failure 0 will
*					be returned, 1 otherwise.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogIO::Write( const std::string &szString )
{
	// Check that at least we have one stream
	if( m_vStreams.size() == 0 )
		return true;

	// Holds all the tabs for the streams
	std::string szTabs;
	for( int n = 0; n != m_nTabs; ++n )
		szTabs.push_back( LogIO_TabChar );

	// Write on every stream
	for( vec_sz nCounter = 0; nCounter != m_vStreams.size(); ++nCounter )
	{
		// Write to the stream if is valid
		if( m_vStreams[nCounter] != NULL )
			*(m_vStreams[nCounter]) << szTabs << szString << '\n';
	}
	++m_nWrites;

	// Flush if necesary
	if( m_nWrites >= LogIO_FlushWait )
	{
		m_nWrites = 0;
		Flush();
	}

	return true;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Flush
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogIO::Flush()
{
	// Flush every stream
	for( vec_sz nCounter = 0; nCounter != m_vStreams.size(); ++nCounter )
		if( m_vStreams[nCounter] != NULL )
			m_vStreams[nCounter]->flush();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Log :: IMPLEMENTATION ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

Ludos::LogIO Ludos::Log::StaticLog;

//----------------------------------------------------------------------------------------
/*  Function	:	Log (constructor)
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::Log::Log( const std::string &szName ) : m_szName( szName )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	Write
*	Description	:	Writes a line to the log.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::Write( const std::string &szMsg )
{
	return StaticLog.Write( NameString() + szMsg );
}

//----------------------------------------------------------------------------------------
/*  Function	:	ProcWrite
*	Description	:	Writes the message for the success or failure of a process.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::ProcWrite( const std::string &szMsg, int Status )
{
	return ProcWrite( szMsg, Status ? true : false );
}

bool Ludos::Log::ProcWrite( const std::string &szMsg, bool Status )
{
	if( Status )
		return StaticLog.Write( "(+) " + NameString() + szMsg );
	else
		return StaticLog.Write( "(-) " + NameString() + szMsg );
}

//----------------------------------------------------------------------------------------
/*  Function	:	BlockStart
*	Description	:	Starts a block by adding a tab to the StaticLog. 
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::BlockStart( const std::string &szMsg )
{
	bool Ret = StaticLog.Write( NameString() + szMsg + " ->" );

	if( Ret )
		StaticLog.AddTab();

	return Ret;
}

//----------------------------------------------------------------------------------------
/*  Function	:	BlockEnd
*	Description	:	Ends a block by removing a tab from the StaticLog.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::BlockEnd()
{
	StaticLog.RemoveTab();

	return true;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Error
*	Description	:	Writes an error to the Log and flushes all the streams.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/19/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::Error( const std::string &szMsg )
{
	// The return value
	bool Ret = true;

	if( m_szName.size() != 0 )
		Ret = StaticLog.Write( "*** ERROR ON : " + m_szName + " *** -> " + szMsg );
	else
		Ret = StaticLog.Write( "*** ERROR *** " + szMsg );

	StaticLog.Flush();

	return Ret;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Warning
*	Description	:	Writes a warning to the log. Returns 1 on success, 0 otherwise.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/19/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::Warning( const std::string &szMsg )
{
	return StaticLog.Write( "(*)Warning(*) : " + NameString() + szMsg );
}

//----------------------------------------------------------------------------------------
/*  Function	:	LineChange
*	Description	:	Writes a line change char to the log.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::Log::LineChange()
{
	return StaticLog.Write( "" );
}

//----------------------------------------------------------------------------------------
/*  Function	:	NameString
*	Description	:	Returns the string containing the name of the associated object.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	07/26/2010
*/	
//----------------------------------------------------------------------------------------
std::string Ludos::Log::NameString()
{
	if( m_szName.size() != 0 )
		return m_szName + " - ";
	else
		return "";
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LogFile :: IMPLEMENTATION ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	LogFile
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::LogUtilities::LogFile::LogFile( const std::string &szFileName ) 
: m_FileID()
{
	FileOpen( szFileName );
}

//----------------------------------------------------------------------------------------
/*  Function	:	~LogFile
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::LogUtilities::LogFile::~LogFile()
{
	FileClose();
}

//----------------------------------------------------------------------------------------
/*  Function	:	FileOpen
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogUtilities::LogFile::FileOpen( const std::string &szName )
{
	// Validate the name
	if( szName.size() == 0 )
		return false;

	FileClose(); //Just in case

	// Open the file
	m_sFile.open( szName.c_str() );
	m_sFile.clear();
	
	if( isValid() )
	{
		// Register it to the static log
		Log Log;
		
		m_FileID = Log.AddStream( m_sFile );
		if( m_FileID.isValid() )
			return true;
		else
		{
			FileClose();
			return false;
		}
	} else {
		return false;
	}
}

//----------------------------------------------------------------------------------------
/*  Function	:	FileClose
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogUtilities::LogFile::FileClose()
{
	// Close the file if there's one
	m_sFile.close();

	// Unregister the stream
	if( m_FileID.isValid() )
	{
		Log Log;
		Log.RemoveStream( m_FileID );

		m_FileID = StreamID();
	}

	return true;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isValid
*	Description	:	Test the validity of the file.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
bool Ludos::LogUtilities::LogFile::isValid()
{
	if( m_sFile.is_open() )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// LogBlock :: IMPLEMENTATION /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	LogBlock (constructor)
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::LogUtilities::LogBlock::LogBlock( const std::string &szBlockName,
										 const Log& AssociatedLog )
: m_bValid( true ), m_AssociatedLog( AssociatedLog )
{
	// Start the block
	m_AssociatedLog.BlockStart( szBlockName );
}

//----------------------------------------------------------------------------------------
/*  Function	:	~LogBlock
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
Ludos::LogUtilities::LogBlock::~LogBlock()
{
	// End the block
	Kill();
}

//----------------------------------------------------------------------------------------
/*  Function	:	Kill
*	Description	:	Kills the block
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	10/18/2008
*/	
//----------------------------------------------------------------------------------------
void Ludos::LogUtilities::LogBlock::Kill()
{
	if( m_bValid )
	{
		m_AssociatedLog.BlockEnd();
		m_bValid = false;
	}
}