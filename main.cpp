#include "medicinewindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedicineWindow medicinewindow;
    // Получаем размер главного монитора

    medicinewindow.show();
    return a.exec();
}
