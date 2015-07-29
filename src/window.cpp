#include "window.h"
#include "io.h"
#include "camera.h"
#include <memory>
#include <qcolordialog.h>

Window::Window(int argc, char** argv) {}

bool Window::initialise()
{
	ui.setupUi(this);

	connect(ui.ButtonAdd, SIGNAL(pressed()), this, SLOT(buttonAddEvent()));
	connect(ui.ButtonRemove, SIGNAL(pressed()), this, SLOT(buttonRemoveEvent()));
	connect(ui.ButtonUp, SIGNAL(pressed()), this, SLOT(buttonUpEvent()));
	connect(ui.ButtonDown, SIGNAL(pressed()), this, SLOT(buttonDownEvent()));

	connect(ui.BoxIso, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIsoEvent(int)));
	connect(ui.BoxAperture, SIGNAL(currentIndexChanged(int)), this, SLOT(changeApertureEvent(int)));
	connect(ui.BoxShutter, SIGNAL(currentIndexChanged(int)), this, SLOT(changeShutterEvent(int)));

	connect(ui.ButtonGo, SIGNAL(pressed()), this, SLOT(shootEvent()));

	ui.BoxProcessedformat->addItems({ "tiff", "bmp", "jpg", "png" });

	mColourModel = std::unique_ptr<QStringListModel>(new QStringListModel(this));
	ui.ListColours->setModel(mColourModel.get());
	ui.ListColours->setEditTriggers(QAbstractItemView::NoEditTriggers);
	if (!initialiseCamera())
		return false;

	return true;
}

bool Window::initialiseCamera()
{
	//Initialise camera system
	Inform("Initialising camera");
	mCameraList = std::unique_ptr<CameraList> (CameraList::create(true));

	if (!mCameraList.get())
		return false;

	if (mCameraList->ennumerate() == 0)
	{
		Error("No cameras found");
		return false;
	}

	Camera& mainCamera = mCameraList->cameras[0];
	if (!mainCamera.select())
	{
		Error("Could not select the main camera.");
		return false;
	}

	//Set selectable values
	std::vector<int> apList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::Aperture);
	std::vector<int> isList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::ISO);
	std::vector<int> shList = mainCamera.ennumeratePossibleValues(Camera::EnnumerableProperties::ShutterSpeed);

	int currentAp = mainCamera.aperture();
	int currentIs = mainCamera.iso();
	int currentSh = mainCamera.shutterSpeed();

	for (size_t i = 0; i < apList.size(); ++i)
	{
		ui.BoxAperture->addItem(Camera::apertureMappings[apList[i]].c_str());
		if (apList[i] == currentAp)
			ui.BoxAperture->setCurrentIndex(i);
	}

	for (size_t i = 0; i < isList.size(); ++i)
	{
		ui.BoxIso->addItem(Camera::isoMappings[isList[i]].c_str());
		if (isList[i] == currentIs)
			ui.BoxIso->setCurrentIndex(i);
	}

	for (size_t i = 0; i < shList.size(); ++i)
	{
		ui.BoxShutter->addItem(Camera::shutterSpeedMappings[shList[i]].c_str());
		if (shList[i] == currentSh)
			ui.BoxShutter->setCurrentIndex(i);
	}

	return true;
}

Window* Window::create(int argc, char** argv)
{
	Window* out = new Window(argc, argv);
	if (!out)
		return nullptr;
	if (!out->initialise())
	{
		delete out;
		return nullptr;
	}
	else
	{
		return out;
	}
}

Window::~Window(){}

void Window::buttonAddEvent()
{
	QColor colour = QColorDialog::getColor(Qt::white);
	if (!colour.isValid())
		return;

	mColourModel->setStringList(mColourModel->stringList() <<colour.name());
}

void Window::buttonRemoveEvent()
{
	auto removeIndex = ui.ListColours->currentIndex();
	if (removeIndex.isValid())
		mColourModel->removeRow(removeIndex.row());
}
void Window::buttonUpEvent()
{
	auto currentIndex = ui.ListColours->currentIndex();
	if (!currentIndex.isValid())
		return;

	if (currentIndex.row() > 0)
	{
		auto list = mColourModel->stringList();
		list.move(currentIndex.row(), currentIndex.row() - 1);
		mColourModel->setStringList(list);
		ui.ListColours->setCurrentIndex(mColourModel->index(currentIndex.row() - 1));
	}
}

void Window::buttonDownEvent()
{
	auto currentIndex = ui.ListColours->currentIndex();
	if (!currentIndex.isValid())
		return;

	auto list = mColourModel->stringList();
	if (currentIndex.row() >= list.size()-1)
		return;

	list.move(currentIndex.row(), currentIndex.row() + 1);
	mColourModel->setStringList(list);
	
	ui.ListColours->setCurrentIndex(mColourModel->index(currentIndex.row() + 1));
}

void Window::changeIsoEvent(int value)
{
	Camera* camera = CameraList::instance()->activeCamera();
	if (camera == nullptr)
	{
		Error("Invalid camera pointer at " + ToString(__LINE__));
		return;
	}

	camera->iso(Camera::isoMappings[std::string(ui.BoxIso->itemText(value).toUtf8())]);
}

void Window::changeApertureEvent(int value)
{
	Camera* camera = CameraList::instance()->activeCamera();
	if (camera == nullptr)
	{
		Error("Invalid camera pointer at " + ToString(__LINE__));
		return;
	}

	camera->aperture(Camera::apertureMappings[std::string(ui.BoxAperture->itemText(value).toUtf8())]);
}

void Window::changeShutterEvent(int value)
{
	Camera* camera = CameraList::instance()->activeCamera();
	if (camera == nullptr)
	{
		Error("Invalid camera pointer at " + ToString(__LINE__));
		return;
	}

	camera->shutterSpeed(Camera::shutterSpeedMappings[std::string(ui.BoxShutter->itemText(value).toUtf8())]);
}


void Window::shootEvent()
{
	bool saveProcessed = ui.CheckSaveProcessed->isChecked();
	bool saveRaw = ui.CheckSaveRaw->isChecked();

	std::string processedExtension = ui.BoxProcessedformat->itemText(ui.BoxProcessedformat->currentIndex()).toUtf8();

	int delay = ui.SpinDelay->value();

	Inform(std::string("Shooting with saveProcessed=") + (saveProcessed ? "TRUE" : "FALSE") +
		" saveRaw=" + (saveRaw ? "TRUE" : "FALSE") + " delay=" + ToString(delay) +
		" extension=" + processedExtension);
}
