#pragma once

class WorkerThread : public QObject
{
	Q_OBJECT

public:
	WorkerThread();
	virtual ~WorkerThread();

	bool isRunning() const { return running; }
	void waitAndQuit();

signals:
	void started();
	void stopped();

protected:
	virtual void mainloop() = 0;

private:
	void goToWork();

protected:
	QString name;
	double timePassed; // 记录两次循环的时间间隔
	double defaultFPS; // 设定帧率

private:
	QThread workThread;
	QTimer timer;
	time_t lastTime, currentTime;
	volatile bool running, toQuit;
};
