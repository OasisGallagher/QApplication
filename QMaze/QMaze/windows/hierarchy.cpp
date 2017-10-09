#include <QHeaderView>
#include <QStandardItemModel>

#include "sprite.h"
#include "hierarchy.h"

Hierarchy::Hierarchy(QWidget* parent) {
	model_ = new QStandardItemModel(this);
	setModel(model_);

	model_->setColumnCount(2);
}

Hierarchy::~Hierarchy() {
}

void Hierarchy::update(const std::vector<Sprite>& sprites) {
	model_->setRowCount(0);
	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		if (updated_.contains(sprite->GetInstanceID())) {
			continue;
		}

		updated_.insert(sprite->GetInstanceID());
		QStandardItem* item = new QStandardItem(sprite->GetName().c_str());
		model_->appendRow(item);

		updateRecursively(sprite, item);
	}

	updated_.clear();
	collapseAll();
	header()->resizeSections(QHeaderView::ResizeToContents);
}

void Hierarchy::updateRecursively(Sprite pp, QStandardItem* pi) {
	for (int i = 0; i < pp->GetChildCount(); ++i) {
		Sprite child = pp->GetChildAt(i);
		if (updated_.contains(child->GetInstanceID())) {
			continue;
		}

		updated_.insert(child->GetInstanceID());
		QStandardItem* item = new QStandardItem(child->GetName().c_str());
		pi->appendRow(item);

		updateRecursively(child, item);
	}
}
