#include "stdafx.h"
#include "WorkerThread.h"

WorkerThread::WorkerThread()
	: QObject(nullptr), running(false), toQuit(false)
{
	timer.setTimerType(Qt::PreciseTimer);
	timer.setSingleShot(true);
	timer.moveToThread(&workThread);
	connect(&timer, &QTimer::timeout, this, &WorkerThread::goToWork);
	connect(this, &WorkerThread::started, this, &WorkerThread::goToWork);
	connect(this, &WorkerThread::started, [this]() { lastTime = clock(); });
	connect(this, &WorkerThread::stopped, &timer, &QTimer::stop);

	moveToThread(&workThread);
	workThread.start();
}

WorkerThread::~WorkerThread()
{
	workThread.quit();
	workThread.wait();
}

void WorkerThread::waitAndQuit()
{
	emit stopped();
	toQuit = true;
	while (running)
		_sleep(1);
}

void WorkerThread::goToWork()
{
	running = true;
	while (!toQuit)
	{
		currentTime = clock();
		timePassed = difftime(currentTime, lastTime) * 0.001;
		lastTime = currentTime;
		mainloop();
		double time_cost = difftime(clock(), currentTime);
		if (time_cost >= 10)
		{
			qDebug() << QString(u8"%1此次循环用时%2毫秒").arg(name).arg(time_cost);
		}
		if (time_cost < 1000.0 / defaultFPS)
		{
			timer.start(1000.0 / defaultFPS - time_cost);
			break;
		}
	}
	running = false;
}
