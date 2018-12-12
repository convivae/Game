#pragma once

// 地图视图类，用于绘制地图
// 地图坐标系y轴向上，屏幕坐标系y轴向下，x轴都向右
class MapView
{
public:
	MapView(QString source_file_name);

	static void LoadResource();

	void drawBody(QPainter &painter, const ScreenPoint &map_zero_on_screen, double block_size, double screen_width, double screen_height);

	int getRows() const { return rows; }
	int getColumns() const { return columns; }

private:
	static QVector<QImage> sourcePictures;
	static QVector<QPixmap> scaledPictures;

	int rows, columns;
	QVector<quint8> data;	
};
