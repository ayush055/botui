#include "EditorWidget.h"
#include "ui_EditorWidget.h"

#include "MenuBar.h"

#include "RootController.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "AreYouSureDialog.h"
#include "SystemPrefix.h"

#include <QApplication>
#include <QDebug>


EditorWidget::EditorWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	m_device(device),
	ui(new Ui::EditorWidget),
	m_menuBar(new MenuBar(this)),
	m_currentIndex(0)
{
	ui->setupUi(this);
	layout()->setMenuBar(m_menuBar);
	QAction *saveAndExit = m_menuBar->addAction(tr("Save"));
	connect(saveAndExit, SIGNAL(triggered()), SLOT(saveAndExit()));
	
	QAction *addFile = m_menuBar->addAction(tr("Add File"));
	connect(addFile, SIGNAL(triggered()), SLOT(addFile()));
	
	QAction *removeFile = m_menuBar->addAction(tr("Delete File"));
	connect(removeFile, SIGNAL(triggered()), SLOT(removeFile()));
	
	connect(ui->files, SIGNAL(currentIndexChanged(int)), SLOT(fileChanged(int)));
}

EditorWidget::~EditorWidget()
{
	delete ui;
}


void EditorWidget::setProjectPath(const QString &projectPath)
{
	m_projectPath = projectPath;

	m_lookup.clear();
	ui->files->clear();
	m_currentIndex = -1;

	const QDir projectDir(projectPath);
	const QDir includeDir(projectPath + "/include/");
	const QDir srcDir(projectPath + "/src/");
	const QDir dataDir(projectPath + "/data/");


	//foreach(const QString &file, m_archive->files()) m_lookup << file;

	qDebug() << " checking for source files at " << srcDir.absolutePath();
	foreach(const QString &file, srcDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files)) m_lookup << "/src/" + file;

	qDebug() << " checking for include files at " << includeDir.absolutePath();
	foreach(const QString &file, includeDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files)) m_lookup << "/include/" + file;

	qDebug() << " checking for data files at " << dataDir.absolutePath();
	foreach(const QString &file, dataDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files)) m_lookup << "/data/" + file;

	ui->files->addItems(m_lookup);
	if(m_lookup.isEmpty()) {
		fileChanged(-1);
		return;
	}else{
		qDebug() << " m_lookup[0] " << m_lookup[0];
	}

	// clear text window
    ui->text->clear();

    // open file
    QString filepath = projectPath + m_lookup[0];
    QFile file(filepath);
    if(!file.exists()){
        qDebug() << "Missing file: "<< filepath;
    }

    // load file contents
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        QString contents = stream.readAll();

        //display contents
        ui->text->setPlainText(contents);
    }

    // close file
    file.close();
}

const QString &EditorWidget::projectPath() const
{
	return m_projectPath;
}

void EditorWidget::fileChanged(int i)
{
	/*
	ui->text->setEnabled(i >= 0);
	if(i < 0) {
		ui->text->setPlainText(tr("No files to edit."));
		m_currentIndex = -1;
	}
	if(i >= m_lookup.size() || i < 0) return;
	
	if(m_currentIndex >= 0) m_archive->setFile(m_lookup[m_currentIndex], ui->text->toPlainText().toUtf8());
	ui->text->setPlainText(m_archive->data(m_lookup[i]));
	
	m_currentIndex = i;
	*/
}

bool EditorWidget::removeDir(const QString &path) const
{
	bool success = true;
/*
	QDir directory(path);

	QFileInfoList files = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	foreach(const QFileInfo &file, files) success &= directory.remove(file.fileName());

	QFileInfoList dirs = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(const QFileInfo &dir, dirs) success &= removeDir(dir.absoluteFilePath());

	success &= directory.rmdir(directory.absolutePath());
*/
	return success;
}

void EditorWidget::saveAndExit()
{
	/*
	// TODO: Error checking?
	fileChanged(m_currentIndex); // Save current file
	m_archive->save(m_savePath);
	RootController::ref().dismissWidget();
  const QString name = QFileInfo(m_savePath).fileName();
  const QString binPath = SystemPrefix::ref().rootManager()->binPath(name);
  removeDir(binPath);
  */
}

void EditorWidget::addFile()
{
	KeyboardDialog dialog(tr("File Name"));
	if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	const QString filename = dialog.input();
	m_lookup << filename;
	ui->files->addItem(filename);
	ui->files->setCurrentIndex(m_lookup.size() - 1);
}

void EditorWidget::removeFile()
{
	if(m_currentIndex < 0) return;
	
	AreYouSureDialog dialog;
	int index = ui->files->currentIndex();
	dialog.setDescription(tr("You're about to permanently delete \"%1\". Continue?").arg(m_lookup[index]));
	if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	//FIXME: m_archive->removeFile(m_lookup[index]);
	m_lookup.removeAt(index);
	ui->files->removeItem(index);
	if(m_lookup.isEmpty()) return;
	index = ui->files->currentIndex();
	//FIXME: ui->text->setPlainText(m_archive->data(m_lookup[index]));
	m_currentIndex = index;
}
