/******************************************************************************
    Copyright (C) 2015 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <memory>
#include <obs.hpp>

#include "properties-view.hpp"

class OBSBasic;
class QMenu;

#include "ui_OBSBasicFilters.h"

class OBSBasicFilters : public QDialog {
	Q_OBJECT

private:
	OBSBasic *main;

	std::unique_ptr<Ui::OBSBasicFilters> ui;
	int resizeTimer = 0;
	OBSSource source;
	OBSPropertiesView *view = nullptr;

	OBSDisplay display;
	OBSSignal addSignal;
	OBSSignal removeSignal;

	void UpdateFilters();
	void UpdatePropertiesView(int row);

	static void OBSSourceFilterAdded(void *param, calldata_t *data);
	static void OBSSourceFilterRemoved(void *param, calldata_t *data);
	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);

	QMenu *CreateAddFilterPopupMenu();

	void AddNewFilter(const char *id);

private slots:
	void AddFilter(OBSSource filter);
	void RemoveFilter(OBSSource filter);

	void AddFilterFromAction();

	void OnPreviewResized();

	void on_addFilter_clicked();
	void on_removeFilter_clicked();
	void on_filters_currentRowChanged(int row);

public:
	OBSBasicFilters(QWidget *parent, OBSSource source_);

	void Init();

protected:
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void timerEvent(QTimerEvent *event) override;
};
