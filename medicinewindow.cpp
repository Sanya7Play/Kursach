    #include "medicinewindow.h"
#include "qpushbutton.h"
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QScrollArea>

MedicineWindow::MedicineWindow(QWidget *parent) : QWidget(parent), currentIndex(0) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);

    this->setWindowTitle("Медицинские изображения");
    this->setStyleSheet("background-color: white; color: black;");

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(label);

    mainLayout->addWidget(scrollArea);

    // Создаем горизонтальное расположение для кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *loadImagesButton = new QPushButton("Загрузить изображения", this);
    connect(loadImagesButton, &QPushButton::clicked, this, &MedicineWindow::loadingImagesFunction);

    QPushButton *saveCollageButton = new QPushButton("Сохранить коллаж", this);
    connect(saveCollageButton, &QPushButton::clicked, this, &MedicineWindow::preserveImageFunction);

    QPushButton *viewCollageButton = new QPushButton("Посмотреть коллаж", this);
    connect(viewCollageButton, &QPushButton::clicked, this, &MedicineWindow::showImageFunction);

    comboButton = new QComboBox(this);
    comboButton->addItem("Одно изображение");
    comboButton->addItem("Два изображения горизонтально");
    comboButton->addItem("Четыре изображения в сетке 2x2");
    comboButton->addItem("Шесть изображений в сетке 2x3");
    connect(comboButton, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MedicineWindow::changeGridFunction);

    // Добавляем кнопки в горизонтальное расположение
    buttonLayout->addWidget(loadImagesButton);
    buttonLayout->addWidget(saveCollageButton);
    buttonLayout->addWidget(viewCollageButton);
    buttonLayout->addWidget(comboButton);

    // Добавляем горизонтальное расположение с кнопками в основное вертикальное расположение
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    setFixedSize(800, 800);

    currentLayout = OneImage;
}

void MedicineWindow::loadingImagesFunction()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Выберите медицинские изображения", "", "Изображения (*.png *.jpg *.bmp);;Все файлы (*)");

    if (fileNames.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Не выбраны изображения.");
        return;
    }

    images.clear();
    for (const QString &fileName : fileNames) {
        QFileInfo fileInfo(fileName);
        QString ext = fileInfo.suffix().toLower();
        if (!(ext == "png" || ext == "jpg" || ext == "bmp" || ext == "jpeg" || ext == "gif")) {
            QMessageBox::warning(this, "Предупреждение", "Выбран файл с недопустимым расширением. Пожалуйста, выберите файл в поддерживаемом формате (PNG, JPG, BMP, JPEG, GIF).");
            return;
        }

        QPixmap pixmap(fileName);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение: " + fileName);
            return;
        }

        images.push_back(pixmap);
    }

    changeGridFunction();
}

void MedicineWindow::preserveImageFunction()
{
    if (images.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Нет изображений для сохранения.");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Сохранить коллаж", "", "Изображения (*.png)");

    if (savePath.isEmpty()) {
        return;
    }

    QPixmap collage;
    int fixedWidth = label->width();
    int fixedHeight = label->height();

    switch (currentLayout) {
    case OneImage:
        if (!images.isEmpty()) {
            QPixmap scaledImage = images.first().scaled(label->size(), Qt::KeepAspectRatio);
            label->setPixmap(scaledImage);
            collage = scaledImage;
        }
        break;
    case TwoImagesHorizontal:
        if (images.size() >= 2) {
            collage = QPixmap(fixedWidth * 2, fixedHeight);
            collage.fill(Qt::white);

            QPainter painter(&collage);
            painter.drawPixmap(0, 0, images[0].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
            painter.drawPixmap(fixedWidth, 0, images[1].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
        }
        break;
    case FourImagesGrid:
        if (images.size() >= 4) {
            collage = QPixmap(fixedWidth * 2, fixedHeight * 2);
            collage.fill(Qt::white);

            QPainter painter(&collage);
            for (int i = 0; i < qMin(4, images.size()); ++i) {
                int row = i / 2;
                int col = i % 2;
                painter.drawPixmap(col * fixedWidth, row * fixedHeight, images[i].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
            }
        }
        break;
    case SixImagesGrid:
        if (images.size() >= 6) {
            collage = QPixmap(fixedWidth * 2, fixedHeight * 3);
            collage.fill(Qt::white);

            QPainter painter(&collage);
            for (int i = 0; i < qMin(6, images.size()); ++i) {
                int row = i / 2;
                int col = i % 2;
                painter.drawPixmap(col * fixedWidth, row * fixedHeight, images[i].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
            }
        }
        break;
    }

    if (collage.save(savePath)) {
        QMessageBox::information(this, "Сохранение", "Коллаж успешно сохранен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить коллаж.");
    }
}

void MedicineWindow::changeGridFunction()
{
    currentLayout = static_cast<LayoutType>(comboButton->currentIndex());

    int fixedWidth = label->width();
    int fixedHeight = label->height();

    switch (currentLayout) {
    case OneImage:
        if (!images.isEmpty()) {
            QPixmap scaledImage = images.first().scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio);

            // Устанавливаем черный фон
            QPixmap blackBackground(fixedWidth, fixedHeight);
            blackBackground.fill(Qt::black);

            QPainter painter(&blackBackground);
            painter.drawPixmap((fixedWidth - scaledImage.width()) / 2, (fixedHeight - scaledImage.height()) / 2, scaledImage);

            label->setPixmap(blackBackground);
        }
        break;
    case TwoImagesHorizontal:
        if (images.size() >= 2) {
            int halfWidth = fixedWidth / 2;

            QPixmap collage(fixedWidth, fixedHeight);
            collage.fill(Qt::black);  // Set black background

            QPainter painter(&collage);

            QPixmap scaledImage1 = images[0].scaledToWidth(halfWidth);
            painter.drawPixmap(0, 0, scaledImage1);

            QPixmap scaledImage2 = images[1].scaledToWidth(halfWidth);
            painter.drawPixmap(halfWidth, 0, scaledImage2);

            label->setPixmap(collage);
        }
        break;
    case FourImagesGrid:
        if (images.size() >= 4) {
            int imagesPerRow = 2;
            int imagesPerColumn = 2;

            int cellWidth = fixedWidth / imagesPerRow;
            int cellHeight = fixedHeight / imagesPerColumn;

            QPixmap collage(fixedWidth, fixedHeight);
            collage.fill(Qt::black);  // Set black background

            QPainter painter(&collage);
            for (int i = 0; i < qMin(imagesPerRow * imagesPerColumn, images.size()); ++i) {
                int row = i / imagesPerRow;
                int col = i % imagesPerRow;

                int xOffset = col * cellWidth;
                int yOffset = row * cellHeight;

                painter.drawPixmap(xOffset, yOffset, images[i].scaled(cellWidth, cellHeight, Qt::KeepAspectRatio));
            }

            label->setPixmap(collage);
        }
        break;
    case SixImagesGrid:
        if (images.size() >= 6) {
            int imagesPerRow = 2;
            int imagesPerColumn = 3;

            int cellWidth = fixedWidth / imagesPerRow;
            int cellHeight = fixedHeight / imagesPerColumn;

            QPixmap collage(fixedWidth, fixedHeight);
            collage.fill(Qt::black);  // Set black background

            QPainter painter(&collage);
            for (int i = 0; i < qMin(imagesPerRow * imagesPerColumn, images.size()); ++i) {
                int row = i / imagesPerRow;
                int col = i % imagesPerRow;
                painter.drawPixmap(col * cellWidth, row * cellHeight, images[i].scaled(cellWidth, cellHeight, Qt::KeepAspectRatio));
            }

            label->setPixmap(collage);
        }
        break;
    }
}



void MedicineWindow::showImageFunction() {
    if (images.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Нет изображений для просмотра.");
        return;
    }

    QString savePath;

    if (currentLayout == OneImage || currentLayout == TwoImagesHorizontal || currentLayout == FourImagesGrid || currentLayout == SixImagesGrid) {
        savePath = QFileDialog::getSaveFileName(this, "Сохранить коллаж", "", "Изображения (*.png)");

        if (savePath.isEmpty()) {
            return;
        }

        QPixmap collage;
        int fixedWidth = label->width();
        int fixedHeight = label->height();

        if (QFileInfo::exists(savePath)) {
            collage = QPixmap(savePath);
        } else {
            switch (currentLayout) {
            case OneImage:
                if (!images.isEmpty()) {
                    collage = images.first().scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio);
                }
                break;
            case TwoImagesHorizontal:
                if (images.size() >= 2) {
                    collage = QPixmap(fixedWidth * 2, fixedHeight);
                    collage.fill(Qt::white);

                    QPainter painter(&collage);
                    painter.drawPixmap(0, 0, images[0].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
                    painter.drawPixmap(fixedWidth, 0, images[1].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
                }
                break;
            case FourImagesGrid:
                if (images.size() >= 4) {
                    collage = QPixmap(fixedWidth * 2, fixedHeight * 2);
                    collage.fill(Qt::white);

                    QPainter painter(&collage);
                    for (int i = 0; qMin(4, images.size()); ++i) {
                        int row = i / 2;
                        int col = i % 2;
                        painter.drawPixmap(col * fixedWidth, row * fixedHeight, images[i].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
                    }
                }
                break;
            case SixImagesGrid:
                if (images.size() >= 6) {
                    collage = QPixmap(fixedWidth * 2, fixedHeight * 3);
                    collage.fill(Qt::white);

                    QPainter painter(&collage);
                    for (int i = 0; qMin(6, images.size()); ++i) {
                        int row = i / 2;
                        int col = i % 2;
                        painter.drawPixmap(col * fixedWidth, row * fixedHeight, images[i].scaled(fixedWidth, fixedHeight, Qt::KeepAspectRatio));
                    }
                }
                break;
            }

            if (!collage.save(savePath)) {
                QMessageBox::warning(this, "Ошибка", "Не удалось сохранить коллаж.");
                return;
            }
        }
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
}
