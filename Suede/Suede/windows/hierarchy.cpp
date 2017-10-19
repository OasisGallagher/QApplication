#include <QHeaderView>
#include <QStandardItemModel>

#include "sprite.h"
#include "engine.h"
#include "hierarchy.h"

Hierarchy::Hierarchy(QWidget* parent) {
	model_ = new QStandardItemModel(this);
	setModel(model_);

	model_->setColumnCount(1);
}

Hierarchy::~Hierarchy() {
}

void Hierarchy::update(Sprite root) {
	model_->setRowCount(0);
	
	updateRecursively(root, nullptr);

	header()->resizeSections(QHeaderView::ResizeToContents);
}

Sprite Hierarchy::selectedSprite() {
	QModelIndex index = currentIndex();
	if (!index.isValid()) { return nullptr; }

	unsigned id = model_->itemFromIndex(index)->data().toUInt();
	return Engine::get()->world()->GetSprite(id);
}

bool Hierarchy::selectedSprites(QList<Sprite>& sprites) {
	QModelIndexList indexes = selectedIndexes();
	foreach(QModelIndex index, indexes) {
		unsigned id = model_->itemFromIndex(index)->data().toUInt();
		sprites.push_back(Engine::get()->world()->GetSprite(id));
	}

	return !sprites.empty();
}

void Hierarchy::updateRecursively(Sprite pp, QStandardItem* pi) {
	for (int i = 0; i < pp->GetChildCount(); ++i) {
		Sprite child = pp->GetChildAt(i);

		QStandardItem* item = new QStandardItem(child->GetName().c_str());
		item->setData(child->GetInstanceID());

		if (pi != nullptr) {
			pi->appendRow(item);
		}
		else {
			model_->appendRow(item);
		}
		
		updateRecursively(child, item);
	}
}
