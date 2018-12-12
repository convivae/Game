#pragma once

#include "WorkerThread.h"
#include "Actor.h"

class MapWidget;
class DataReceiver;
class MapView;

class GameDisplayer : public WorkerThread
{
	Q_OBJECT

public:
	GameDisplayer(DataReceiver *receiver);
	~GameDisplayer();

	S2M_Transformation getScreenToMapTrans();

protected:
	void mainloop() override;

signals:
	void finishedPainting();
	void actorDied();

private:
	void setScreenToMapTrans(const S2M_Transformation &t);

public:
	QPixmap imageToShow;
	QMutex imageMutex;
	bool pauseGame;

private:
	bool bitmapPainted, actorCreated;
	double lastWidth, lastHeight;
	DataReceiver *dataReceiver;
	QList<SpriteView*> sprites;
	MapView *mapView;
	S2M_Transformation s2mt;
	QMutex s2mtMutex;
};
