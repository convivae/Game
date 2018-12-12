#pragma once

class GameController
{
public:
	GameController();

	void stop();

private:
	QStateMachine stateMachine;
	QState mainMenuState;
	QState singlePlayerState;
	QState createOrJoinState;
	QState waitingForJoinState;
	QState inputIPState;
	QState connectIPState;
	QState waitingForStartState;
	QState multiplePlayerState;
	QState pauseGameState;
	QState gameOverState;
};
