#ifndef __LOG_MGR_H_
#define __LOG_MGR_H_

// ��־��ض���
#define MAXLOGFILESIZE (1024 << 10)		// ��־�ļ���󳤶�
#define _LOG_DATABUFF			1024	// ��־���ݻ�������С

// ��־����
#define	__DEBUG		_T("DEBUG")
#define	__ERROR		_T("ERROR")
#define	__INFO		_T("INFO")
#define	__CRITICAL	_T("CRITICAL")

// ��־ת���ļ���
#define	LOG_CONVEY_FILE_NAME	"LogBak"
#define	CONVEY_RULE				"LogBak.*"
#define	MAX_SAVE_NUM			5

//
// ��־����
//
class CLogMgr
{
public:
	CLogMgr();
	~CLogMgr();
	
	static CLogMgr* Instance();
	static void Release();
	
	void SetLogFile(CString strLogFile) { m_strLogFile = strLogFile; m_bLogEnable = TRUE; }
	CString GetLogFile() { return m_strLogFile; }
	void SetLogLevel(int nLogLevel) { m_nLogLevel = nLogLevel; }
	int GetLogLevel() { return m_nLogLevel; }
	BOOL IsLogEnable() { return m_bLogEnable; }
	CRITICAL_SECTION* GetLogMutex() { return &m_WriteLogMutex; }
	

	static void LogEvent(int nLevel, LPCTSTR lpFormat, ...);
	int LogEventArgs(int nLevel, LPCTSTR lpFormat, va_list argp);

private:
	CString					m_strLogFile;			// ��־�ļ���
	int						m_nLogLevel;			// ��־����
	BOOL					m_bLogEnable;			// �Ƿ�������־����
	int						m_nMaxLogFileSize;		// ��־�ļ����ȵ����ֵ
	CRITICAL_SECTION		m_WriteLogMutex;		// ��־ͬ����
};

#endif	// __LOG_MGR_H_