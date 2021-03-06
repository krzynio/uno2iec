#ifndef FILEDRIVERBASE_HPP
#define FILEDRIVERBASE_HPP

#include <QString>
#include <QFile>

class ISendLine
{
public:
	virtual void send(short lineNo, const QString& text) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
// Error messages
//
// For detailed descriptions see: http://www.scribd.com/doc/40438/The-Commodore-1541-Disk-Drive-Users-Guide

typedef enum {
	ErrOK = 0,
	ErrFilesScratched,	// Files scratched response, not an error condition.
	ErrBlockHeaderNotFound = 20,
	ErrSyncCharNotFound,
	ErrDataBlockNotFound,
	ErrChecksumInData,
	ErrByteDecoding,
	ErrWriteVerify,
	ErrWriteProtectOn,
	ErrChecksumInHeader,
	ErrDataExtendsNextBlock,
	ErrDiskIdMismatch,
	ErrSyntaxError,
	ErrInvalidCommand,
	ErrLongLine,
	ErrInvalidFilename,
	ErrNoFileGiven,		// The file name was left out of a command or the DOS does not recognize it as such.
										// Typically, a colon(:) has been left out of the command
	ErrCommandNotFound = 39,	// This error may result if thecommand sent to command channel (secondary address 15) is unrecognizedby the DOS.
	ErrRecordNotPresent = 50,
	ErrOverflowInRecord,
	ErrFileTooLarge,
	ErrFileOpenForWrite = 60,
	ErrFileNotOpen,
	ErrFileNotFound,
	ErrFileExists,
	ErrFileTypeMismatch,
	ErrNoBlock,
	ErrIllegalTrackOrSector,
	ErrIllegalSystemTrackOrSector,
	ErrNoChannelAvailable = 70,
	ErrDirectoryError,
	ErrDiskFullOrDirectoryFull,
	ErrIntro,					// power up message or write attempt with DOS mismatch
	ErrDriveNotReady,	// typically in this emulation could also mean: not supported on this file system.
	ErrSerialComm,		// something went sideways with serial communication to the file server.
	ErrCount
} IOErrorMessage;


class FileDriverBase
{
public:
	enum FSStatus
	{
		NOT_READY = 0,  // driver not ready (host file not read or accepted).
		IMAGE_OK   = (1 << 0),  // The open file in fat driver is accepted as a valid image (of the specific file system type).
		FILE_OPEN = (1 << 1),  // A file is open right now
		FILE_EOF  = (1 << 2),  // The open file is ended
		DIR_OPEN  = (1 << 3),  // A directory entry is active
		DIR_EOF   = (1 << 4)  // Last directory entry has been retrieved
	};

	FileDriverBase();
	virtual ~FileDriverBase();

	// The three letter extension this file format represents (DOS style) Empty string returned means 'any' and is in default fs mode.
	virtual const QString& extension() const = 0;
	// method below performs init of the driver with the given ATN command string.
	virtual bool openHostFile(const QString& fileName) = 0;
	virtual void closeHostFile() = 0;

	// returns true if the file system supports directory listing (t64 for instance doesn't).
	virtual bool supportsListing() const;
	// Send realistic $ file basic listing, line by line (returning false means there was some error, but that there is a listing anyway).
	virtual bool sendListing(ISendLine& cb);
	// Whether this file system supports media info or not (true == supports it).
	virtual bool supportsMediaInfo() const;
	// Send information about file system (whether it is OK, sizes etc.).
	virtual bool sendMediaInfo(ISendLine &);
	// Command to the command channel. When not supported (overridden we just say write protect error).
	virtual IOErrorMessage cmdChannel(const QString& cmd);

	// Open a file in the image/file system by filename: Returns true if successful (false if not supported or error).
	virtual bool fopen(const QString& fileName);
	// return the name of the last opened file (may not be same as fopen in case it resulted in something else, like when using wildcards).
	virtual QString openedFileName() const = 0;
	// return the file size of the last opened file.
	virtual ushort openedFileSize() const = 0;
	// Create a file in the image/file system by filename: Returns true if successful (false if not supported or error).
	virtual bool newFile(const QString& fileName);
	// returns a character from the open file. Should always be supported in order to make implementation make any sense.
	virtual char getc() = 0;
	// returns true if end of file reached. Should always be supported in order to make implementation make any sense.
	virtual bool isEOF() const = 0;
	// returns a character to the open file. If not overridden, returns always true. If implemented returns false on failure.
	// write char to open file, returns false if failure
	virtual bool putc(char c);
	// closes the open file. Should always be supported in order to make implementation make any sense.
	virtual bool close() = 0;

	// Current status of operation.
	virtual FSStatus status() const;

	// This method is not relevant for any CBM side file systems (unless future one support such).
	// It sets the current directory on the native fs. Optionally implemented for a specific file system, base returns false.
	virtual bool setCurrentDirectory(const QString& dir);

protected:
	// Status of the driver:
	uchar m_status;

};

#endif // FILEDRIVERBASE_HPP
