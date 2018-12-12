#include "stdafx.h"
#include "MapView.h"

#define NUM_OF_PICTURES 37

QVector<QImage> MapView::sourcePictures;
QVector<QPixmap> MapView::scaledPictures;

MapView::MapView(QString source_file_name)
{
	QFile file(source_file_name);
	file.open(QIODevice::ReadOnly);
	if (!file.isOpen())
		throw QString(u8"无法打开文件%1!").arg(source_file_name);
	QDataStream(&file) >> rows >> columns >> data;
}

void MapView::LoadResource()
{
	sourcePictures.resize(NUM_OF_PICTURES);
	scaledPictures.resize(NUM_OF_PICTURES);
	for (int i = 0; i < NUM_OF_PICTURES; i++)
	{
		QString source_file_name = QString("../images/map/%1.png").arg(i);
		sourcePictures[i].load(source_file_name);
		if (sourcePictures[i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}
}

void MapView::drawBody(QPainter &painter, const ScreenPoint &map_zero_on_screen, double block_size, double screen_width, double screen_height)
{
	static double last_block_size = 0;
	if (block_size != last_block_size) 
	{
		for (int i = 0; i < NUM_OF_PICTURES; i++) 
			scaledPictures[i] = QPixmap::fromImage(sourcePictures[i].scaledToWidth(ceil(block_size), Qt::SmoothTransformation));
		last_block_size = block_size;
	}

	int rows_block_num = screen_height / block_size;
	int cols_block_num = screen_width / block_size;

	int rows_start = map_zero_on_screen.y >= 0 ? 0 : -(int)(map_zero_on_screen.y / block_size);
	int cols_start = map_zero_on_screen.x >= 0 ? 0 : -(int)(map_zero_on_screen.x / block_size);

	int rows_end = rows_start + rows_block_num + 2 > rows ? rows : rows_start + rows_block_num + 2;
	int cols_end = cols_start + cols_block_num + 2 > columns ? columns : cols_start + cols_block_num + 2;

	for (int i = rows_start; i < rows_end; i++)
	{
		for (int j = cols_start; j < cols_end; j++)
		{
			if (data[i * columns + j] != 0xff)
			{
				int x = j * block_size + map_zero_on_screen.x, y = i * block_size + map_zero_on_screen.y;
				painter.drawPixmap(x, y, scaledPictures[data[i * columns + j]]);
			}
		}
	}
}
