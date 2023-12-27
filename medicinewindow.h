#ifndef MEDICINEWINDOW_H
#define MEDICINEWINDOW_H

#include "qlabel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVector>

class MedicineWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineWindow(QWidget *parent = nullptr);

private slots:
    void loadingImagesFunction();
    void preserveImageFunction();
    void showImageFunction();
    void changeGridFunction();


private:
    QVBoxLayout *peremen;
    QLabel *label;
    QComboBox *comboButton;
    enum LayoutType {
        OneImage,
        TwoImagesHorizontal,
        FourImagesGrid,
        SixImagesGrid
    };
    LayoutType currentLayout;
    QVector<QPixmap> images;
    int currentIndex;

};

#endif // MEDICINEWINDOW_H
