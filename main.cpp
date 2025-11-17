#include <QApplication>
#include "Pet.hpp"



int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	SorenPet::Pet widget("MyPet");
	widget.show();
	return QApplication::exec();
}