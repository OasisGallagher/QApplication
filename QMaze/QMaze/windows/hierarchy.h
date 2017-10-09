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
	void update(const std::vector<Sprite>& sprites);

private:
	void updateRecursively(Sprite pp, QStandardItem* pi);

private:
	QSet<unsigned> updated_;
	QStandardItemModel* model_;
};
