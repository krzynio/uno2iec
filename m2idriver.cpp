//
// Title        : M2IDRIVER
// Author       : Lars Pontoppidan
// Date         : May. 2007
// Version      : 0.1
// Target MCU   : AtMega32(L) at 8 MHz
//
//
// DESCRIPTION:
// This module implements support for the file format with extension .m2i
// With an .m2i file, it is possible to mimic an unlimited size diskette with
// read/write support and realistic filenames, while actually using FAT files.
//
// .m2i format:
//
// <DISK TITLE (16 chars)><cr><lf>
// <file type 1 char>:<dos filename 12chars>:<cbm filename 16 chars><cr><lf>
// <file type 1 char>:<dos filename 12chars>:<cbm filename 16 chars><cr><lf>
// <file type 1 char>:<dos filename 12chars>:<cbm filename 16 chars><cr><lf>
//
// file types: P means prg file
//             D means del file
//             - means deleted file
//
// DISCLAIMER:
// The author is in no way responsible for any problems or damage caused by
// using this code. Use at your own risk.
//
// LICENSE:
// This code is distributed under the GNU Public License
// which can be found at http://www.gnu.org/licenses/gpl.txt
//

#include <string.h>
#include "m2idriver.hpp"


// Local vars, driver state:

namespace {
const QString strError1("M2I FILE ERROR");
const QString strPRG("PRG");
const QString strDEL("DEL");
const QString strID(" M2I");
const QString strDotPRG(".PRG\0");
const QString strDotM2I(".M2I\0");
}


// method below performs init of the driver with the given ATN command string.
bool M2I::openHostFile(const QString& fileName)
{
	closeHostFile();

	// Interface has just opened the m2i file, save filename
	m_hostFile.setFileName(fileName);

	if(!m_hostFile.open(QIODevice::ReadOnly))
		return false;

	// We accept m2i file if first line is OK:
	return readFirstLine();
} // openHostFile


void M2I::closeHostFile()
{
	if(!m_hostFile.fileName().isEmpty() and m_hostFile.isOpen())
		m_hostFile.close();
	m_status = NOT_READY;
} // closeHostFile


bool M2I::sendListing(ISendLine &cb)
{
	m_status = NOT_READY;
	 // disk title
	QString parsed;
	// Reseek from beginning of M2I.
	if(!m_hostFile.seek(0) or !readFirstLine(&parsed)) {
		cb.send(0, strError1);
		return false;
	}

	// First line is disk title
	QString line = QString("\x12\x22%1\x22%2").arg(parsed, strID);
	cb.send(0, line);

	while(!m_hostFile.atEnd()) {
		// Write lines
		uchar ftype(parseLine(0, &parsed));

		if('P' == ftype or 'D' == ftype) {
			line = QString("  \x22%1\x22 %2").arg(parsed, 'p' == ftype ? strDotPRG : strDEL);
			cb.send(0, line);
		}
	}
	return true;
} // sendListing


// Seek through M2I index. dosname and dirname must be pointers to
// 12 and 16 byte buffers. Dosname can be NULL.
//
bool M2I::seekFile(QString* dosName, QString& dirName, const QString& seekName,
									 bool doOpen)
{
	Q_UNUSED(dosName);
	Q_UNUSED(dirName);
	Q_UNUSED(seekName);
	Q_UNUSED(doOpen);
	bool found = false;
/*
	uchar i;
	uchar seeklen, dirlen;

	if(doOpen) {
		// Open the .m2i file
		if (!fatFopen(m_hostFile))
			return false;
	}
	else {
		// Just ensure we are at beginning
		fatFseek(0, SEEK_SET);
	}

	if(!readFirstLine(0))
		return false;

	// Find length of seek name, disregard ending spaces
	seeklen = strnlen(seekname, 16);
	for(; (seeklen > 0) and (seekname[seeklen-1] == ' '); seeklen--);

	while(!found and !fatFeof()) {
		i = parseLine(dosname, dirname);

		if('P' == i) {
			// determine length of dir filename
			for(dirlen = 16; (dirlen > 0) and (dirname[dirlen - 1] == ' '); dirlen--);

			// Compare filename respecting * and ? wildcards
			found = true;
			for(i = 0; i < seeklen and found; i++) {
				if(seekname[i] == '?') {
					// This character is ignored
				}
				else if(seekname[i] == '*') {
					// No need to check more chars
					break;
				}
				else
					found = seekname[i] == dirname[i];
			}

			// If searched to end of filename, dir filename must end here also
			if(found and (i == seeklen))
				found = (seeklen == dirlen);
		}
	}

	// Close m2i file
	if(doOpen)
		closeHostFile();
*/
	return found;
} // seekFile


bool M2I::remove(char* fileName)
{
	Q_UNUSED(fileName);
	bool ret = false;
/*
	char dosname[12];
	char dirname[16];
	uchar found;

	m_status and_eq compl FILE_OPEN;

	// Open m2i index
	if(!fatFopen(m2i_filename))
		return false;

	do {
		// Seek for filename
		found = seekFile(dosname, dirname, filename, false);

		if (found) {
			ret = true;
			// File found, delete entry in m2i, seek back length of one entry
			fatFseek((ulong)(-33), SEEK_CUR);
			// write a -
			fatFputc('-');
			// Close m2i
			fatFclose();
			// Delete fat file
			fatRemove(dosname);
			// Open m2i again
			fatFopen(m2i_filename);
		}
	} while(found);

	// Finally, close m2i
	fatFclose();
*/
	return ret;
} // remove


bool M2I::rename(char* oldName, char* newName)
{
	Q_UNUSED(oldName);
	Q_UNUSED(newName);
	bool ret = false;
	/*
	char dirName[16];
	uchar j;

	m_status and_eq compl FILE_OPEN;

	// Open m2i index
	if(!fatFopen(m2i_filename))
		return false;

	// Look for oldname
	if(seekFile(NULL, dirName, oldName, false)) {

		// File found, rename entry in m2i
		// Seek back until name
		fatFseek((ulong)(-33 + 15), SEEK_CUR);

		// write new name, space padded
		j = newName[0];
		for(uchar i = 0; i < 16;i++) {
			if (j) {
				fatFputc(j);
				j = newName[i + 1];
			}
			else
				fatFputc(' ');
		}
		ret = true;
	}

	// Close m2i
	fatFclose();
*/
	return ret;
} // rename


uchar M2I::newDisk(char* diskName)
{
	Q_UNUSED(diskName);
	/*
	char dosName[13];
	uchar i,j;

	m_status and_eq compl FILE_OPEN;

	// Find new .m2i filename
	// Take starting 8 chars padded with space ending with .m2i
	strncpy(dosName, diskName, 8);
	i = strnlen(dosName, 8);
	if (i < 8)
		memset(dosName + i, ' ', 8 - i);
	memcpy(dosName + 8, pstr_dot_m2i, 5);

	// Is this filename free
	if(fatFopen(dosName)) {
		fatFclose();
		return ErrFileExists;
	}

	// It is free, create new file
	if(!fatFcreate(dosName))
		return ErrWriteProtectOn;

	// Write m2i header with the diskname
	j = diskName[0];
	for(i = 0; i < 16; i++) {
		if(j) {
			fatFputc(j);
			j = diskName[i + 1];
		}
		else
			fatFputc(' ');
	}

	// Write newline
	fatFputc(13);
	fatFputc(10);
	fatFclose();

	// Now this is the open m2i
	memcpy(m2i_filename, dosName, 13);
*/
	return ErrOK;
} // newDisk


// Respond to command channel command
//
uchar M2I::cmd(char* cmd)
{
	Q_UNUSED(cmd);
	uchar ret = ErrOK;
	/*
	// Get command letter and argument
	char cmdLetter;
	char *arg, *p;

	cmdLetter = cmd[0];

	arg = strchr(cmd,':');

	if(0 not_eq arg) {
		arg++;

		if(cmdLetter == 'S') {
			// Scratch file(s)
			if(!remove(arg))
				ret = ErrFileNotFound;
		}
		else if(cmdLetter == 'R') {
			// Rename, find =  and replace with 0 to get cstr
			p = strchr(arg, '=');

			if(0 not_eq p) {
				*p = 0;
				p++;
				if(!rename(p, arg))
					ret = ErrFileNotFound;
			}

		}
		else if(cmdLetter == 'N')
			ret = newDisk(arg);
		else
			ret = ErrSyntaxError;
	}
*/
	return ret;
} // cmd


bool M2I::fopen(const QString& fileName)
{
	Q_UNUSED(fileName);
/*
	char dirname[16];
	char dosname[12];

	m_status = NOT_READY;

	// Look for filename
	if(seekFile(dosname, dirname, filename, true)) {
		// Open the dos name corresponding!
		if(fatFopen(dosname))
			m_status or_eq FILE_OPEN;

	}
	*/
	return(m_status bitand FILE_OPEN);
} // open


// The new function ensures filename exists and is empty.
//
uchar M2I::newFile(char* fileName)
{
	Q_UNUSED(fileName);
	/*
	char dirName[16];
	char dosName[12];

	m_status = NOT_READY;

	// Look for filename
	if(seekFile(dosName, dirName, fileName, true)) {
		// File exists, delete it
		fatRemove(dosName);
		// And create empty
		fatFcreate(dosName);
		m_status or_eq FILE_OPEN;

	}
	else {
		// File does not exist, create it
		createFile(fileName);
	}
*/
	return(m_status bitand FILE_OPEN);
} // newFile


QString M2I::openedFileName() const
{
	// TODO: implement!
	return QString();
} // openedFileName


ushort M2I::openedFileSize() const
{
	// TODO: implement!
	return 0;
} // openedFileSize


char M2I::getc(void)
{
	/*
	if(m_status bitand FILE_OPEN)
		return fatFgetc();
*/
	return 0;
} // getc


bool M2I::isEOF(void) const
{
	/*
	if(m2i_status bitand FILE_OPEN)
		return fatFeof();
*/
	return true;
} // isEOF


// write char to open file, returns false if failure
bool M2I::putc(char c)
{
	Q_UNUSED(c);
	/*
	if(m_status bitand FILE_OPEN)
		return fatFputc(c);
		*/
	return false;
} // putc


// close file
bool M2I::close(void)
{
	m_status and_eq compl FILE_OPEN;
	closeHostFile();

	return true;
} // close


// Get first line of m2i file, writes 16 chars in dest
// Returns FALSE if there is a problem in the file
bool M2I::readFirstLine(QString* dest)
{
	bool res = false;
	char data[18];
	if(sizeof(data) == m_hostFile.read(data, sizeof(data))) {
		QByteArray qdata(data, sizeof(data));
		if(0 not_eq dest)
			*dest = qdata.left(sizeof(data) - 2);
		res = qdata.endsWith("\r");
	}

	return res;
} // readFirstLine


// Parses one line from the .m2i file.
// File pos must be after reading the initial status char
//
// <file type char>:<dosname>:<cbmname><cr><lf>
//
// at dosname 12 chars is written, at cbmname 16 chars
//
uchar M2I::parseLine(QString* dosName, QString* cbmName)
{
	if(m_hostFile.atEnd())
		return 0;  // no more records
	// ftype + fsName + cbmName + separators + LF
	QList<QByteArray> qData = m_hostFile.read(1 + 256 + 16 + 3 + 1).split(':');
	// must be three splits.
	if(qData.count() < 3)
		return 0;

	// first one ONLY the fs type.
	if(qData.at(0).count() not_eq 1)
		return 0;
	uchar fsType(qData.at(0).at(0));

	if(qData.at(1).isEmpty())
		return 0;
	if(0 not_eq dosName)
		*dosName = qData.at(1);

	// must be ending with a CR, the cbm string must be 16 chars long.
	if(!qData.at(2).endsWith("\r") or qData.at(2).length() not_eq 17)
		return 0;
	QString cbmStr(qData.at(2));
	cbmStr.chop(1);
	if(0 not_eq cbmName)
		*cbmName = cbmStr;

	return fsType;
} // parseLine


bool M2I::createFile(char* fileName)
{
	Q_UNUSED(fileName);
	// TODO: Implement
	m_status = NOT_READY;
/*
	// Find dos filename
	char dosName[13];
	uchar i,j;

	// Initial guess is starting 8 chars padded with space ending with .prg
	strncpy(dosName, fileName, 8);
	i = strnlen(dosName, 8);
	if(i < 8)
		memset(dosName + i, ' ', 8 - i);
	memcpy(dosName + 8, pstr_dot_prg, 5);

	// Is this filename free?
	i = 0;
	while(fatFopen(dosName)) {
		if(i > 99)
			return false;

		// No, modify it
		j = i % 10;
		dosName[6] = i - j + '0';
		dosName[7] = j + '0';

		i++;
	}

	// Now we have a suitable dos filename, create entry in m2i index
	if(!fatFopen(m2i_filename))
		return false;

	if(!read_first_line(NULL))
		return false;

	fatFseek(0,SEEK_END);

	fatFputc('P');
	fatFputc(':');

	for(i = 0; i < 12; i++)
		fatFputc(dosName[i]);

	fatFputc(':');

	j = fileName[0];
	for(i = 0; i < 16; i++) {
		if(j) {
			fatFputc(j);
			j = fileName[i + 1];
		}
		else
			fatFputc(' ');
	}

	fatFputc(13);
	fatFputc(10);
	fatFclose();

	// Finally, create the target file
	fatFcreate(dosName);

	m_status or_eq FILE_OPEN;
*/
	return true;
} // createFile


