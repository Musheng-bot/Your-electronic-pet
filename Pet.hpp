//
// Created by musheng on 11/17/25.
//

#ifndef PET_PET_HPP
#define PET_PET_HPP

#include <QVector>
#include <QTimer>
#include <QString>
#include <QMenu>
#include <QSystemTrayIcon>

namespace SorenPet {
class Pet final : public QWidget{
	Q_OBJECT
	public:
		explicit Pet(QString name, QWidget *parent = nullptr);
		~Pet() override;

		enum class AnimationType {
			IDLE,
			CLICK,
		};

	protected:
		void paintEvent(QPaintEvent *event) override;				// 绘制宠物图像
		void mousePressEvent(QMouseEvent *event) override;			// 鼠标按下（用于拖动）
		void mouseMoveEvent(QMouseEvent *event) override;			// 鼠标移动（拖动窗口）
		void mouseReleaseEvent(QMouseEvent *event) override;		// 鼠标释放（点击交互）
		void contextMenuEvent(QContextMenuEvent *event) override;	// 右键菜单

	private slots:
		void updateAnimation();			// 更新动画帧
		void onExitAction() const;      // 退出程序
		void onHideAction();			// 隐藏宠物
		void onShowAction();			// 显示宠物

	private:
		void loadResources(const QString &path, AnimationType type);
		void initSystemTray();
		void initContextMenu();


	private:
		QString name_;					//元数据，宠物名称

		bool is_dragging_ = false;		//是否正在被拖拽
		QPoint drag_pos_;				//标识鼠标光标相对窗口的位置

		QTimer *animation_timer_ = nullptr;	//每隔一段时间更新动画帧
		qsizetype current_frame_index_ = 0;	//当前播放到哪一帧

		QVector<QPixmap> idle_frames_;		//闲置时的动画
		QVector<QPixmap> click_frames_;		//点击后的动画
		bool is_clicked_ = false;			//是否被点击，用于防止重复触发动画
		QMenu *context_menu_ = nullptr;		//右键菜单

		QSystemTrayIcon *system_tray_icon_ = nullptr;	//系统托盘图标
		QMenu *tray_menu_ = nullptr;					//系统托盘右键目录
};

} // SorenPet

#endif //PET_PET_HPP