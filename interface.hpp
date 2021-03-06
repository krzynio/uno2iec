#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <qextserialport.h>
#include <QStringList>

#include "filedriverbase.hpp"
#include "d64driver.hpp"
#include "t64driver.hpp"
#include "m2idriver.hpp"
#include "nativefs.hpp"

#define READPRG_CHANNEL 0
#define WRITEPRG_CHANNEL 1
#define CMD_CHANNEL 15

typedef QList<FileDriverBase*> FileDriverList;

enum OpenState {
	O_NOTHING,			// Nothing to send / File not found error
	O_INFO,					// User issued a reload sd card
	O_FILE,					// A program file is opened
	O_DIR,					// A listing is requested
	O_FILE_ERR,			// Incorrect file format opened
	O_SAVE_REPLACE	// Save-with-replace is requested
};


class Interface : public ISendLine
{
public:
	// Callback Interface for notification when Arduino triggers various operations (upon CBM requests).
	// This is typically just for UI reflection, updating progress bars, mounted file names, directory lists etc.
	struct IFileOpsNotify
	{
		virtual void directoryChanged(const QString& newPath) = 0;
		virtual void imageMounted(const QString& imagePath, FileDriverBase* pFileSystem) = 0;
		virtual void imageUnmounted() = 0;
		virtual void fileLoading(const QString& fileName, ushort fileSize) = 0;
		virtual void fileSaving(const QString& fileName) = 0;
		virtual void bytesRead(uint numBytes) = 0;
		virtual void bytesWritten(uint numBytes) = 0;
		virtual void fileClosed(const QString& lastFileName) = 0;
		virtual void deviceReset() = 0;
	};


	Interface(QextSerialPort& port);
	void processOpenCommand(const QString &cmd, bool localImageSelectionMode = false);
	void processReadFileRequest();
	void processWriteFileRequest(uchar theByte);
	void reset();

	// State specific: CBM requests a single directory line from us.
	void processLineRequest();
	void buildDirectoryOrMediaList();

	// ISendLine implementation.
	void send(short lineNo, const QString& text);

	void processGetOpenFileSize();
	void processCloseCommand();
	void processErrorStringRequest(IOErrorMessage code);
	bool changeNativeFSDirectory(const QString &newDir);
	void setMountNotifyListener(IFileOpsNotify *pListener);
	void setImageFilters(const QString &filters, bool showDirs);

private:
	void openFile(const QString &cmdString, bool localImageSelection = false);
	bool removeFilePrefix(QString &cmd);

	D64 m_d64;
	T64 m_t64;
	M2I m_m2i;
	NativeFS m_native; // In fact, this is .PRG

	FileDriverList m_fsList;
	FileDriverBase* m_currFileDriver;
	QextSerialPort& m_port;
	IOErrorMessage m_queuedError;
	OpenState m_openState;
	QString m_lastCmdString;
	QList<QByteArray> m_dirListing;
	IFileOpsNotify* m_pListener;
};

#endif // INTERFACE_HPP
