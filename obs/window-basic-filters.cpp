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

#include "window-namedialog.hpp"
#include "window-basic-filters.hpp"
#include "display-helpers.hpp"
#include "qt-wrappers.hpp"
#include "obs-app.hpp"

#include <QMessageBox>
#include <QCloseEvent>
#include <string>
#include <QMenu>
#include <QVariant>

using namespace std;

Q_DECLARE_METATYPE(OBSSource);

OBSBasicFilters::OBSBasicFilters(QWidget *parent, OBSSource source_)
	: QDialog                      (parent),
	  source                       (source_),
	  ui                           (new Ui::OBSBasicFilters),
	  addSignal                    (obs_source_get_signal_handler(source),
	                                "filter_add",
					OBSBasicFilters::OBSSourceFilterAdded,
	                                this),
	  removeSignal                 (obs_source_get_signal_handler(source),
	                                "filter_remove",
					OBSBasicFilters::OBSSourceFilterRemoved,
	                                this)
{
	ui->setupUi(this);
	UpdateFilters();

	connect(ui->preview, SIGNAL(DisplayResized()),
			this, SLOT(OnPreviewResized()));
}

void OBSBasicFilters::Init()
{
	gs_init_data init_data = {};

	show();

	QSize previewSize = GetPixelSize(ui->preview);
	init_data.cx      = uint32_t(previewSize.width());
	init_data.cy      = uint32_t(previewSize.height());
	init_data.format  = GS_RGBA;
	QTToGSWindow(ui->preview->winId(), init_data.window);

	display = obs_display_create(&init_data);

	if (display)
		obs_display_add_draw_callback(display,
				OBSBasicFilters::DrawPreview, this);
}

void OBSBasicFilters::UpdatePropertiesView(int row)
{
	delete view;
	view = nullptr;

	if (row == -1)
		return;

	QListWidgetItem *item = ui->filters->item(row);
	QVariant v = item->data(Qt::UserRole);
	OBSSource filter = v.value<OBSSource>();

	obs_data_t *settings = obs_source_get_settings(filter);

	view = new OBSPropertiesView(settings, filter,
			(PropertiesReloadCallback)obs_source_properties,
			(PropertiesUpdateCallback)obs_source_update);

	obs_data_release(settings);

	view->setMaximumHeight(250);
	view->setMinimumHeight(150);
	ui->rightLayout->addWidget(view);
	view->show();
}

void OBSBasicFilters::AddFilter(OBSSource filter)
{
	const char *name = obs_source_get_name(filter);
	QListWidgetItem *item = new QListWidgetItem(QT_UTF8(name));

	item->setData(Qt::UserRole,
			QVariant::fromValue(filter));

	ui->filters->addItem(item);
	ui->filters->setCurrentRow(ui->filters->count() - 1);
}

void OBSBasicFilters::RemoveFilter(OBSSource filter)
{
	for (int i = 0; i < ui->filters->count(); i++) {
		QListWidgetItem *item = ui->filters->item(i);
		QVariant v = item->data(Qt::UserRole);
		OBSSource curFilter = v.value<OBSSource>();

		if (filter == curFilter) {
			delete item;
			break;
		}
	}
}


void OBSBasicFilters::UpdateFilters()
{
	if (!source)
		return;

	ui->filters->clear();

	obs_source_enum_filters(source,
			[] (obs_source_t*, obs_source_t *filter, void *p)
			{
				OBSBasicFilters *window =
					reinterpret_cast<OBSBasicFilters*>(p);

				window->AddFilter(filter);
			}, this);
}

QMenu *OBSBasicFilters::CreateAddFilterPopupMenu()
{
	const char *type;
	bool foundValues = false;
	size_t idx = 0;

	QMenu *popup = new QMenu(QTStr("Add"), this);
	while (obs_enum_filter_types(idx++, &type)) {
		const char *name = obs_source_get_display_name(
				OBS_SOURCE_TYPE_FILTER, type);

		QAction *popupItem = new QAction(QT_UTF8(name), this);
		popupItem->setData(QT_UTF8(type));
		connect(popupItem, SIGNAL(triggered(bool)),
				this, SLOT(AddFilterFromAction()));
		popup->addAction(popupItem);

		foundValues = true;
	}

	if (!foundValues) {
		delete popup;
		popup = nullptr;
	}

	return popup;
}

void OBSBasicFilters::AddNewFilter(const char *id)
{
	if (id && *id) {
		obs_source_t *existing_filter;
		string name = obs_source_get_display_name(
				OBS_SOURCE_TYPE_FILTER, id);

		bool success = NameDialog::AskForName(this,
				QTStr("Basic.Filters.AddFilter.Title"),
				QTStr("Basic.FIlters.AddFilter.Text"), name,
				QT_UTF8(name.c_str()));
		if (!success)
			return;

		if (name.empty()) {
			QMessageBox::information(this,
					QTStr("NoNameEntered.Title"),
					QTStr("NoNameEntered.Text"));
			AddNewFilter(id);
			return;
		}

		existing_filter = obs_source_get_filter_by_name(source,
				name.c_str());
		if (existing_filter) {
			QMessageBox::information(this,
					QTStr("NameExists.Title"),
					QTStr("NameExists.Text"));
			obs_source_release(existing_filter);
			AddNewFilter(id);
			return;
		}

		obs_source_t *filter = obs_source_create(OBS_SOURCE_TYPE_FILTER,
				id, name.c_str(), nullptr);
		if (filter) {
			obs_source_filter_add(source, filter);
			obs_source_release(filter);
		}
	}
}

void OBSBasicFilters::AddFilterFromAction()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (!action)
		return;

	AddNewFilter(QT_TO_UTF8(action->data().toString()));
}

void OBSBasicFilters::OnPreviewResized()
{
	if (resizeTimer)
		killTimer(resizeTimer);
	resizeTimer = startTimer(100);
}

void OBSBasicFilters::closeEvent(QCloseEvent *event)
{
	QDialog::closeEvent(event);
	if (!event->isAccepted())
		return;

	// remove draw callback and release display in case our drawable
	// surfaces go away before the destructor gets called
	obs_display_remove_draw_callback(display,
			OBSBasicFilters::DrawPreview, this);
	display = nullptr;
}

void OBSBasicFilters::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == resizeTimer) {
		killTimer(resizeTimer);
		resizeTimer = 0;

		QSize size = GetPixelSize(ui->preview);
		obs_display_resize(display, size.width(), size.height());
	}
}

/* OBS Signals */

void OBSBasicFilters::OBSSourceFilterAdded(void *param, calldata_t *data)
{
	OBSBasicFilters *window = reinterpret_cast<OBSBasicFilters*>(param);
	obs_source_t *filter = (obs_source_t*)calldata_ptr(data, "filter");

	QMetaObject::invokeMethod(window, "AddFilter",
			Q_ARG(OBSSource, OBSSource(filter)));
}

void OBSBasicFilters::OBSSourceFilterRemoved(void *param, calldata_t *data)
{
	OBSBasicFilters *window = reinterpret_cast<OBSBasicFilters*>(param);
	obs_source_t *filter = (obs_source_t*)calldata_ptr(data, "filter");

	QMetaObject::invokeMethod(window, "RemoveFilter",
			Q_ARG(OBSSource, OBSSource(filter)));
}

void OBSBasicFilters::DrawPreview(void *data, uint32_t cx, uint32_t cy)
{
	OBSBasicFilters *window = static_cast<OBSBasicFilters*>(data);

	if (!window->source)
		return;

	uint32_t sourceCX = max(obs_source_get_width(window->source), 1u);
	uint32_t sourceCY = max(obs_source_get_height(window->source), 1u);

	int   x, y;
	int   newCX, newCY;
	float scale;

	GetScaleAndCenterPos(sourceCX, sourceCY, cx, cy, x, y, scale);

	newCX = int(scale * float(sourceCX));
	newCY = int(scale * float(sourceCY));

	gs_viewport_push();
	gs_projection_push();
	gs_ortho(0.0f, float(sourceCX), 0.0f, float(sourceCY), -100.0f, 100.0f);
	gs_set_viewport(x, y, newCX, newCY);

	obs_source_video_render(window->source);

	gs_projection_pop();
	gs_viewport_pop();
}

/* Qt Slots */

void OBSBasicFilters::on_addFilter_clicked()
{
	QPointer<QMenu> popup = CreateAddFilterPopupMenu();
	if (popup)
		popup->exec(QCursor::pos());
}

void OBSBasicFilters::on_removeFilter_clicked()
{
	int row = ui->filters->currentRow();
	if (row == -1)
		return;

	QListWidgetItem *item = ui->filters->item(row);
	QVariant v = item->data(Qt::UserRole);
	OBSSource filter = v.value<OBSSource>();

	obs_source_filter_remove(source, filter);
}

void OBSBasicFilters::on_filters_currentRowChanged(int row)
{
	UpdatePropertiesView(row);
}
