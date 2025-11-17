//
// Created by musheng on 11/17/25.
//

#include "Pet.hpp"

#include <QApplication>
#include <QDebug>
#include <QtLogging>
#include <utility>
#include <QPainter>
#include <QAction>
#include <QDir>
#include <QPixmap>
#include <QMouseEvent>

namespace SorenPet {

Pet::Pet(QString name, QWidget *parent) :
	QWidget(parent),
	name_(std::move(name)),
	tray_menu_(new QMenu(this)){

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground);

	loadResources(":/idle/res", AnimationType::IDLE);
	initSystemTray();
	initContextMenu();

	qDebug() << "Pet " << name_ << " start.";

	animation_timer_ = new QTimer(this);
	animation_timer_->start(60);
	connect(animation_timer_, &QTimer::timeout, this, &Pet::updateAnimation);
}

Pet::~Pet() = default;

void Pet::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放

	const QList<QPixmap> &current_frames = is_clicked_ ? click_frames_ : idle_frames_;

	if (!current_frames.isEmpty()) {
		painter.drawPixmap(this->rect(), current_frames[current_frame_index_]);
	}
	qDebug() << "Pet " << name_ << " painted frame " << current_frame_index_ << " of " <<
		(is_clicked_ ? " click frames." : " idle frames.");
}

void Pet::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		is_dragging_ = true;
		qDebug() << "Pet " << name_ << " is being dragged.";
		drag_pos_ = event->pos();
		event->accept();
	}
}
void Pet::mouseMoveEvent(QMouseEvent *event) {
	if (is_dragging_ && static_cast<bool>(event->buttons() & Qt::LeftButton)) {
		const QPoint pos = QPoint(event->globalPosition().x(), event->globalPosition().y()) - drag_pos_;
		this->move(pos);
		qDebug() << "Pet " << name_ << " is dragged to " << pos;
		event->accept();
	}
}
void Pet::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton && is_dragging_) {
		is_dragging_ = false;
		qDebug() << "Pet " << name_ << " stops being dragged.";
	}
	else if (event->button() == Qt::LeftButton) {
		is_clicked_ = true;
		current_frame_index_ = 0;
		qDebug() << "Pet " << name_ << " is clicked, start animation.";
	}
	event->accept();
}
void Pet::contextMenuEvent(QContextMenuEvent *event) {
	qDebug() << "Pet " << name_ << " context menu selected.";
	context_menu_->exec(event->globalPos());
	event->accept();
}

void Pet::updateAnimation() {
	const QList<QPixmap> &frames = is_clicked_ ? click_frames_ : idle_frames_;
	if (frames.isEmpty()) {
		qDebug() << "Pet " << name_ << " has nothing to show.";
		return;
	}
	current_frame_index_ = (current_frame_index_ + 1) % frames.size();
	if (is_clicked_ && current_frame_index_ == 0) {
		qDebug() << "Pet " << name_ << " is clicked and idle now.";
		is_clicked_ = false;
		return;
	}
	this->update();
}

void Pet::onExitAction() const {
	qDebug() << "Pet " << name_ << " quits.";
	QApplication::exit();
}

void Pet::onHideAction() {
	qDebug() << "Pet " << name_ << " is hidden.";
	this->setVisible(false);
}

void Pet::onShowAction() {
	this->show();
}

void Pet::loadResources(const QString &path, AnimationType type) {
	QDir dir(path);
	const QStringList filters = {"*.png"}; // 只加载png图片
	dir.setNameFilters(filters);
	QStringList file_names = dir.entryList();

	// 按文件名中的数字序号排序
	std::sort(file_names.begin(), file_names.end(), [](const QString &a, const QString &b) {
		// 提取文件名中的数字（如"page_5.png"→5）
		int numA = a.split("_").last().split(".").first().toInt();
		int numB = b.split("_").last().split(".").first().toInt();
		return numA < numB;
	});

	QSize target_size(200, 200);
	for (const auto& fileName : file_names) {
		QPixmap pixmap(dir.filePath(fileName));
		if (pixmap.isNull()) continue;

		// 提前缩放+缓存，绘制时直接使用（避免实时计算）
		QPixmap scaledPix = pixmap.scaled(
			target_size,
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation
		);

		switch (type) {
			case AnimationType::IDLE:
				idle_frames_.append(scaledPix);
				break;
			case AnimationType::CLICK:
				click_frames_.append(scaledPix);
				break;
		}
	}

	// 用第一帧的尺寸设置窗口大小（确保适配图片）
	if (!idle_frames_.isEmpty()) {
		setFixedSize(idle_frames_.first().size());
	}
}

void Pet::initSystemTray() {
	system_tray_icon_ = new QSystemTrayIcon(this);
	if (!idle_frames_.isEmpty()) {
		system_tray_icon_->setIcon(QIcon(idle_frames_.first())); // 设置托盘图标
	}
	system_tray_icon_->setToolTip("桌面宠物"); // 鼠标悬停托盘时显示的提示文字
	system_tray_icon_->setContextMenu(tray_menu_); // 给托盘图标绑定右键菜单
	auto tray_show_action = new QAction("Show");
	auto tray_exit_action = new QAction("Exit");
	tray_menu_->addAction(tray_show_action);
	tray_menu_->addAction(tray_exit_action);
	connect(tray_exit_action, &QAction::triggered, this, &Pet::onExitAction);
	connect(tray_show_action, &QAction::triggered, this, &QWidget::show);
	system_tray_icon_->show();
}

void Pet::initContextMenu() {
	context_menu_ = new QMenu(this);
	auto quit_action = new QAction("Quit");
	auto hide_action = new QAction("Hide");
	this->context_menu_->addAction(quit_action);
	this->context_menu_->addAction(hide_action);
	connect(quit_action, &QAction::triggered, this, &Pet::onExitAction);
	connect(hide_action, &QAction::triggered, this, &Pet::onHideAction);
}

} // SorenPet