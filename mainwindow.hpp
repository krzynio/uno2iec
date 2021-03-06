#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileInfo>
#include <qextserialport.h>
#include <QMap>
#include "interface.hpp"
#include "logger.hpp"
#include "settingsdialog.hpp"

namespace Ui {
class MainWindow;
}

typedef QMap<QChar, QString> FacilityMap;

class MainWindow : public QMainWindow, public Logging::ILogTransport, public Interface::IFileOpsNotify,
		public ISendLine
{
	Q_OBJECT

	typedef QMap<QString, QFileInfo> QFileInfoMap;
public:
	struct CbmMachineTheme {
		QString bootText;
		QString font;
		uchar fgColorIndex;
		uchar bgColorIndex;
		uchar cursorWidth;
	};

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void processAddNewFacility(const QString &str);
	void checkVersion();
	void closeEvent(QCloseEvent *event);

	// IMountNotifyListener interface implementation
	void directoryChanged(const QString& newPath);
	void imageMounted(const QString& imagePath, FileDriverBase* pFileSystem);
	void imageUnmounted();
	void fileLoading(const QString &fileName, ushort fileSize);
	void fileSaving(const QString& fileName);
	void bytesRead(uint numBytes);
	void bytesWritten(uint numBytes);
	void fileClosed(const QString &lastFileName);
	void deviceReset();

	// ISendLine interface implementation.
	void send(short lineNo, const QString &text);

	// ILogTransport implementation.
	void appendTime(const QString& dateTime);
	void appendLevelAndFacility(Logging::LogLevelE level, const QString& levelFacility);
	void appendMessage(const QString& msg);


private slots:
		void onDirListColorSelected(QAction *pAction);
		void onCbmMachineSelected(QAction *pAction);
		void onDataAvailable();
		void on_clearLog_clicked();
		void on_pauseLog_toggled(bool checked);
		void on_saveLog_clicked();
		void on_saveHtml_clicked();
		void on_resetArduino_clicked();
		void on_browseImageDir_clicked();
		void on_imageDir_editingFinished();
		void on_imageFilter_textChanged(const QString &filter);
		void on_mountSelected_clicked();
		void on_browseSingle_clicked();
		void on_mountSingle_clicked();
		void on_actionAbout_triggered();
		void on_unmountCurrent_clicked();
		void on_actionSettings_triggered();
		void on_reloadImageDir_clicked();
		void on_dirList_doubleClicked(const QModelIndex &index);

private:
	void enumerateComPorts();
	void usePortByFriendlyName(const QString &friendlyName);
	void processDebug(const QString &str);
	void updateImageList(bool reloadDirectory = true);
	void boldifyItem(QStandardItem *pItem);
	void readSettings();
	void writeSettings() const;

	void setupActionGroups();
	void selectActionByName(const QList<QAction *>& actions, const QString& name) const;
	void updateDirListColors();
	void getBgAndFgColors(QColor &bgColor, QColor &fgColor);
	void getMachineAndPaletteTheme(CbmMachineTheme *&pMachine, const QRgb *&pEmulatorPalette);

	Ui::MainWindow *ui;
	QextSerialPort m_port;
	QByteArray m_pendingBuffer;
	bool m_isConnected;
	FacilityMap m_clientFacilities;
	Interface m_iface;
	QextPortInfoList m_ports;
	QStandardItemModel* m_dirListItemModel;
	QFileInfoList m_filteredInfoList;
	QFileInfoList m_infoList;
	bool m_isInitialized;
	QStringList m_imageDirListing;
	AppSettings m_appSettings;
};

#endif // MAINWINDOW_HPP
