#pragma once
#include <QTreeView>

#include "sprite.h"

class QStandardItem;
class QStandardItemModel;

class Hierarchy : public QTreeView {
	Q_OBJECT
public:
	Hierarchy(QWidget* parent = Q_NULLPTR);
	~Hierarchy();

public:
	void update(Sprite root);

	Sprite selectedSprite();
	bool selectedSprites(QList<Sprite>& sprites);

private:
	void updateRecursively(Sprite pp, QStandardItem* pi);

private:
	QStandardItemModel* model_;
};
