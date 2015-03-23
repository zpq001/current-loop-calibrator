/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "textdisplay.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    TextDisplay *TextDisplay1;
    QPushButton *pb_num1;
    QPushButton *pb_num2;
    QPushButton *pb_num3;
    QPushButton *pb_num4;
    QPushButton *pb_num5;
    QPushButton *pb_num6;
    QPushButton *pb_num7;
    QPushButton *pb_num8;
    QPushButton *pb_num9;
    QPushButton *pb_num0;
    QPushButton *pb_dot;
    QPushButton *pb_bkspace;
    QPushButton *pb_Ok;
    QPushButton *pb_Esc;
    QPushButton *pb_Const;
    QPushButton *pb_Altern;
    QPushButton *pb_Update;
    QPushButton *pb_OutCtrl;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(775, 329);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        TextDisplay1 = new TextDisplay(centralWidget);
        TextDisplay1->setObjectName(QStringLiteral("TextDisplay1"));
        TextDisplay1->setGeometry(QRect(30, 20, 450, 180));
        pb_num1 = new QPushButton(centralWidget);
        pb_num1->setObjectName(QStringLiteral("pb_num1"));
        pb_num1->setGeometry(QRect(510, 20, 31, 31));
        pb_num2 = new QPushButton(centralWidget);
        pb_num2->setObjectName(QStringLiteral("pb_num2"));
        pb_num2->setGeometry(QRect(560, 20, 31, 31));
        pb_num3 = new QPushButton(centralWidget);
        pb_num3->setObjectName(QStringLiteral("pb_num3"));
        pb_num3->setGeometry(QRect(610, 20, 31, 31));
        pb_num4 = new QPushButton(centralWidget);
        pb_num4->setObjectName(QStringLiteral("pb_num4"));
        pb_num4->setGeometry(QRect(510, 70, 31, 31));
        pb_num5 = new QPushButton(centralWidget);
        pb_num5->setObjectName(QStringLiteral("pb_num5"));
        pb_num5->setGeometry(QRect(560, 70, 31, 31));
        pb_num6 = new QPushButton(centralWidget);
        pb_num6->setObjectName(QStringLiteral("pb_num6"));
        pb_num6->setGeometry(QRect(610, 70, 31, 31));
        pb_num7 = new QPushButton(centralWidget);
        pb_num7->setObjectName(QStringLiteral("pb_num7"));
        pb_num7->setGeometry(QRect(510, 120, 31, 31));
        pb_num8 = new QPushButton(centralWidget);
        pb_num8->setObjectName(QStringLiteral("pb_num8"));
        pb_num8->setGeometry(QRect(560, 120, 31, 31));
        pb_num9 = new QPushButton(centralWidget);
        pb_num9->setObjectName(QStringLiteral("pb_num9"));
        pb_num9->setGeometry(QRect(610, 120, 31, 31));
        pb_num0 = new QPushButton(centralWidget);
        pb_num0->setObjectName(QStringLiteral("pb_num0"));
        pb_num0->setGeometry(QRect(510, 170, 31, 31));
        pb_dot = new QPushButton(centralWidget);
        pb_dot->setObjectName(QStringLiteral("pb_dot"));
        pb_dot->setGeometry(QRect(560, 170, 31, 31));
        pb_bkspace = new QPushButton(centralWidget);
        pb_bkspace->setObjectName(QStringLiteral("pb_bkspace"));
        pb_bkspace->setGeometry(QRect(610, 170, 31, 31));
        pb_Ok = new QPushButton(centralWidget);
        pb_Ok->setObjectName(QStringLiteral("pb_Ok"));
        pb_Ok->setGeometry(QRect(680, 20, 61, 31));
        pb_Esc = new QPushButton(centralWidget);
        pb_Esc->setObjectName(QStringLiteral("pb_Esc"));
        pb_Esc->setGeometry(QRect(680, 70, 61, 31));
        pb_Const = new QPushButton(centralWidget);
        pb_Const->setObjectName(QStringLiteral("pb_Const"));
        pb_Const->setGeometry(QRect(680, 120, 61, 31));
        pb_Altern = new QPushButton(centralWidget);
        pb_Altern->setObjectName(QStringLiteral("pb_Altern"));
        pb_Altern->setGeometry(QRect(680, 170, 61, 31));
        pb_Update = new QPushButton(centralWidget);
        pb_Update->setObjectName(QStringLiteral("pb_Update"));
        pb_Update->setGeometry(QRect(30, 220, 91, 31));
        pb_OutCtrl = new QPushButton(centralWidget);
        pb_OutCtrl->setObjectName(QStringLiteral("pb_OutCtrl"));
        pb_OutCtrl->setGeometry(QRect(610, 220, 131, 31));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 775, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        pb_num1->setText(QApplication::translate("MainWindow", "1", 0));
        pb_num2->setText(QApplication::translate("MainWindow", "2", 0));
        pb_num3->setText(QApplication::translate("MainWindow", "3", 0));
        pb_num4->setText(QApplication::translate("MainWindow", "4", 0));
        pb_num5->setText(QApplication::translate("MainWindow", "5", 0));
        pb_num6->setText(QApplication::translate("MainWindow", "6", 0));
        pb_num7->setText(QApplication::translate("MainWindow", "7", 0));
        pb_num8->setText(QApplication::translate("MainWindow", "8", 0));
        pb_num9->setText(QApplication::translate("MainWindow", "9", 0));
        pb_num0->setText(QApplication::translate("MainWindow", "0", 0));
        pb_dot->setText(QApplication::translate("MainWindow", ".", 0));
        pb_bkspace->setText(QApplication::translate("MainWindow", "<-", 0));
        pb_Ok->setText(QApplication::translate("MainWindow", "\320\222\320\262\320\276\320\264", 0));
        pb_Esc->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\274\320\265\320\275\320\260", 0));
        pb_Const->setText(QApplication::translate("MainWindow", "\320\237\320\276\321\201\321\202", 0));
        pb_Altern->setText(QApplication::translate("MainWindow", "\320\230\320\274\320\277", 0));
        pb_Update->setText(QApplication::translate("MainWindow", "Update", 0));
        pb_OutCtrl->setText(QApplication::translate("MainWindow", "\320\240\320\265\321\201\321\202\320\260\321\200\321\202/\320\277\321\200\320\276\321\204\320\270\320\273\321\214", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
