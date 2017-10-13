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

void Hierarchy::update(Sprite root) {
	model_->setRowCount(0);
	
	updateRecursively(root, nullptr);

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void Hierarchy::updateRecursively(Sprite pp, QStandardItem* pi) {
	for (int i = 0; i < pp->GetChildCount(); ++i) {
		Sprite child = pp->GetChildAt(i);

		QStandardItem* item = new QStandardItem(child->GetName().c_str());
		if (pi != nullptr) {
			pi->appendRow(item);
		}
		else {
			model_->appendRow(item);
		}

		updateRecursively(child, item);
	}
}
