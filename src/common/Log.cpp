#include "Log.h"


//use magic statics
//CLogPtrVector g_arLogs;

CLog::CLog(string sName)
	: m_fp(nullptr)
	, m_sName( sName )
{
}


CLog::~CLog(void)
{
	Close();
}

void CLog::Close()
{
	if ( m_fp != nullptr )
	{
		fclose(m_fp);
		m_fp = nullptr;
	}
}

void CLog::Start( const string& sLogfileName )
{
	m_fp = fopen( sLogfileName.c_str(), "w+" );
}

void CLog::AddLine( const string& sLine )
{
	if ( m_fp == nullptr ) return;

	fprintf( m_fp, "%s\n", sLine.c_str() );
	fflush( m_fp );
}

void CLog::AddLine( const char* pcszFormat, ... )
{
   if ( m_fp == nullptr ) return;

   va_list args;
   va_start(args, pcszFormat);
   vfprintf( m_fp, pcszFormat, args );
   va_end (args);
   fflush( m_fp );
}
