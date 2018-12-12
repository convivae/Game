#pragma once

extern class GameController *gameController; // 指向游戏控制器的指针
extern class GameWindow *gameWindow; // 指向主窗口的指针
extern class ChooseWidget *chooseWidget; // 指向选择界面的指针
extern class MapWidget *mapWidget; // 指向地图界面的指针
extern class PhysicsEngine *physicsEngine; // 指向物理引擎的指针
extern class GameDisplayer *gameDisplayer; // 指向绘图模块的指针

extern int playersNum, myID;
